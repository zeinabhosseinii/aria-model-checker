package erdg.pipeline;

import java.util.List;

/**
 * The Hierarchical Rebec Group Graph (HRG) — a directed graph whose nodes are
 * interference groups (connected components of the RIG) and whose edges encode
 * coarse-grained causal precedence between groups.
 *
 * Java port of the dict-style HRG used in Python's ERDGTestGenerator:
 *   self.HRG = {"groups": [...], "edges": [...]}
 *
 * The HRG is a DAG (after transitive reduction and cycle-breaking).
 */
public class HRG {

    /** Directed edge between two group indices: from -> to. */
    public record Edge(int from, int to) {
        @Override
        public String toString() {
            return "Group " + (from + 1) + " -> Group " + (to + 1);
        }
    }

    /** The interference groups, each as a list of rebec instance names. */
    public final List<List<String>> groups;

    /** Directed precedence edges between group indices. */
    public final List<Edge> edges;

    public HRG(List<List<String>> groups, List<Edge> edges) {
        this.groups = groups;
        this.edges = edges;
    }
}