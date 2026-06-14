package erdg.ast;

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.antlr.v4.runtime.tree.ParseTree;

import erdg.ast.AnalysisResult.MessageServer;
import erdg.ast.AnalysisResult.ReactiveClass;
import erdg.ast.AnalysisResult.RebecInstance;
import erdg.ast.AnalysisResult.Send;
import erdg.grammar.BabelBaseVisitor;
import erdg.grammar.BabelParser;

public class ASTAnalyzer extends BabelBaseVisitor<Void> {
    private final Map<String, Set<String>> stateVarsPerClass = new LinkedHashMap<>();
    private final Map<String, Map<String, MutableMessageServer>> methodsPerClass = new LinkedHashMap<>();
    private final List<RebecInstance> mainInstances = new ArrayList<>();
    private String currentClass = null;
    private String currentMethod = null;

    private static final class MutableMessageServer {
        Integer priority;
        final List<Send> sends = new ArrayList<>();
        final Set<String> reads = new LinkedHashSet<>();
        final Set<String> writes = new LinkedHashSet<>();
    }

    public AnalysisResult analyze(ParseTree tree) {
        visit(tree);
        Map<String, ReactiveClass> classes = new LinkedHashMap<>();
        for (String clsName : stateVarsPerClass.keySet()) {
            Map<String, MessageServer> msMap = new LinkedHashMap<>();
            for (Map.Entry<String, MutableMessageServer> e : methodsPerClass.get(clsName).entrySet()) {
                MutableMessageServer m = e.getValue();
                msMap.put(e.getKey(), new MessageServer(e.getKey(), m.priority,
                        List.copyOf(m.sends), Set.copyOf(m.reads), Set.copyOf(m.writes)));
            }
            classes.put(clsName, new ReactiveClass(clsName,
                    Set.copyOf(stateVarsPerClass.get(clsName)), msMap));
        }
        return new AnalysisResult(classes, List.copyOf(mainInstances));
    }

    @Override
    public Void visitClassDef(BabelParser.ClassDefContext ctx) {
        String className = ctx.identifier().getText();
        currentClass = className;
        stateVarsPerClass.put(className, new LinkedHashSet<>());
        methodsPerClass.put(className, new LinkedHashMap<>());
        if (ctx.vars() != null) visit(ctx.vars());
        for (BabelParser.MethodContext m : ctx.method()) visit(m);
        currentClass = null;
        return null;
    }

    @Override
    public Void visitVarDecl(BabelParser.VarDeclContext ctx) {
        if (currentClass == null) return null;
        stateVarsPerClass.get(currentClass).add(ctx.identifier().getText());
        return null;
    }

    @Override
    public Void visitMethod(BabelParser.MethodContext ctx) {
        if (currentClass == null) return null;
        String methodName = ctx.LOWER_IDENT().getText();
        Integer priority = null;
        if (ctx.priorityBlock() != null)
            priority = Integer.parseInt(ctx.priorityBlock().NUMBER().getText());
        MutableMessageServer ms = new MutableMessageServer();
        ms.priority = priority;
        methodsPerClass.get(currentClass).put(methodName, ms);
        currentMethod = methodName;
        for (BabelParser.StmtContext s : ctx.stmt()) visit(s);
        currentMethod = null;
        return null;
    }

    @Override
    public Void visitAssignStatement(BabelParser.AssignStatementContext ctx) {
        BabelParser.AssignStmtContext assign = ctx.assignStmt();
        String varName = assign.identifier().getText();
        if (currentClass != null && currentMethod != null)
            methodsPerClass.get(currentClass).get(currentMethod).writes.add(varName);
        collectReads(assign.expr());
        return null;
    }

    @Override
    public Void visitSendStatement(BabelParser.SendStatementContext ctx) {
        BabelParser.SendStmtContext send = ctx.sendStmt();
        String target = send.identifier().getText();
        String message = send.LOWER_IDENT().getText();
        if (currentClass != null && currentMethod != null)
            methodsPerClass.get(currentClass).get(currentMethod).sends.add(new Send(target, message));
        return null;
    }

    @Override
    public Void visitIfStatement(BabelParser.IfStatementContext ctx) {
        BabelParser.IfStmtContext ifs = ctx.ifStmt();
        collectReads(ifs.expr());
        for (BabelParser.StmtContext s : ifs.stmt()) visit(s);
        return null;
    }

    private void collectReads(ParseTree node) {
        if (currentClass == null || currentMethod == null || node == null) return;
        if (node instanceof BabelParser.NameExprContext nameExpr) {
            String name = nameExpr.identifier().getText();
            if (stateVarsPerClass.get(currentClass).contains(name))
                methodsPerClass.get(currentClass).get(currentMethod).reads.add(name);
        }
        for (int i = 0; i < node.getChildCount(); i++) collectReads(node.getChild(i));
    }

    @Override
    public Void visitRebecInstance(BabelParser.RebecInstanceContext ctx) {
        List<BabelParser.IdentifierContext> ids = ctx.identifier();
        String instanceName = ids.get(0).getText();
        String className = ids.get(1).getText();
        Integer priority = null;
        if (ctx.priorityBlock() != null)
            priority = Integer.parseInt(ctx.priorityBlock().NUMBER().getText());
        mainInstances.add(new RebecInstance(instanceName, className, Map.of(), priority));
        return null;
    }
}
