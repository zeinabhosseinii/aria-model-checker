package erdg.erdg;

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Deque;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import erdg.model.ActivationNode;

/**
 * Detects causal relationships between message servers in the ERDG.
 *
 * Java port of the following Python methods from ERDGTestGenerator:
 *   - _is_reachable_ms(start, target)
 *   - _causally_triggers_ms(ms1, ms2)
 *
 * These analyses are key for distinguishing genuine concurrent message
 * arrivals from causally-ordered ones, which is the basis for the
 * causal-disjointness condition used in SDI detection.
 */
public class CausalAnalyzer {

    private final ERDG erdg;

    public CausalAnalyzer(ERDG erdg) {
        this.erdg = erdg;
    }

    // -------------------------------------------------------------------------
    // Public API
    // -------------------------------------------------------------------------

    /**
     * Return true if 'target' is reachable from 'start' by following
     * E_MA and E_AM edges in the ERDG:
     *   message server --[E_MA]--> activation --[E_AM]--> message server --> ...
     *
     * This mirrors the structural causal chain.
     */
    public boolean isReachable(String start, String target) {
        return isReachableAvoidingNode(start, target, null);
    }

    /**
     * Return true iff ms1 causally triggers ms2  (ms1 ⇒ ms2).
     *
     * Definition:
     *   (1) There is a directed path from ms1 to ms2 through E_MA/E_AM, AND
     *   (2) ms1 is a mandatory ancestor of ms2: every activation node that
     *       invokes ms2 must be reachable from ms1, AND
     *   (3) ms2 is NOT reachable from any entry-point without passing through
     *       ms1 — i.e. ms1 is the sole gateway to ms2.
     *
     * Condition (3) is the fix for the case where an independent entry-point
     * (e.g. o.O1) can reach ms2 via a path that bypasses ms1 entirely,
     * which means ms1 is not truly a mandatory ancestor.
     */
    public boolean causallyTriggers(String ms1, String ms2) {
        // Condition 1: a causal path must exist
        if (!isReachable(ms1, ms2)) return false;

        // Condition 2: ms1 must be reachable to every activation that invokes ms2
        List<String> activationsInvokingMs2 = new ArrayList<>();
        for (ActivationNode a : erdg.activations) {
            String targetMs = a.targetRebec() + "." + a.messageName();
            if (targetMs.equals(ms2)) {
                activationsInvokingMs2.add(a.toString());
            }
        }

        if (activationsInvokingMs2.isEmpty()) return false;

        for (String actLabel : activationsInvokingMs2) {
            if (!isReachable(ms1, actLabel)) return false;
        }

        // Condition 3: ms2 must NOT be reachable from any entry-point
        // without passing through ms1.
        // If it is, ms1 is not a mandatory ancestor — ms2 can be triggered
        // independently, so ms1 does not causally force ms2.
        if (isReachableWithoutBlocker(ms1, ms2)) return false;

        return true;
    }

    // -------------------------------------------------------------------------
    // Private helpers
    // -------------------------------------------------------------------------

    /**
     * DFS from 'start' to 'target' over E_MA / E_AM edges,
     * never stepping on 'avoid' (pass null to disable blocking).
     */
    private boolean isReachableAvoidingNode(String start, String target, String avoid) {
        Set<String> visited = new HashSet<>();
        Deque<String> stack = new ArrayDeque<>();

        if (start.equals(avoid)) return false;
        stack.push(start);

        while (!stack.isEmpty()) {
            String current = stack.pop();
            if (current.equals(target)) return true;
            if (current.equals(avoid))  continue;   // blocked node — do not expand
            if (!visited.add(current))  continue;   // already expanded

            // Follow E_MA: message server -> its activations
            for (ActivationNode a : erdg.activations) {
                String senderMs = a.senderRebec() + "." + a.senderMethod();
                if (senderMs.equals(current)) {
                    String actLabel = a.toString();
                    if (!visited.contains(actLabel) && !actLabel.equals(avoid))
                        stack.push(actLabel);
                }
            }

            // Follow E_AM: activation -> its target message server
            for (ActivationNode a : erdg.activations) {
                String actLabel = a.toString();
                if (actLabel.equals(current)) {
                    String targetMs = a.targetRebec() + "." + a.messageName();
                    if (!visited.contains(targetMs) && !targetMs.equals(avoid))
                        stack.push(targetMs);
                }
            }
        }
        return false;
    }

    /**
     * Collect all message-server labels that appear as the *sender* side
     * of at least one E_MA edge (i.e. every ms that owns an activation).
     */
    private Set<String> allSenderMs() {
        Set<String> senders = new HashSet<>();
        for (ActivationNode a : erdg.activations) {
            senders.add(a.senderRebec() + "." + a.senderMethod());
        }
        return senders;
    }

    /**
     * Collect all message-server labels that are the *target* of at least
     * one E_AM edge (i.e. every ms that is invoked by some activation).
     */
    private Set<String> allInvokedMs() {
        Set<String> invoked = new HashSet<>();
        for (ActivationNode a : erdg.activations) {
            invoked.add(a.targetRebec() + "." + a.messageName());
        }
        return invoked;
    }

    /**
     * Return the set of entry-point message servers: those that send
     * messages (appear as E_MA sources) but are never themselves invoked
     * by any activation (no incoming E_AM edge).
     *
     * These are the "roots" of the ERDG — execution can begin here
     * without any prior causal trigger.
     */
    private Set<String> entryPoints() {
        Set<String> entries = allSenderMs();
        entries.removeAll(allInvokedMs());
        return entries;
    }

    /**
     * Return true if 'target' is reachable from *any* entry-point
     * of the ERDG without passing through 'blocker'.
     *
     * Used to test whether ms2 has an independent route that bypasses ms1,
     * which would falsify the mandatory-ancestor claim.
     */
    private boolean isReachableWithoutBlocker(String blocker, String target) {
        for (String entry : entryPoints()) {
            if (entry.equals(blocker)) continue;   // this entry is itself blocked
            if (isReachableAvoidingNode(entry, target, blocker)) return true;
        }
        return false;
    }
}