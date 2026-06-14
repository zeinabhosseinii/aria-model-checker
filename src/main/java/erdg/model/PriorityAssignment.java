package erdg.model;
import java.util.Map;
/**
 * Represents the output of the priority assignment algorithm:
 * a mapping from each rebec instance to its global priority,
 * and from each message server to its intra-rebec priority.
 * This is directly translated into @priority annotations in Timed Rebeca.
 */
public record PriorityAssignment(
int id,
Map<String, Integer> rebecPriorities,
Map<String, Map<String, Integer>> methodPriorities
) {
    @Override
public String toString() {
return "PriorityAssignment";
    }
}