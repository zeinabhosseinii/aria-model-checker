package erdg.ast;

import java.util.List;
import java.util.Map;
import java.util.Set;

public record AnalysisResult(
        Map<String, ReactiveClass> classes,
        List<RebecInstance> mainInstances
) {
    public record ReactiveClass(
            String name,
            Set<String> stateVars,
            Map<String, MessageServer> messageServers
    ) {}

    public record MessageServer(
            String name,
            Integer priority,
            List<Send> sends,
            Set<String> reads,
            Set<String> writes
    ) {}

    public record Send(String target, String message) {}

    public record RebecInstance(
            String name,
            String reactiveClass,
            Map<String, String> knownRebecBindings,
            Integer priority
    ) {}
}
