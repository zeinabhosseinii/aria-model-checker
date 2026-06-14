package erdg.model;

/**
 * Represents a concrete rebec instance declared in the main block (N_R).
 * Each instance has a unique name, belongs to a reactive class, and may
 * carry a statically assigned priority used by the scheduler.
 *
 * Java port of the Python @dataclass RebecNode.
 */
public record RebecNode(
        String name,
        String reactiveClass,
        String arg,
        Integer priority   // nullable
) {
    @Override
    public String toString() {
        return name;
    }
}