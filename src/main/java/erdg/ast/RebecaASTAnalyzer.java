package erdg.ast;

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.antlr.v4.runtime.tree.ParseTree;
import org.antlr.v4.runtime.tree.TerminalNode;

import erdg.ast.AnalysisResult.MessageServer;
import erdg.ast.AnalysisResult.ReactiveClass;
import erdg.ast.AnalysisResult.RebecInstance;
import erdg.ast.AnalysisResult.Send;
import erdg.grammar.RebecaBaseVisitor;
import erdg.grammar.RebecaParser;

public class RebecaASTAnalyzer extends RebecaBaseVisitor<Void> {
    private final Map<String, Set<String>> stateVarsPerClass   = new LinkedHashMap<>();
    private final Map<String, Set<String>> knownRebecsPerClass = new LinkedHashMap<>();
    private final Map<String, Map<String, MutableMessageServer>> methodsPerClass = new LinkedHashMap<>();
    private final List<RebecInstance> mainInstances = new ArrayList<>();
    private String currentClass  = null;
    private String currentMethod = null;

    private static final class MutableMessageServer {
        Integer priority = null;
        final List<Send>  sends  = new ArrayList<>();
        final Set<String> reads  = new LinkedHashSet<>();
        final Set<String> writes = new LinkedHashSet<>();
    }

    public AnalysisResult analyze(ParseTree tree) {
        visit(tree);
        Map<String, ReactiveClass> classes = new LinkedHashMap<>();
        for (String clsName : stateVarsPerClass.keySet()) {
            Map<String, MessageServer> msMap = new LinkedHashMap<>();
            Map<String, MutableMessageServer> methods = methodsPerClass.get(clsName);
            if (methods != null) {
                for (var entry : methods.entrySet()) {
                    MutableMessageServer m = entry.getValue();
                    msMap.put(entry.getKey(), new MessageServer(entry.getKey(), m.priority,
                            List.copyOf(m.sends), Set.copyOf(m.reads), Set.copyOf(m.writes)));
                }
            }
            classes.put(clsName, new ReactiveClass(clsName,
                    Set.copyOf(stateVarsPerClass.get(clsName)), msMap));
        }
        return new AnalysisResult(classes, List.copyOf(mainInstances));
    }

    @Override
    public Void visitReactiveClassDeclaration(RebecaParser.ReactiveClassDeclarationContext ctx) {
        String className = ctx.IDENTIFIER(0).getText();
        currentClass = className;
        stateVarsPerClass.put(className, new LinkedHashSet<>());
        knownRebecsPerClass.put(className, new LinkedHashSet<>());
        methodsPerClass.put(className, new LinkedHashMap<>());
        visitChildren(ctx);
        currentClass = null;
        return null;
    }

    @Override
    public Void visitKnownRebecsDeclaration(RebecaParser.KnownRebecsDeclarationContext ctx) {
        if (currentClass == null) return null;
        for (var fd : ctx.fieldDeclaration())
            knownRebecsPerClass.get(currentClass).add(fd.IDENTIFIER().getText());
        return null;
    }

    @Override
    public Void visitStateVarsDeclaration(RebecaParser.StateVarsDeclarationContext ctx) {
        if (currentClass == null) return null;
        for (var fd : ctx.fieldDeclaration())
            stateVarsPerClass.get(currentClass).add(fd.IDENTIFIER().getText());
        return null;
    }

    @Override
    public Void visitConstructorDeclaration(RebecaParser.ConstructorDeclarationContext ctx) {
        if (currentClass == null) return null;
        String methodName = ctx.IDENTIFIER().getText();
        MutableMessageServer ms = new MutableMessageServer();
        methodsPerClass.get(currentClass).put(methodName, ms);
        currentMethod = methodName;
        visit(ctx.block());
        currentMethod = null;
        return null;
    }

    @Override
    public Void visitMsgsrvDeclaration(RebecaParser.MsgsrvDeclarationContext ctx) {
        if (currentClass == null) return null;
        String methodName = ctx.IDENTIFIER().getText();
        MutableMessageServer ms = new MutableMessageServer();
        methodsPerClass.get(currentClass).put(methodName, ms);

        // abstract msgsrv has no block — skip
        if (ctx.block() == null) return null;

        currentMethod = methodName;
        visit(ctx.block());
        currentMethod = null;
        return null;
    }

    @Override
    public Void visitExprStatement(RebecaParser.ExprStatementContext ctx) {
        if (currentClass == null || currentMethod == null) return null;
        processExpression(ctx.expression());
        return null;
    }

    @Override
    public Void visitVarDeclStatement(RebecaParser.VarDeclStatementContext ctx) {
        if (currentClass == null || currentMethod == null) return null;
        String varName = ctx.IDENTIFIER().getText();
        if (stateVarsPerClass.get(currentClass).contains(varName))
            methodsPerClass.get(currentClass).get(currentMethod).writes.add(varName);
        if (ctx.expression() != null) collectReads(ctx.expression());
        return null;
    }

    @Override
    public Void visitIfStatement(RebecaParser.IfStatementContext ctx) {
        if (currentClass == null || currentMethod == null) return null;
        collectReads(ctx.expression());
        for (var stmt : ctx.statement()) visit(stmt);
        return null;
    }

    @Override
    public Void visitWhileStatement(RebecaParser.WhileStatementContext ctx) {
        if (currentClass == null || currentMethod == null) return null;
        collectReads(ctx.expression());
        visit(ctx.statement());
        return null;
    }

    private void processExpression(RebecaParser.ExpressionContext ctx) {
        if (ctx instanceof RebecaParser.AssignExprContext assign) {
            RebecaParser.ExpressionContext lhs = assign.expression(0);
            if (lhs instanceof RebecaParser.CallOrVarExprContext cv) {
                String varName = cv.IDENTIFIER().getText();
                if (stateVarsPerClass.get(currentClass).contains(varName))
                    methodsPerClass.get(currentClass).get(currentMethod).writes.add(varName);
            }
            collectReads(assign.expression(1));
        } else if (ctx instanceof RebecaParser.MemberCallExprContext mc) {
            if (mc.arguments() != null) {
                String methodName = mc.IDENTIFIER().getText();
                String target = resolveReceiver(mc.expression());
                if (target != null)
                    methodsPerClass.get(currentClass).get(currentMethod).sends.add(new Send(target, methodName));
            }
            collectReads(mc.expression());
        } else {
            collectReads(ctx);
        }
    }

    private String resolveReceiver(RebecaParser.ExpressionContext expr) {
        if (expr instanceof RebecaParser.SelfExprContext) return "self";
        if (expr instanceof RebecaParser.CallOrVarExprContext cv) {
            String name = cv.IDENTIFIER().getText();
            Set<String> known = knownRebecsPerClass.getOrDefault(currentClass, Set.of());
            if (known.contains(name)) return name;
        }
        return null;
    }

    private void collectReads(RebecaParser.ExpressionContext ctx) {
        if (ctx == null || currentClass == null || currentMethod == null) return;
        if (ctx instanceof RebecaParser.CallOrVarExprContext cv) {
            String name = cv.IDENTIFIER().getText();
            Set<String> stateVars = stateVarsPerClass.get(currentClass);
            if (stateVars != null && stateVars.contains(name))
                methodsPerClass.get(currentClass).get(currentMethod).reads.add(name);
        }
        for (int i = 0; i < ctx.getChildCount(); i++) {
            ParseTree child = ctx.getChild(i);
            if (child instanceof RebecaParser.ExpressionContext ec) collectReads(ec);
        }
    }

    @Override
    public Void visitMainRebecDefinition(RebecaParser.MainRebecDefinitionContext ctx) {
        String reactiveClass = ctx.type().getText();
        String instanceName  = ctx.IDENTIFIER().getText();
        Map<String, String> bindings = new LinkedHashMap<>();
        if (ctx.bindingList() != null) {
            List<String> boundNames = new ArrayList<>();
            for (TerminalNode id : ctx.bindingList().IDENTIFIER()) boundNames.add(id.getText());
            List<String> knownAliases = new ArrayList<>(
                    knownRebecsPerClass.getOrDefault(reactiveClass, new LinkedHashSet<>()));
            for (int i = 0; i < Math.min(knownAliases.size(), boundNames.size()); i++)
                bindings.put(knownAliases.get(i), boundNames.get(i));
        }
        mainInstances.add(new RebecInstance(instanceName, reactiveClass, bindings, null));
        return null;
    }
}
