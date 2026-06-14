package erdg.viz;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;

import erdg.erdg.ERDG;
import erdg.model.ActivationNode;
import erdg.model.MessageServerNode;
import erdg.model.RebecNode;
import erdg.pipeline.HRG;
import erdg.pipeline.RebecDependencyGraph;

/**
 * Renders ERDG, RIG, and HRG graphs to PNG files by writing DOT files
 * and calling the 'dot' command-line tool directly.
 *
 * This avoids the slf4j version conflict in graphviz-java.
 *
 * Java port of draw_erdg, draw_rebec_dependency_graph, draw_hrg
 * from ERDGTestGenerator in Python.
 */
public class Visualizer {

    private final String outputDir;

    public Visualizer(String outputDir) {
        this.outputDir = outputDir;
        new File(outputDir).mkdirs();
    }

    // =========================================================================
    // ERDG
    // =========================================================================

    public void drawERDG(ERDG erdg, String filename) {
        System.out.println("\n=== Drawing ERDG ===");
        StringBuilder dot = new StringBuilder();
        dot.append("digraph ERDG {\n");
        dot.append("  rankdir=TB;\n");

        // Rebec nodes — light blue boxes
        for (RebecNode r : erdg.rebecs) {
            dot.append(String.format(
                "  \"%s\" [shape=box, style=filled, fillcolor=lightblue, label=\"%s\\n(%s)\"];\n",
                r.name(), r.name(), r.reactiveClass()));
        }

        // Message server nodes — light green ellipses
        for (MessageServerNode m : erdg.messageServers) {
            dot.append(String.format(
                "  \"%s\" [shape=ellipse, style=filled, fillcolor=\"#90EE90\"];\n",
                m.toString()));
            // E_RM edge
            dot.append(String.format(
                "  \"%s\" -> \"%s\";\n", m.rebecName(), m.toString()));
        }

        // Activation nodes — yellow diamonds
        for (ActivationNode a : erdg.activations) {
            String act = escape(a.toString());
            dot.append(String.format(
                "  \"%s\" [shape=diamond, style=filled, fillcolor=yellow];\n", act));
            String senderMs = a.senderRebec() + "." + a.senderMethod();
            dot.append(String.format("  \"%s\" -> \"%s\";\n", senderMs, act));
            dot.append(String.format("  \"%s\" -> \"%s\";\n", act, a.targetRebec()));
            dot.append(String.format("  \"%s\" -> \"%s\";\n",
                act, a.targetRebec() + "." + a.messageName()));
        }

        // Variable nodes — orange, write/read edges
        for (var e : erdg.E_write) {
            dot.append(String.format(
                "  \"%s\" [shape=box, style=filled, fillcolor=\"#FFA07A\"];\n", e.target()));
            dot.append(String.format(
                "  \"%s\" -> \"%s\" [color=green, label=\"write\"];\n",
                e.source(), e.target()));
        }
        for (var e : erdg.E_read) {
            dot.append(String.format(
                "  \"%s\" -> \"%s\" [color=blue, label=\"read\"];\n",
                e.source(), e.target()));
        }

        dot.append("}\n");
        renderDot(dot.toString(), filename);
    }

    // =========================================================================
    // RIG
    // =========================================================================

    public void drawRIG(RebecDependencyGraph rig, String filename) {
        System.out.println("=== Drawing RIG ===");
        StringBuilder dot = new StringBuilder();
        dot.append("graph RIG {\n");
        dot.append("  rankdir=LR;\n");

        for (String node : rig.nodes) {
            dot.append(String.format(
                "  \"%s\" [shape=ellipse, style=filled, fillcolor=lightblue];\n", node));
        }
        for (var e : rig.edges) {
            dot.append(String.format("  \"%s\" -- \"%s\";\n", e.a(), e.b()));
        }

        dot.append("}\n");
        renderDot(dot.toString(), filename);
    }

    // =========================================================================
    // HRG
    // =========================================================================

    public void drawHRG(HRG hrg, String filename) {
        System.out.println("=== Drawing HRG ===");
        StringBuilder dot = new StringBuilder();
        dot.append("digraph HRG {\n");
        dot.append("  rankdir=TB;\n");

        for (int i = 0; i < hrg.groups.size(); i++) {
            String label = "Group " + (i + 1) + "\\n"
                    + String.join(", ", hrg.groups.get(i));
            dot.append(String.format(
                "  \"G%d\" [shape=box, style=filled, fillcolor=\"#90EE90\", label=\"%s\"];\n",
                i, label));
        }
        for (var e : hrg.edges) {
            dot.append(String.format("  \"G%d\" -> \"G%d\";\n", e.from(), e.to()));
        }

        dot.append("}\n");
        renderDot(dot.toString(), filename);
    }

    // =========================================================================
    // Helper
    // =========================================================================

    private String escape(String s) {
        return s.replace("\"", "\\\"");
    }

    private void renderDot(String dotContent, String filename) {
        String dotFile = outputDir + "/" + filename + ".dot";
        String pngFile = outputDir + "/" + filename + ".png";

        // Write DOT file
        try (PrintWriter w = new PrintWriter(new FileWriter(dotFile))) {
            w.print(dotContent);
        } catch (IOException e) {
            System.err.println("Failed to write DOT file: " + e.getMessage());
            return;
        }

        // Run dot command
        try {
            ProcessBuilder pb = new ProcessBuilder("dot", "-Tpng", dotFile, "-o", pngFile);
            pb.redirectErrorStream(true);
            Process p = pb.start();
            int exit = p.waitFor();
            if (exit == 0) {
                System.out.println("Saved: " + new File(pngFile).getAbsolutePath());
            } else {
                String err = new String(p.getInputStream().readAllBytes());
                System.err.println("dot command failed: " + err);
            }
        } catch (IOException | InterruptedException e) {
            System.err.println("Failed to run dot: " + e.getMessage());
        }
    }
}