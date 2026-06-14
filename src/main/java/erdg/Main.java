package erdg;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.antlr.v4.runtime.CharStreams;
import org.antlr.v4.runtime.CommonTokenStream;
import org.antlr.v4.runtime.tree.ParseTree;

import erdg.ast.AnalysisResult;
import erdg.ast.RebecaASTAnalyzer;
import erdg.erdg.CausalAnalyzer;
import erdg.erdg.ERDG;
import erdg.erdg.ERDGBuilder;
import erdg.erdg.SDIAnalyzer;
import erdg.erdg.SVIAnalyzer;
import erdg.grammar.RebecaLexer;
import erdg.grammar.RebecaParser;
import erdg.model.PriorityAssignment;
import erdg.pipeline.RebecDependencyGraph;
import erdg.pipeline.HRG;
import erdg.pipeline.Step1RebecDependencyGraph;
import erdg.pipeline.Step2HierarchicalRebecGraph;
import erdg.pipeline.Step3RebecPriorities;
import erdg.pipeline.Step4MessageServerPriorities;
import erdg.pipeline.Step5PriorityAssignmentGenerator;
import erdg.viz.Visualizer;

public class Main {

    public static void main(String[] args) throws IOException {
        if (args.length == 0) {
            printUsage();
            return;
        }

        String command = args[0];

        switch (command) {
            case "list" -> listBenchmarks();
            case "run" -> {
                if (args.length < 2) {
                    System.out.println("Usage: run <benchmark-name>");
                    System.out.println("Use \'list\' to see available benchmarks.");
                    return;
                }
                runBenchmark(args[1]);
            }
            case "run-all" -> runAllBenchmarks();
            case "file" -> {
                if (args.length < 2) {
                    System.out.println("Usage: file <path-to-rebeca-file>");
                    return;
                }
                runFile(args[1]);
            }
            default -> {
                System.out.println("Unknown command: " + command);
                printUsage();
            }
        }
    }

    // =========================================================================
    // Commands
    // =========================================================================

    private static void printUsage() {
        System.out.println("""
            ERDG Tool — Usage:
              list              List all available benchmarks
              run <name>        Run a specific benchmark by name
              run-all           Run all benchmarks
              file <path>       Run a .rebeca file directly
            """);
    }

    private static void listBenchmarks() throws IOException {
        System.out.println("=== Available Benchmarks ===");
        var benchmarks = loadBenchmarks();
        for (int i = 0; i < benchmarks.size(); i++) {
            String[] b = benchmarks.get(i);
            System.out.printf("  [%d] %-20s %s%n", i + 1, b[0], b[1]);
        }
        System.out.println("\nRun with: ./run.sh run <name>");
    }

    private static void runBenchmark(String name) throws IOException {
        var benchmarks = loadBenchmarks();
        for (String[] b : benchmarks) {
            if (b[0].equalsIgnoreCase(name)) {
                System.out.println("=== Running benchmark: " + b[0] + " ===");
                System.out.println("Description: " + b[1]);
                System.out.println();
                runModel(b[2], b[0]);
                return;
            }
        }
        System.out.println("Benchmark not found: " + name);
        System.out.println("Use \'list\' to see available benchmarks.");
    }

    private static void runAllBenchmarks() throws IOException {
        var benchmarks = loadBenchmarks();
        System.out.println("=== Running all " + benchmarks.size() + " benchmarks ===\n");
        for (String[] b : benchmarks) {
            System.out.println("\n" + "=".repeat(50));
            System.out.println("Benchmark: " + b[0]);
            System.out.println("Description: " + b[1]);
            System.out.println("=".repeat(50));
            runModel(b[2], b[0]);
        }
        System.out.println("\n=== All benchmarks complete ===");
    }

    private static void runFile(String filePath) throws IOException {
        String code = Files.readString(Paths.get(filePath));
        String name = Paths.get(filePath).getFileName().toString().replace(".rebeca", "");
        System.out.println("=== Running file: " + filePath + " ===\n");
        runModel(code, name);
    }

    // =========================================================================
    // Core pipeline
    // =========================================================================

    private static void runModel(String code, String modelName) {
        // Parse
        RebecaLexer lexer = new RebecaLexer(CharStreams.fromString(code));
        CommonTokenStream tokens = new CommonTokenStream(lexer);
        RebecaParser parser = new RebecaParser(tokens);
        ParseTree tree = parser.model();

        if (parser.getNumberOfSyntaxErrors() > 0) {
            System.out.println("Parse FAILED — syntax errors found.");
            return;
        }
        System.out.println("✅ Parse successful.");

        // Analyze
        RebecaASTAnalyzer analyzer = new RebecaASTAnalyzer();
        AnalysisResult result = analyzer.analyze(tree);

        System.out.println("\n--- Classes ---");
        for (var cls : result.classes().entrySet()) {
            System.out.println("  " + cls.getKey()
                + " | statevars: " + cls.getValue().stateVars()
                + " | msgsrvs: " + cls.getValue().messageServers().keySet());
        }
        System.out.println("--- Instances ---");
        for (var inst : result.mainInstances()) {
            System.out.println("  " + inst.name() + ":" + inst.reactiveClass()
                + " bindings=" + inst.knownRebecBindings());
        }

        // Pipeline
        ERDGBuilder builder = new ERDGBuilder(result);
        ERDG erdg = builder.build();

        SVIAnalyzer svi = new SVIAnalyzer(erdg);
        CausalAnalyzer causal = new CausalAnalyzer(erdg);
        SDIAnalyzer sdi = new SDIAnalyzer(erdg, svi, causal);

        Step1RebecDependencyGraph step1 = new Step1RebecDependencyGraph(erdg, sdi);
        RebecDependencyGraph rig = step1.build();

        Step2HierarchicalRebecGraph step2 = new Step2HierarchicalRebecGraph(erdg, result, rig);
        HRG hrg = step2.build();

        Step3RebecPriorities step3 = new Step3RebecPriorities(erdg, result, hrg, step2);
        Map<String, Integer> rebecPriorities = step3.assign();

        Step4MessageServerPriorities step4 = new Step4MessageServerPriorities(erdg, result, svi);
        Map<String, Map<String, Integer>> msPriorities = step4.assign();

        Step5PriorityAssignmentGenerator step5 = new Step5PriorityAssignmentGenerator(rebecPriorities, msPriorities);
        PriorityAssignment pa = step5.generate();
        System.out.println("\n✅ Final Priority Assignment: " + pa);
        System.out.println("   Rebec priorities:  " + pa.rebecPriorities());
        System.out.println("   Method priorities: " + pa.methodPriorities());


        // Generate annotated model
        String annotatedDir = "output/" + modelName;
        new java.io.File(annotatedDir).mkdirs();
        String annotatedPath = annotatedDir + "/" + modelName + "_annotated.rebeca";
        try {
            new erdg.generator.RebecaModelGenerator(code, pa, result)
                .saveToFile(annotatedPath);
        } catch (java.io.IOException e) {
            System.err.println("Failed to save annotated model: " + e.getMessage());
        }
        
        // Visualize
        String outDir = "output/" + modelName;
        Visualizer viz = new Visualizer(outDir);
        viz.drawERDG(erdg, "ERDG");
        viz.drawRIG(rig, "RIG");
        viz.drawHRG(hrg, "HRG");
        System.out.println("\n📊 Graphs saved to: " + outDir + "/");
    }

    // =========================================================================
    // Benchmark loader — simple JSON parser (no external library)
    // =========================================================================

    /**
     * Returns list of [name, description, model] arrays from benchmarks.json
     */
    private static List<String[]> loadBenchmarks() throws IOException {
        String jsonFile = "benchmarks/benchmarks.json";
        if (!Files.exists(Paths.get(jsonFile))) {
            System.out.println("benchmarks.json not found at: " + jsonFile);
            return List.of();
        }

        String json = Files.readString(Paths.get(jsonFile));
        List<String[]> result = new ArrayList<>();

        // Simple extraction: find each benchmark object
        int pos = 0;
        while (true) {
            int nameIdx = json.indexOf("\"name\"", pos);
            if (nameIdx < 0) break;

            String name = extractJsonString(json, nameIdx + 7);
            int descIdx = json.indexOf("\"description\"", nameIdx);
            String desc = extractJsonString(json, descIdx + 14);
            int modelIdx = json.indexOf("\"model\"", descIdx);
            String model = extractJsonString(json, modelIdx + 8);

            // Unescape newlines
            model = model.replace("\\n", "\n");

            result.add(new String[]{name, desc, model});
            pos = modelIdx + 8;
        }
        return result;
    }

    private static String extractJsonString(String json, int afterColon) {
        int start = json.indexOf('"', afterColon) + 1;
        StringBuilder sb = new StringBuilder();
        int i = start;
        while (i < json.length()) {
            char c = json.charAt(i);
            if (c == '\\') {
                char next = json.charAt(i + 1);
                if (next == '"') { sb.append('"'); i += 2; }
                else if (next == 'n') { sb.append('\n'); i += 2; }
                else if (next == '\\') { sb.append('\\'); i += 2; }
                else { sb.append(c); i++; }
            } else if (c == '"') {
                break;
            } else {
                sb.append(c);
                i++;
            }
        }
        return sb.toString();
    }
}
