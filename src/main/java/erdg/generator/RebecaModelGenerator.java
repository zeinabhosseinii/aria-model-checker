package erdg.generator;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.Map;

import erdg.ast.AnalysisResult;
import erdg.model.PriorityAssignment;

/**
 * Generates a priority-annotated Rebeca model.
 *
 * Format:
 *   - @priority(n) on the line BEFORE each msgsrv declaration
 *   - @priority(n) on the line BEFORE each instance in main block
 */
public class RebecaModelGenerator {

    private final String originalCode;
    private final PriorityAssignment testCase;
    private final AnalysisResult analysis;

    public RebecaModelGenerator(String originalCode,
                                 PriorityAssignment testCase,
                                 AnalysisResult analysis) {
        this.originalCode = originalCode;
        this.testCase = testCase;
        this.analysis = analysis;
    }

    public String generate() {
        String[] lines = originalCode.split("\n");
        StringBuilder sb = new StringBuilder();
    
        String currentClass = null;
        boolean inMain = false;
        int braceDepth = 0;
        int classStartDepth = 0;
    
        for (String line : lines) {
            String trimmed = line.trim();
            String indent = getIndent(line);
    
            int openCount  = countChar(line, '{');
            int closeCount = countChar(line, '}');
    
            // Detect 'main' block — handles both "main {" and "main{"
            if (trimmed.startsWith("main") && trimmed.replaceAll("\\s", "").startsWith("main{")) {
                inMain = true;
                currentClass = null;
                sb.append(line).append("\n");
                braceDepth += openCount - closeCount;
                continue;
            }
    
            // Detect reactiveclass
            if (trimmed.startsWith("reactiveclass ") || trimmed.startsWith("abstract reactiveclass ")) {
                String[] parts = trimmed.replaceFirst("abstract\\s+", "").split("[\\s(]+");
                if (parts.length >= 2) {
                    currentClass = parts[1];
                    classStartDepth = braceDepth;
                }
                inMain = false;
                sb.append(line).append("\n");
                braceDepth += openCount - closeCount;
                continue;
            }
    
            // Close brace
            if (trimmed.equals("}") || trimmed.startsWith("}")) {
                braceDepth += openCount - closeCount;
                if (currentClass != null && braceDepth <= classStartDepth) {
                    currentClass = null;
                }
                if (inMain && braceDepth <= 0) {
                    inMain = false;
                }
                sb.append(line).append("\n");
                continue;
            }
    
            // Inside a reactiveclass: annotate msgsrv
            if (currentClass != null && (trimmed.startsWith("msgsrv ") || trimmed.startsWith("abstract msgsrv "))) {
                String methodName = extractMsgsrvName(trimmed);
                Integer priority = getMethodPriority(currentClass, methodName);
                if (priority != null) {
                    sb.append(indent).append("@priority(").append(priority).append(")\n");
                }
                sb.append(line).append("\n");
                braceDepth += openCount - closeCount;
                continue;
            }
    
            // Inside main: annotate instance declarations
            if (inMain && isMainInstance(trimmed)) {
                String instanceName = extractInstanceName(trimmed);
                Integer priority = testCase.rebecPriorities().get(instanceName);
                if (priority != null) {
                    sb.append(indent).append("@priority(").append(priority).append(")\n");
                }
                sb.append(line).append("\n");
                braceDepth += openCount - closeCount;
                continue;
            }
    
            // Default
            sb.append(line).append("\n");
            braceDepth += openCount - closeCount;
        }
    
        return sb.toString();
    }

    public void saveToFile(String outputPath) throws IOException {
        String content = generate();
        Files.writeString(Paths.get(outputPath), content);
        System.out.println("✅ Annotated model saved to: " + outputPath);
    }

    // =========================================================================
    // Helpers
    // =========================================================================

    /** Extract method name from "msgsrv foo(...) {" */
    private String extractMsgsrvName(String trimmed) {
        String after = trimmed.substring("msgsrv".length()).trim();
        int paren = after.indexOf('(');
        return (paren >= 0) ? after.substring(0, paren).trim() : after.trim();
    }

    /** Extract instance name from "Type name(...):(...);" */
    private String extractInstanceName(String trimmed) {
        String[] tokens = trimmed.split("\\s+");
        if (tokens.length < 2) return "";
        String second = tokens[1];
        int paren = second.indexOf('(');
        return (paren >= 0) ? second.substring(0, paren) : second;
    }

    /** Check if a line in main is an instance declaration */
    private boolean isMainInstance(String trimmed) {
        if (trimmed.isEmpty()) return false;
        if (!Character.isUpperCase(trimmed.charAt(0))) return false;
        return trimmed.matches("[A-Za-z_$][A-Za-z0-9_$]*\\s+[A-Za-z_$][A-Za-z0-9_$]*\\s*\\(.*\\)\\s*:\\s*\\(.*\\)\\s*;\\s*");
    }

    /** Get priority for a method of a class (tries all instances of that class) */
    private Integer getMethodPriority(String className, String methodName) {
        Map<String, Integer> mp = testCase.methodPriorities().get(className);
        if (mp != null && mp.containsKey(methodName)) return mp.get(methodName);
        return null;
    }

    private String getIndent(String line) {
        int i = 0;
        while (i < line.length() && (line.charAt(i) == ' ' || line.charAt(i) == '\t')) i++;
        return line.substring(0, i);
    }

    private int countChar(String s, char c) {
        int count = 0;
        boolean inString = false;
        for (char ch : s.toCharArray()) {
            if (ch == '"') inString = !inString;
            if (!inString && ch == c) count++;
        }
        return count;
    }
}