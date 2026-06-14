package erdg.pipeline;

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Deque;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeSet;

import erdg.ast.AnalysisResult;
import erdg.ast.AnalysisResult.MessageServer;
import erdg.ast.AnalysisResult.RebecInstance;
import erdg.ast.AnalysisResult.Send;
import erdg.erdg.ERDG;
import erdg.model.ActivationNode;

/**
 * Step 2 of the priority assignment pipeline.
 *
 * Partitions rebecs into interference groups (connected components of RIG),
 * then builds the Hierarchical Rebec Group Graph (HRG):
 *   - Nodes: interference groups
 *   - Edges: directed precedence constraints derived from causal structure,
 *            transitive-reduced, with bidirectional cycles broken using the
 *            constructor self-message heuristic.
 *
 * Java port of the Python ERDGTestGenerator.step2_identify_rebec_groups_and_build_hrg()
 * and all its private helpers.
 */
public class Step2HierarchicalRebecGraph {

    private final ERDG erdg;
    private final AnalysisResult analysis;
    private final RebecDependencyGraph rig;

    /** Cached rebec adjacency, useful in Step 3. */
    public Map<String, Set<String>> rebecAdjacencyCache = new HashMap<>();

    public Step2HierarchicalRebecGraph(ERDG erdg, AnalysisResult analysis,
                                       RebecDependencyGraph rig) {
        this.erdg = erdg;
        this.analysis = analysis;
        this.rig = rig;
    }

    public HRG build() {
        System.out.println("\n=== Step 2: Building HRG ===");

        // 1. Connected components of RIG -> interference groups
        List<List<String>> groups = rig.connectedComponents();
        for (int i = 0; i < groups.size(); i++) {
            System.out.println("  Group " + (i + 1) + ": " + groups.get(i));
        }

        // 2. Direct rebec adjacency (who sends to whom)
        Map<String, Set<String>> rebecAdj = buildRebecAdjacency();
        this.rebecAdjacencyCache = rebecAdj;

        // 3. Lift to group level
        List<Set<Integer>> groupAdj = groupDirectAdj(rebecAdj, groups);

        // 4. Remove transitive edges (keep only immediate links)
        groupAdj = transitiveReduction(groupAdj);

        // 5. Break bidirectional edges (cycles of length 2)
        breakBidirectionalEdges(groupAdj, groups);

        // 6. Emit final HRG edges
        List<HRG.Edge> edges = new ArrayList<>();
        for (int u = 0; u < groupAdj.size(); u++) {
            for (int v : groupAdj.get(u)) {
                edges.add(new HRG.Edge(u, v));
                System.out.println("  HRG edge: Group " + (u + 1) + " -> Group " + (v + 1));
            }
        }

        return new HRG(groups, edges);
    }

    // =========================================================================
    // Direct rebec adjacency
    // =========================================================================

    /**
     * Build a direct adjacency map: rebec -> set of rebecs it sends messages to.
     * Self-sends are included (e.g., 's' may send to 's').
     */
    private Map<String, Set<String>> buildRebecAdjacency() {
        Map<String, Set<String>> adj = new HashMap<>();
        for (ActivationNode a : erdg.activations) {
            adj.computeIfAbsent(a.senderRebec(), k -> new HashSet<>()).add(a.targetRebec());
        }
        return adj;
    }

    // =========================================================================
    // Lift to group level
    // =========================================================================

    /**
     * Lift rebec adjacency to group adjacency:
     *   group Gi -> group Gj  iff some rebec in Gi sends directly to some
     *                              rebec in Gj  (and Gi != Gj).
     */
    private List<Set<Integer>> groupDirectAdj(
            Map<String, Set<String>> rebecAdj,
            List<List<String>> groups) {
        int n = groups.size();
        List<Set<Integer>> adj = new ArrayList<>();
        for (int i = 0; i < n; i++) adj.add(new LinkedHashSet<>());

        // Map each rebec to its group index
        Map<String, Integer> rebec2group = new HashMap<>();
        for (int gi = 0; gi < n; gi++) {
            for (String a : groups.get(gi)) rebec2group.put(a, gi);
        }

        for (var entry : rebecAdj.entrySet()) {
            String src = entry.getKey();
            Integer gi = rebec2group.get(src);
            if (gi == null) continue;
            for (String tgt : entry.getValue()) {
                Integer gj = rebec2group.get(tgt);
                if (gj != null && !gi.equals(gj)) {
                    adj.get(gi).add(gj);
                }
            }
        }
        return adj;
    }

    // =========================================================================
    // Transitive reduction
    // =========================================================================

    /**
     * Remove transitive edges: keep only direct (immediate) causal links.
     * An edge u -> v is kept iff there is NO alternative path of length >= 2
     * from u to v in the original adjacency.
     */
    private List<Set<Integer>> transitiveReduction(List<Set<Integer>> adj) {
        int n = adj.size();
        List<Set<Integer>> reduced = new ArrayList<>();
        for (int i = 0; i < n; i++) reduced.add(new LinkedHashSet<>());

        for (int u = 0; u < n; u++) {
            for (int v : new ArrayList<>(adj.get(u))) {
                // Try to reach v from u without using the direct edge u -> v
                Deque<Integer> stack = new ArrayDeque<>();
                Set<Integer> visited = new HashSet<>();
                for (int w : adj.get(u)) if (w != v) stack.push(w);
                boolean found = false;
                while (!stack.isEmpty()) {
                    int x = stack.pop();
                    if (x == v) { found = true; break; }
                    for (int nei : adj.get(x)) {
                        if (visited.add(nei)) stack.push(nei);
                    }
                }
                if (!found) reduced.get(u).add(v);
            }
        }
        return reduced;
    }

    // =========================================================================
    // Break bidirectional edges
    // =========================================================================

    /**
     * Resolve length-2 cycles (mutual edges u <-> v) by keeping only one direction.
     * Preference: the group whose rebec has a self-message in its constructor
     * (the natural execution initiator). If both or neither qualify, keep the
     * lower-index group first.
     */
    private void breakBidirectionalEdges(List<Set<Integer>> adj, List<List<String>> groups) {
        int n = adj.size();
        for (int u = 0; u < n; u++) {
            for (int v : new ArrayList<>(adj.get(u))) {
                if (u < v && adj.get(v).contains(u)) {
                    boolean uStarter = groupHasConstructorSelf(groups.get(u));
                    boolean vStarter = groupHasConstructorSelf(groups.get(v));
                    if (uStarter && !vStarter) {
                        adj.get(v).remove(u);   // keep u -> v
                    } else if (vStarter && !uStarter) {
                        adj.get(u).remove(v);   // keep v -> u
                    } else {
                        adj.get(v).remove(u);   // tie: lower index first
                    }
                }
            }
        }
    }

    /**
     * Return true iff at least one rebec in the given group is an execution
     * initiator (its constructor sends a self-message).
     */
    boolean groupHasConstructorSelf(List<String> group) {
        for (String rebecName : group) {
            RebecInstance inst = findInstance(rebecName);
            if (inst == null) continue;
            if (hasSelfSendInConstructor(inst.reactiveClass())) return true;
        }
        return false;
    }

    /**
     * Check whether the constructor of the given class contains a self-send.
     * Convention: the constructor method shares its name with the class
     * (case-insensitive match).
     */
    boolean hasSelfSendInConstructor(String rebecClass) {
        var cls = analysis.classes().get(rebecClass);
        if (cls == null) return false;
        for (var entry : cls.messageServers().entrySet()) {
            String methodName = entry.getKey();
            if (methodName.equalsIgnoreCase(rebecClass)) {
                MessageServer m = entry.getValue();
                for (Send s : m.sends()) {
                    if (s.target().equalsIgnoreCase("self")) return true;
                }
            }
        }
        return false;
    }

    private RebecInstance findInstance(String name) {
        for (RebecInstance i : analysis.mainInstances()) {
            if (i.name().equals(name)) return i;
        }
        return null;
    }

    /**
     * Sorted view of all group indices, used by Step 3.
     */
    public static Set<Integer> allGroupIndices(int n) {
        Set<Integer> s = new TreeSet<>();
        for (int i = 0; i < n; i++) s.add(i);
        return s;
    }
}