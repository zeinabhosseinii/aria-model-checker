package erdg.pipeline;

import java.util.Map;

import erdg.model.PriorityAssignment;

/**
 * Step 5 of the priority assignment pipeline.
 * Combines rebec priorities (Step 3) and method priorities (Step 4)
 * into a single canonical PriorityAssignment.
 *
 * Java port of ERDGTestGenerator.step5_generate_prioritized_Priority_Assignment().
 */
public class Step5PriorityAssignmentGenerator {

    private final Map<String, Integer> rebecPriorities;
    private final Map<String, Map<String, Integer>> methodPriorities;

    public Step5PriorityAssignmentGenerator(
            Map<String, Integer> rebecPriorities,
            Map<String, Map<String, Integer>> methodPriorities) {
        this.rebecPriorities = rebecPriorities;
        this.methodPriorities = methodPriorities;
    }

    public PriorityAssignment generate() {
        System.out.println("\n=== Step 5: Generating Canonical Priority Assignment ===");
        PriorityAssignment tc = new PriorityAssignment(1, rebecPriorities, methodPriorities);
        System.out.println("✅ Canonical Priority Assignment generated.");
        System.out.println("  Rebec priorities:  " + tc.rebecPriorities());
        System.out.println("  Method priorities:");
        for (var entry : tc.methodPriorities().entrySet()) {
            System.out.println("    " + entry.getKey() + ": " + entry.getValue());
        }
        return tc;
    }
}