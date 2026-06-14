package erdg.pipeline;

import java.util.ArrayList;
import java.util.List;

import erdg.erdg.ERDG;
import erdg.erdg.SDIAnalyzer;
import erdg.model.RebecNode;

/**
 * Step 1 of the priority assignment pipeline.
 *
 * Builds the undirected Rebec Dependency Graph (RIG) where an edge
 * {r1, r2} exists iff r1 and r2 exhibit SDI (Shared Destination Interference).
 *
 * This graph captures which pairs of rebecs must have their interleavings
 * explored non-deterministically.
 *
 * Java port of the Python ERDGTestGenerator.step1_build_rebec_dependency_graph().
 */
public class Step1RebecDependencyGraph {

    private final ERDG erdg;
    private final SDIAnalyzer sdi;

    public Step1RebecDependencyGraph(ERDG erdg, SDIAnalyzer sdi) {
        this.erdg = erdg;
        this.sdi = sdi;
    }

    public RebecDependencyGraph build() {
        System.out.println("\n=== Step 1: Building Rebec Dependency Graph ===");

        List<String> rebecNames = new ArrayList<>();
        for (RebecNode r : erdg.rebecs) rebecNames.add(r.name());

        List<RebecDependencyGraph.Edge> edges = new ArrayList<>();
        for (int i = 0; i < rebecNames.size(); i++) {
            for (int j = i + 1; j < rebecNames.size(); j++) {
                String r1 = rebecNames.get(i);
                String r2 = rebecNames.get(j);
                if (sdi.areRebecDependent(r1, r2)) {
                    edges.add(new RebecDependencyGraph.Edge(r1, r2));
                    System.out.println("  Dependency edge: " + r1 + " <-> " + r2);
                }
            }
        }

        System.out.println("RIG: " + edges.size() + " dependency edges");
        return new RebecDependencyGraph(rebecNames, edges);
    }
}