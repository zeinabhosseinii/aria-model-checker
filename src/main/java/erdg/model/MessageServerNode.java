package erdg.model;

/**
 * Represents a message server (method) of a specific rebec instance (N_M).
 * Identified by the pair (rebecName, methodName). May carry an intra-rebec
 * priority used to order message servers within the same rebec.
 *
 * Java port of the Python @dataclass MessageServerNode.
 */
public record MessageServerNode(
        String rebecName,
        String methodName,
        Integer priority   // nullable
) {
    /**
     * Canonical string form used as a label everywhere in the ERDG:
     *   "rebecName.methodName"
     * Matches the Python __str__ implementation.
     */
    @Override
    public String toString() {
        return rebecName + "." + methodName;
    }
}