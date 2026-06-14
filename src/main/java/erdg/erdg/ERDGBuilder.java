package erdg.erdg;

import java.util.HashMap;
import java.util.Map;

import erdg.ast.AnalysisResult;
import erdg.ast.AnalysisResult.MessageServer;
import erdg.ast.AnalysisResult.RebecInstance;
import erdg.ast.AnalysisResult.Send;
import erdg.model.ActivationNode;
import erdg.model.MessageServerNode;
import erdg.model.RebecNode;

/**
 * Builds the Essential Rebeca Dependency Graph (ERDG) from an AnalysisResult.
 *
 * Java port of the ERDG construction phase of ERDGTestGenerator in Python:
 *   - build_erdg()
 *   - _create_rebec_nodes()
 *   - _create_message_server_nodes()
 *   - _create_activation_nodes()
 *   - _resolve_target()
 *   - _create_variable_dependency_edges()
 *
 * The resulting ERDG object is consumed by the SVI/SDI analyzers
 * and the priority-assignment pipeline.
 */
public class ERDGBuilder {

    private final AnalysisResult analysis;
    private final Map<String, RebecInstance> instanceMap = new HashMap<>();

    public ERDGBuilder(AnalysisResult analysis) {
        this.analysis = analysis;
        for (RebecInstance inst : analysis.mainInstances()) {
            instanceMap.put(inst.name(), inst);
        }
    }

    /**
     * Construct all node sets and edge sets of the ERDG from the analysis result.
     */
    public ERDG build() {
        System.out.println("\n=== Building ERDG ===");
        ERDG erdg = new ERDG();

        createRebecNodes(erdg);
        createMessageServerNodes(erdg);
        createActivationNodes(erdg);
        createVariableDependencyEdges(erdg);

        System.out.printf("ERDG built: %d rebecs, %d message servers, "
                        + "%d activations, %d writes, %d reads%n",
                erdg.rebecs.size(),
                erdg.messageServers.size(),
                erdg.activations.size(),
                erdg.E_write.size(),
                erdg.E_read.size());

        return erdg;
    }

    // =========================================================================
    // Node creation
    // =========================================================================

    /** Create one rebec node for each rebec instance in the main block. */
    private void createRebecNodes(ERDG erdg) {
        for (RebecInstance inst : analysis.mainInstances()) {
            erdg.rebecs.add(new RebecNode(
                    inst.name(),
                    inst.reactiveClass(),
                    inst.reactiveClass() != null ? inst.reactiveClass() : "",
                    inst.priority()
            ));
        }
    }

    /**
     * Create one message server node for each method of each rebec instance,
     * and add the corresponding E_RM edge (rebec → message server).
     */
    private void createMessageServerNodes(ERDG erdg) {
        for (RebecInstance inst : analysis.mainInstances()) {
            String instanceName = inst.name();
            String className = inst.reactiveClass();
            var cls = analysis.classes().get(className);
            if (cls == null) continue;

            for (var entry : cls.messageServers().entrySet()) {
                String methodName = entry.getKey();
                MessageServer ms = entry.getValue();

                MessageServerNode node = new MessageServerNode(
                        instanceName, methodName, ms.priority());
                erdg.messageServers.add(node);
                erdg.E_RM.add(new ERDG.Edge(instanceName, node.toString()));
            }
        }
    }

    /**
     * For each send statement in each message server, create one activation node
     * and add the three structural edges:
     *   E_MA: sender message server → activation
     *   E_AR: activation → destination rebec
     *   E_AM: activation → target message server at destination
     */
    private void createActivationNodes(ERDG erdg) {
        for (RebecInstance inst : analysis.mainInstances()) {
            String instanceName = inst.name();
            String className = inst.reactiveClass();
            var cls = analysis.classes().get(className);
            if (cls == null) continue;

            for (var entry : cls.messageServers().entrySet()) {
                String methodName = entry.getKey();
                MessageServer ms = entry.getValue();

                for (Send send : ms.sends()) {
                    String targetRebec = resolveTarget(send.target(), instanceName);
                    if (targetRebec == null) {
                        System.out.println("Warning: Unknown target '" + send.target()
                                + "' in " + instanceName + "." + methodName);
                        continue;
                    }
                    String targetClass = null;
                    RebecInstance targetInst = instanceMap.get(targetRebec);
                    if (targetInst != null) targetClass = targetInst.reactiveClass();

                    if (targetClass != null && analysis.classes().containsKey(targetClass)) {
                        if (!analysis.classes().get(targetClass)
                                .messageServers().containsKey(send.message())) {
                            System.out.println("Warning: Method '" + send.message()
                                    + "' not in '" + targetRebec + "'");
                            continue;
                        }
                    }

                    ActivationNode activation = new ActivationNode(
                            instanceName, methodName,
                            targetRebec, send.message(), null);
                    erdg.activations.add(activation);

                    erdg.E_MA.add(new ERDG.Edge(
                            instanceName + "." + methodName, activation.toString()));
                    erdg.E_AR.add(new ERDG.Edge(activation.toString(), targetRebec));
                    erdg.E_AM.add(new ERDG.Edge(
                            activation.toString(), targetRebec + "." + send.message()));
                }
            }
        }
    }

    /**
     * Resolve a symbolic send target (e.g. 'self', a known rebec name, or
     * a class name used as shorthand) to a concrete instance name.
     *
     * For the real Rebeca grammar we will instead use the per-instance
     * knownRebecBindings map. This implementation matches the Python fallback.
     */
    private String resolveTarget(String target, String senderInstance) {
        if (target == null) return null;
        if (target.equals("self")) return senderInstance;

        // NEW: resolve through knownRebecBindings of the sender instance
        var senderInst = instanceMap.get(senderInstance);
        if (senderInst != null) {
            String resolved = senderInst.knownRebecBindings().get(target);
            if (resolved != null) return resolved;
        }

        // Fallback: direct instance name match
        if (instanceMap.containsKey(target)) return target;

        // Fallback: case-insensitive match
        for (String name : instanceMap.keySet()) {
            if (name.equalsIgnoreCase(target)) return name;
        }
        // Fallback: match by class name
        for (var inst : analysis.mainInstances()) {
            if (inst.reactiveClass().equalsIgnoreCase(target)) return inst.name();
        }
        return null;
    }

    /**
     * Populate E_write and E_read edges for each instance and method:
     *   E_write: (instance.method, instance.variable)  — method writes variable
     *   E_read:  (instance.variable, instance.method)  — method reads variable
     *
     * Variables are prefixed with the instance name to ensure uniqueness
     * across different instances of the same class. These edges are the
     * basis for Shared Variable Interference (SVI) detection.
     */
    private void createVariableDependencyEdges(ERDG erdg) {
        for (RebecInstance inst : analysis.mainInstances()) {
            String instanceName = inst.name();
            String className = inst.reactiveClass();
            var cls = analysis.classes().get(className);
            if (cls == null) continue;

            for (var entry : cls.messageServers().entrySet()) {
                String methodName = entry.getKey();
                MessageServer ms = entry.getValue();
                String msLabel = instanceName + "." + methodName;

                for (String var : ms.writes()) {
                    String varLabel = instanceName + "." + var;
                    erdg.E_write.add(new ERDG.Edge(msLabel, varLabel));
                }
                for (String var : ms.reads()) {
                    String varLabel = instanceName + "." + var;
                    erdg.E_read.add(new ERDG.Edge(varLabel, msLabel));
                }
            }
        }
    }
}