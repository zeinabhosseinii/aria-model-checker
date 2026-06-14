package erdg.pipeline;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import erdg.ast.AnalysisResult;
import erdg.erdg.ERDG;
import erdg.erdg.SVIAnalyzer;

/**
 * Step 4 of the priority assignment pipeline.
 *
 * For each reactive class, partitions its message servers into
 * SVI-based interference components:
 *   - Message servers in the same component get equal priority.
 *   - Message servers in different components get strictly ordered priorities.
 *
 * Constructor methods (named after the class) are excluded.
 *
 * Java port of ERDGTestGenerator.step4_identify_message_dependency_components().
 */
public class Step4MessageServerPriorities {

    private final ERDG erdg;
    private final AnalysisResult analysis;
    private final SVIAnalyzer svi;

    public Step4MessageServerPriorities(ERDG erdg, AnalysisResult analysis, SVIAnalyzer svi) {
        this.erdg = erdg;
        this.analysis = analysis;
        this.svi = svi;
    }

    /**
     * Returns: Map<className, Map<methodName, priority>>
     */
    public Map<String, Map<String, Integer>> assign() {
        System.out.println("\n=== Step 4: Intra-Rebec Message Server Priorities ===");

        Map<String, Map<String, Integer>> result = new LinkedHashMap<>();

        for (var classEntry : analysis.classes().entrySet()) {
            String className = classEntry.getKey();
            var cls = classEntry.getValue();

            // Exclude constructor
            List<String> methods = new ArrayList<>();
            for (String m : cls.messageServers().keySet()) {
                if (!m.equalsIgnoreCase(className)) methods.add(m);
            }

            // Find SVI pairs using any instance of this class
            List<String[]> sviPairs = new ArrayList<>();
            for (int i = 0; i < methods.size(); i++) {
                for (int j = i + 1; j < methods.size(); j++) {
                    String m1 = methods.get(i);
                    String m2 = methods.get(j);
                    for (var inst : analysis.mainInstances()) {
                        if (inst.reactiveClass().equals(className)) {
                            if (svi.haveSvi(inst.name() + "." + m1, inst.name() + "." + m2)) {
                                sviPairs.add(new String[]{m1, m2});
                                System.out.println("  SVI: " + className + "."
                                        + m1 + " <-> " + className + "." + m2);
                                break;
                            }
                        }
                    }
                }
            }

            // Cluster into SVI-connected components
            List<List<String>> groups = groupBySvi(sviPairs);

            // Independent methods get their own group
            Set<String> assigned = new HashSet<>();
            for (var g : groups) assigned.addAll(g);
            for (String m : methods) {
                if (!assigned.contains(m)) groups.add(List.of(m));
            }

            // Assign priorities
            Map<String, Integer> methodPriorities = new LinkedHashMap<>();
            int priority = 1;
            for (var group : groups) {
                for (String m : group) methodPriorities.put(m, priority);
                priority++;
            }

            result.put(className, methodPriorities);
            System.out.println("  " + className + ": " + methodPriorities);
        }

        return result;
    }

    private List<List<String>> groupBySvi(List<String[]> sviPairs) {
        Map<String, Set<String>> neighbors = new HashMap<>();
        for (String[] pair : sviPairs) {
            neighbors.computeIfAbsent(pair[0], k -> new HashSet<>()).add(pair[1]);
            neighbors.computeIfAbsent(pair[1], k -> new HashSet<>()).add(pair[0]);
        }
        Set<String> visited = new HashSet<>();
        List<List<String>> groups = new ArrayList<>();
        for (String node : neighbors.keySet()) {
            if (!visited.contains(node)) {
                List<String> group = new ArrayList<>();
                dfs(node, neighbors, visited, group);
                groups.add(group);
            }
        }
        return groups;
    }

    private void dfs(String node, Map<String, Set<String>> neighbors,
                     Set<String> visited, List<String> group) {
        visited.add(node);
        group.add(node);
        for (String nei : neighbors.getOrDefault(node, Set.of())) {
            if (!visited.contains(nei)) dfs(nei, neighbors, visited, group);
        }
    }
}