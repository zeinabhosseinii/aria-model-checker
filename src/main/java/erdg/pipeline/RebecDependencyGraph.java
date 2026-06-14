package erdg.pipeline;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

/**
 * The Rebec Interference Graph (RIG) — an undirected graph where the nodes are
 * rebec instances and an edge {r1, r2} exists iff r1 and r2 exhibit SDI.
 *
 * Java port of the dict-style RIG used in Python's ERDGTestGenerator:
 *   self.RIG = {"nodes": [...], "edges": [...]}
 *
 * Provides connected-component computation for use in Step 2 (HRG construction).
 */
public class RebecDependencyGraph {

    /** Undirected edge between two rebec instances. */
    public record Edge(String a, String b) {
        @Override
        public String toString() {
            return a + " <-> " + b;
        }
    }

    public final List<String> nodes;
    public final List<Edge> edges;

    public RebecDependencyGraph(List<String> nodes, List<Edge> edges) {
        this.nodes = nodes;
        this.edges = edges;
    }

    /**
     * Compute the connected components of this undirected graph using DFS.
     * Each component is returned as a list of rebec instance names.
     *
     * Java port of the Python _find_connected_components helper.
     */
    public List<List<String>> connectedComponents() {
        Set<String> visited = new HashSet<>();
        List<List<String>> components = new ArrayList<>();

        for (String node : nodes) {
            if (visited.contains(node)) continue;
            List<String> component = new ArrayList<>();
            dfs(node, visited, component);
            components.add(component);
        }
        return components;
    }

    private void dfs(String node, Set<String> visited, List<String> component) {
        visited.add(node);
        component.add(node);
        for (Edge e : edges) {
            if (e.a.equals(node) && !visited.contains(e.b)) {
                dfs(e.b, visited, component);
            } else if (e.b.equals(node) && !visited.contains(e.a)) {
                dfs(e.a, visited, component);
            }
        }
    }
}