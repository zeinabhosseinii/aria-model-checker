package erdg.pipeline;

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Deque;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeSet;

import erdg.ast.AnalysisResult;
import erdg.erdg.ERDG;
import erdg.model.RebecNode;

/**
 * Step 3 of the priority assignment pipeline.
 *
 * Assigns a global numeric priority to each rebec instance by topologically
 * sorting the HRG (augmented with full reachability edges):
 *   - Rebecs in the same interference group receive equal priority
 *     (preserves non-determinism where required by SDI).
 *   - Rebecs in different groups receive strictly ordered priorities
 *     (enables the model checker to prune redundant interleavings).
 *
 * Cycles, if any, are broken using the constructor self-message heuristic,
 * falling back to a sink-first heuristic (most incoming reachability edges)
 * to ensure rebecs at the end of the causal chain get lower priority than
 * rebecs that initiate messages toward them.
 *
 * Java port of the Python ERDGTestGenerator.step3_assign_priorities_to_rebecs().
 */
public class Step3RebecPriorities {

    private final ERDG erdg;
    private final AnalysisResult analysis;
    private final HRG hrg;
    private final Step2HierarchicalRebecGraph step2;

    public Step3RebecPriorities(ERDG erdg, AnalysisResult analysis,
                                HRG hrg, Step2HierarchicalRebecGraph step2) {
        this.erdg = erdg;
        this.analysis = analysis;
        this.hrg = hrg;
        this.step2 = step2;
    }

    // =========================================================================
    // Public API
    // =========================================================================

    /**
     * Assign priorities and return a map: rebecInstanceName -> priority (1, 2, ...).
     */
    public Map<String, Integer> assign() {
        System.out.println("\n=== Step 3: Assigning Rebec Priorities ===");

        // Combine HRG immediate edges with full reachability edges
        Set<HRG.Edge> combined = new LinkedHashSet<>(hrg.edges);

        // Separate connected vs. isolated groups
        Set<Integer> connectedGroups = new TreeSet<>();
        for (HRG.Edge e : combined) {
            connectedGroups.add(e.from());
            connectedGroups.add(e.to());
        }

        int n = hrg.groups.size();
        Set<Integer> allGroups = new TreeSet<>();
        for (int i = 0; i < n; i++) allGroups.add(i);

        Set<Integer> isolatedGroups = new TreeSet<>(allGroups);
        isolatedGroups.removeAll(connectedGroups);

        // Topologically order the connected groups, then append isolated ones
        List<Integer> order = topoWithCycleBreaking(new ArrayList<>(connectedGroups), combined);
        order.addAll(isolatedGroups);

        // Assign priorities
        Map<String, Integer> assignment = new LinkedHashMap<>();
        int priority = 1;
        for (int gi : order) {
            for (String Rebec : hrg.groups.get(gi)) {
                assignment.put(Rebec, priority);
            }
            System.out.println("  Group " + (gi + 1) + ": " + hrg.groups.get(gi)
                    + " -> Priority " + priority);
            priority++;
        }

        System.out.println("✅ Rebec priorities: " + assignment);
        return assignment;
    }

    // =========================================================================
    // Reachability-based augmentation
    // =========================================================================

    /**
     * Compute group-level precedence edges implied by the full transitive
     * closure of the rebec adjacency. These are added on top of the immediate
     * HRG edges before topological sorting.
     */
    private Set<HRG.Edge> computeGroupReachabilityEdges() {
        Map<String, Set<String>> rebecAdj = step2.rebecAdjacencyCache;
        List<String> rebecs = new ArrayList<>();
        for (RebecNode r : erdg.rebecs) rebecs.add(r.name());

        // Compute the reachable set for each rebec via BFS/DFS on the rebec adjacency graph
        Map<String, Set<String>> reach = new HashMap<>();
        for (String a : rebecs) {
            Set<String> seen = new HashSet<>();
            seen.add(a);
            Deque<String> stack = new ArrayDeque<>();
            stack.push(a);
            while (!stack.isEmpty()) {
                String x = stack.pop();
                Set<String> nbrs = rebecAdj.getOrDefault(x, Set.of());
                for (String y : nbrs) {
                    if (seen.add(y)) stack.push(y);
                }
            }
            reach.put(a, seen);
        }

        // Map each rebec to its group index
        Map<String, Integer> rebec2group = new HashMap<>();
        for (int gi = 0; gi < hrg.groups.size(); gi++) {
            for (String a : hrg.groups.get(gi)) rebec2group.put(a, gi);
        }

        Set<HRG.Edge> edges = new TreeSet<>((e1, e2) -> {
            int c = Integer.compare(e1.from(), e2.from());
            return c != 0 ? c : Integer.compare(e1.to(), e2.to());
        });

        // Add a group-level edge for every cross-group reachability pair
        for (String a : rebecs) {
            Integer gi = rebec2group.get(a);
            if (gi == null) continue;
            for (String b : reach.get(a)) {
                Integer gj = rebec2group.get(b);
                if (gj != null && !gi.equals(gj)) {
                    edges.add(new HRG.Edge(gi, gj));
                }
            }
        }
        return edges;
    }

    // =========================================================================
    // Topological sort with cycle-breaking
    // =========================================================================

    /**
     * Topologically sort the given subset of group indices using Kahn's algorithm.
     * When a cycle is detected (no zero-indegree node remains), pickBreakNode()
     * selects which group to forcibly emit next.
     */
    private List<Integer> topoWithCycleBreaking(List<Integer> nodes, Set<HRG.Edge> edges) {
        Set<Integer> nset = new HashSet<>(nodes);

        // Build local adjacency and in-degree maps for this subgraph
        Map<Integer, Set<Integer>> adj = new HashMap<>();
        Map<Integer, Integer> indeg = new HashMap<>();
        for (int u : nodes) {
            adj.put(u, new LinkedHashSet<>());
            indeg.put(u, 0);
        }
        for (HRG.Edge e : edges) {
            if (!nset.contains(e.from()) || !nset.contains(e.to())) continue;
            if (e.from() == e.to()) continue;
            if (adj.get(e.from()).add(e.to())) {
                indeg.put(e.to(), indeg.get(e.to()) + 1);
            }
        }

        // Snapshot the original in-degree for use in the pickBreakNode heuristic
        Map<Integer, Integer> originalIndeg = new HashMap<>(indeg);

        System.out.println("  [DEBUG] indeg: " + originalIndeg);
        System.out.println("  [DEBUG] adj: " + adj);

        Deque<Integer> queue = new ArrayDeque<>();
        for (int u : nodes) if (indeg.get(u) == 0) queue.offer(u);

        List<Integer> order = new ArrayList<>();
        Set<Integer> remaining = new TreeSet<>(nodes);

        while (!remaining.isEmpty()) {
            Integer u;
            if (!queue.isEmpty()) {
                u = queue.poll();
            } else {
                // Cycle detected — pick the best node to break it
                u = pickBreakNode(remaining, originalIndeg);
                System.out.println("  ⚠️ Cycle detected. Breaking at Group " + (u + 1) + ".");
            }
            order.add(u);
            remaining.remove(u);
            for (int v : new ArrayList<>(adj.get(u))) {
                indeg.put(v, indeg.get(v) - 1);
                if (remaining.contains(v) && indeg.get(v) == 0) {
                    queue.offer(v);
                }
            }
        }
        return order;
    }

    /**
     * Pick which group to emit when a cycle must be broken.
     *
     * Selection priority:
     *   1. Constructor self-message group (execution initiator) — emit first
     *      so it receives a higher (lower-numbered) priority.
     *   2. Outgoing-edge heuristic: the group with the most outgoing HRG edges
     *      sends messages to others and should therefore have a higher priority,
     *      so it is emitted first among cycle members.
     *   3. Tie-break: smallest group index.
     */
    private Integer pickBreakNode(Set<Integer> remaining,
                                  Map<Integer, Integer> originalIndeg) {
        // Heuristic 1: prefer the group that has a constructor self-message (execution initiator)
        List<Integer> starters = new ArrayList<>();
        for (int g : remaining) {
            if (step2.groupHasConstructorSelf(hrg.groups.get(g))) starters.add(g);
        }
        if (!starters.isEmpty()) {
            starters.sort(Integer::compareTo);
            return starters.get(0);
        }

        // Heuristic 2: prefer the group with the most outgoing HRG edges,
        // as it sends messages to others and should have a higher priority
        int maxOut = -1;
        Integer best = null;
        for (int g : remaining) {
            int outDeg = 0;
            for (HRG.Edge e : hrg.edges) {
                if (e.from() == g && remaining.contains(e.to())) outDeg++;
            }
            if (outDeg > maxOut || (outDeg == maxOut && (best == null || g < best))) {
                maxOut = outDeg;
                best = g;
            }
        }
        return best != null ? best : new TreeSet<>(remaining).first();
    }
}