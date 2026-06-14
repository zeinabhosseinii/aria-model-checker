package erdg.erdg;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import erdg.model.ActivationNode;

/**
 * Detects Shared Destination Interference (SDI) between pairs of rebec instances.
 *
 * Java port of the following Python methods from ERDGTestGenerator:
 *   - _are_possibly_concurrent(r1, r2, target)
 *   - are_rebec_dependent(r1, r2)
 *
 * Two rebecs r1 and r2 exhibit SDI with respect to a common target r3 iff:
 *   (1) Both r1 and r2 send messages to r3 (shared destination), AND
 *   (2) The sending message servers are causally Independent  — their
 *       messages can genuinely arrive concurrently at r3, AND
 *   (3) The target message servers invoked at r3 exhibit SVI — their
 *       execution order affects r3's local state.
 */
public class SDIAnalyzer {

    private final ERDG erdg;
    private final SVIAnalyzer svi;
    private final CausalAnalyzer causal;

    public SDIAnalyzer(ERDG erdg, SVIAnalyzer svi, CausalAnalyzer causal) {
        this.erdg = erdg;
        this.svi = svi;
        this.causal = causal;
    }

    /**
     * Return true iff the messages sent by r1 and r2 to 'target' are
     * causally Independent — i.e., they can genuinely arrive concurrently
     * in target's mailbox.
     *
     * Two sending message servers ms1 (in r1) and ms2 (in r2) are
     * causally Independent iff neither causally triggers the other.
     *
     * If a causal ordering exists (e.g., ms1 ⇒ ms2), then ms2 can only
     * appear in target's queue after ms1 has been executed, so they
     * cannot be simultaneously present — no genuine non-determinism arises.
     */
    public boolean arePossiblyConcurrent(String r1, String r2, String target) {
        List<String> sendingMsR1 = new ArrayList<>();
        List<String> sendingMsR2 = new ArrayList<>();

        for (ActivationNode a : erdg.activations) {
            if (a.senderRebec().equals(r1) && a.targetRebec().equals(target)) {
                sendingMsR1.add(a.senderRebec() + "." + a.senderMethod());
            }
            if (a.senderRebec().equals(r2) && a.targetRebec().equals(target)) {
                sendingMsR2.add(a.senderRebec() + "." + a.senderMethod());
            }
        }

        if (sendingMsR1.isEmpty() || sendingMsR2.isEmpty()) return false;

        for (String ms1 : sendingMsR1) {
            for (String ms2 : sendingMsR2) {
                if (causal.causallyTriggers(ms1, ms2)) {
                    System.out.println("  Causal: " + ms1 + " ⇒ " + ms2 + " → NOT Independent");
                    return false;
                }
                if (causal.causallyTriggers(ms2, ms1)) {
                    System.out.println("  Causal: " + ms2 + " ⇒ " + ms1 + " → NOT Independent");
                    return false;
                }
            }
        }
        return true;
    }

    /**
     * Return true iff r1 and r2 are SDI-dependent.
     *
     * Three conditions must hold for some common target r3:
     *   (1) Both r1 and r2 send to r3.
     *   (2) Causal Independence: arePossiblyConcurrent(r1, r2, r3).
     *   (3) SVI: some pair of message servers invoked at r3 by r1 and r2
     *       exhibit Shared Variable Interference.
     */
    public boolean areRebecDependent(String r1, String r2) {
        // Collect (target, message) pairs sent by each rebec
        List<String[]> r1Sends = new ArrayList<>();
        List<String[]> r2Sends = new ArrayList<>();
        Set<String> r1Targets = new HashSet<>();
        Set<String> r2Targets = new HashSet<>();

        for (ActivationNode a : erdg.activations) {
            if (a.senderRebec().equals(r1)) {
                r1Sends.add(new String[]{a.targetRebec(), a.messageName()});
                r1Targets.add(a.targetRebec());
            }
            if (a.senderRebec().equals(r2)) {
                r2Sends.add(new String[]{a.targetRebec(), a.messageName()});
                r2Targets.add(a.targetRebec());
            }
        }

        // Intersection of targets — the shared destinations
        Set<String> commonTargets = new HashSet<>(r1Targets);
        commonTargets.retainAll(r2Targets);

        for (String target : commonTargets) {
            // Condition 2: causal Independent
            if (!arePossiblyConcurrent(r1, r2, target)) continue;

            // Condition 3: SVI between target message servers
            List<String> msgsFromR1 = new ArrayList<>();
            List<String> msgsFromR2 = new ArrayList<>();
            for (String[] s : r1Sends) if (s[0].equals(target)) msgsFromR1.add(s[1]);
            for (String[] s : r2Sends) if (s[0].equals(target)) msgsFromR2.add(s[1]);

            for (String m1 : msgsFromR1) {
                for (String m2 : msgsFromR2) {
                    String ms1Label = target + "." + m1;
                    String ms2Label = target + "." + m2;
                    if (svi.haveSvi(ms1Label, ms2Label)) {
                        System.out.println("SDI: " + r1 + " <-> " + r2
                                + " via " + target
                                + " (SVI: " + ms1Label + " ↔ " + ms2Label + ")");
                        return true;
                    }
                }
            }
        }
        return false;
    }
}