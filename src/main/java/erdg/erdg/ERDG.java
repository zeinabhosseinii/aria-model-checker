package erdg.erdg;

import java.util.ArrayList;
import java.util.List;

import erdg.model.ActivationNode;
import erdg.model.MessageServerNode;
import erdg.model.RebecNode;

/**
 * Container for all node sets and edge sets of the Essential Rebeca
 * Dependency Graph (ERDG).
 *
 * Java port of the per-instance state held by ERDGTestGenerator in Python:
 *   self.N_R, self.N_M, self.N_A
 *   self.E_RM, self.E_MA, self.E_AR, self.E_AM, self.E_write, self.E_read
 *
 * This class is a pure data structure. The algorithms that operate on it
 * (SVI, SDI, HRG, priority assignment) live in separate classes.
 */
public class ERDG {

    // ---------- Node sets ----------
    /** Rebec instances declared in the main block (N_R). */
    public final List<RebecNode> rebecs = new ArrayList<>();

    /** Message servers, one per (rebec, method) pair (N_M). */
    public final List<MessageServerNode> messageServers = new ArrayList<>();

    /** Activation nodes, one per send statement (N_A). */
    public final List<ActivationNode> activations = new ArrayList<>();

    // ---------- Edge sets ----------
    // Each edge is represented as a (source, target) pair of String labels,
    // matching the Python tuples directly.

    /** Rebec → its message servers. */
    public final List<Edge> E_RM = new ArrayList<>();

    /** Message server → activations it creates. */
    public final List<Edge> E_MA = new ArrayList<>();

    /** Activation → destination rebec. */
    public final List<Edge> E_AR = new ArrayList<>();

    /** Activation → target message server at destination. */
    public final List<Edge> E_AM = new ArrayList<>();

    /** Message server → variable it writes to. */
    public final List<Edge> E_write = new ArrayList<>();

    /** Variable → message server that reads it. */
    public final List<Edge> E_read = new ArrayList<>();

    /**
     * A directed edge in any of the edge sets above.
     * Equivalent to Python's Tuple[str, str].
     */
    public record Edge(String source, String target) {
        @Override
        public String toString() {
            return source + " -> " + target;
        }
    }
}