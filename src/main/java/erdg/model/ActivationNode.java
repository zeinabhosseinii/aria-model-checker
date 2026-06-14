package erdg.model;

/**
 * Represents a single send statement within a message server (N_A).
 * Each activation encodes: who sends (senderRebec.senderMethod),
 * to whom (targetRebec), and which message server is invoked (messageName).
 * This is the key structural element for extracting causal dependencies.
 *
 * Java port of the Python @dataclass ActivationNode.
 */
public record ActivationNode(
        String senderRebec,
        String senderMethod,
        String targetRebec,
        String messageName,
        Integer delayTime   // nullable, used for Timed Rebeca
) {
    /**
     * Canonical string form:
     *   "senderRebec.senderMethod -> targetRebec.messageName[@delay]"
     * Matches the Python __str__ implementation.
     */
    @Override
    public String toString() {
        String delayStr = (delayTime != null) ? "@" + delayTime : "";
        return senderRebec + "." + senderMethod
                + " -> " + targetRebec + "." + messageName + delayStr;
    }
}