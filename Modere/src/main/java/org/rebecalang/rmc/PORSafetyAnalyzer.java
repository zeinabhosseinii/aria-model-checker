package org.rebecalang.rmc;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.rebecalang.compiler.modelcompiler.corerebeca.CoreRebecaTypeSystem;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.ArrayType;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.BinaryExpression;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.BlockStatement;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.ConditionalStatement;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.DotPrimary;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.Expression;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.FieldDeclaration;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.ForStatement;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.MainRebecDefinition;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.MsgsrvDeclaration;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.ReactiveClassDeclaration;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.RebecaModel;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.Statement;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.TermPrimary;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.Type;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.UnaryExpression;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.VariableDeclarator;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.WhileStatement;

/**
 * Static safety analysis for Partial Order Reduction (POR).
 *
 * Implements Theorem 3 of "Symmetry and partial order reduction techniques
 * in model checking Rebeca": a message server is safe if all its send
 * sub-actions are safe. A send to target queue t is safe iff t has exactly
 * one sender instance across the entire model (Lemma 7).
 *
 * Produces a 2-D boolean table safeActions[instanceIndex][msgSrvId] used
 * by the generated C++ areActiveActionsSafe() function.
 *
 * Inheritance is handled correctly: for each concrete instance the full
 * inheritance chain is walked so that msgsrvs declared in abstract base
 * classes (e.g. reachBridge / youMayPass in an abstract Train) are analysed
 * under the concrete instance's identity and bindings.  Known-rebec names
 * are resolved against the full inherited list so that a subclass that
 * inherits all its knownrebecs from a parent can still resolve targets.
 */
public class PORSafetyAnalyzer {

    /** Result of the safety analysis. */
    public static class Result {
        /** safeActions[instanceIdx][msgId] — true iff that msgsrv is safe for that instance. */
        public final List<List<Boolean>> safeActionsTable;
        /** Largest message ID across all types; second dimension of the array is maxMsgId+1. */
        public final int maxMsgId;

        Result(List<List<Boolean>> safeActionsTable, int maxMsgId) {
            this.safeActionsTable = safeActionsTable;
            this.maxMsgId = maxMsgId;
        }
    }

    /**
     * Analyse the given model and return a safety table.
     */
    public static Result analyze(RebecaModel rebecaModel) {

        List<MainRebecDefinition> mainDefs =
            rebecaModel.getRebecaCode().getMainDeclaration().getMainRebecDefinition();
        List<ReactiveClassDeclaration> rcds =
            rebecaModel.getRebecaCode().getReactiveClassDeclaration();

        // ── instance maps ────────────────────────────────────────────────────
        // instanceIndex  : instance-name → 0-based index in main
        // instanceTypeName: instance-name → type name
        Map<String, Integer> instanceIndex = new HashMap<>();
        Map<String, String>  instanceTypeName = new HashMap<>();
        List<String>         instanceNames = new ArrayList<>();

        int idx = 0;
        for (MainRebecDefinition mrd : mainDefs) {
            String name = mrd.getName();
            instanceIndex.put(name, idx++);
            instanceTypeName.put(name, mrd.getType().getTypeName());
            instanceNames.add(name);
        }

        // ── type maps ─────────────────────────────────────────────────────────
        Map<String, ReactiveClassDeclaration> typeToRcd = new HashMap<>();
        for (ReactiveClassDeclaration rcd : rcds)
            typeToRcd.put(rcd.getName(), rcd);

        // ── full inherited known-rebec names per type ─────────────────────────
        // For each type, collect known-rebec variable names from root to subclass
        // so that the index into this list matches the binding order in main{}.
        // This handles subclasses that inherit all their knownrebecs from a parent.
        Map<String, List<String>> fullKnownRebecNames = new HashMap<>();
        for (ReactiveClassDeclaration rcd : rcds)
            fullKnownRebecNames.put(rcd.getName(),
                    collectAllKnownRebecNames(rcd, typeToRcd));

        // ── bindings per instance ─────────────────────────────────────────────
        // instanceBindings[instanceName] = ordered list of bound instance names
        Map<String, List<String>> instanceBindings = new HashMap<>();
        for (MainRebecDefinition mrd : mainDefs) {
            List<String> bindings = new ArrayList<>();
            for (Expression e : mrd.getBindings()) {
                if (e instanceof TermPrimary)
                    bindings.add(((TermPrimary) e).getName());
                else
                    bindings.add(null); // unresolvable
            }
            instanceBindings.put(mrd.getName(), bindings);
        }

        // ── compute parentMsgSrvCount (starting message ID) per type ──────────
        Map<String, Integer> startMsgId = new HashMap<>();
        for (ReactiveClassDeclaration rcd : rcds)
            startMsgId.put(rcd.getName(), parentMsgSrvCount(rcd, typeToRcd));

        // ── compute maxMsgId across all types ──────────────────────────────────
        int maxMsgId = 0;
        for (ReactiveClassDeclaration rcd : rcds) {
            int lastId = startMsgId.get(rcd.getName()) + rcd.getMsgsrvs().size() - 1;
            if (lastId > maxMsgId) maxMsgId = lastId;
        }

        // ── build senders map ─────────────────────────────────────────────────
        // sendersOf[targetInstanceIdx] = set of sender INSTANCE indices.
        // Tracks which rebec instances ever send to a given target, across the
        // full inheritance chain.  Per-instance (not per-(instance,msgsrv)) to
        // match the paper's "exclusive sender rebec" condition (Lemma 7): a
        // rebec that sends to itself from two different msgsrvs still counts as
        // exactly one sender.
        //
        // hasUnknownSend[instanceIdx][msgId] = true if any send in that msgsrv
        // has an unresolvable target.
        int instanceCount = instanceNames.size();
        @SuppressWarnings("unchecked")
        Set<Integer>[] sendersOf = new Set[instanceCount];
        for (int i = 0; i < instanceCount; i++)
            sendersOf[i] = new HashSet<>();

        boolean[][] hasUnknownSend = new boolean[instanceCount][maxMsgId + 1];

        for (String instName : instanceNames) {
            int instIdx = instanceIndex.get(instName);
            String typeName = instanceTypeName.get(instName);
            ReactiveClassDeclaration concreteRcd = typeToRcd.get(typeName);
            if (concreteRcd == null) continue;

            List<String> fullKrNames = fullKnownRebecNames.get(typeName);
            List<String> bindings = instanceBindings.get(instName);

            // Walk the full inheritance chain (concrete class first, then ancestors).
            ReactiveClassDeclaration ancestor = concreteRcd;
            while (ancestor != null) {
                int base = startMsgId.get(ancestor.getName());
                List<MsgsrvDeclaration> msgsrvs = ancestor.getMsgsrvs();
                for (int mi = 0; mi < msgsrvs.size(); mi++) {
                    MsgsrvDeclaration msgsrv = msgsrvs.get(mi);
                    int msgId = base + mi;

                    List<DotPrimary> sends = new ArrayList<>();
                    if (msgsrv.getBlock() != null)
                        collectSends(msgsrv.getBlock(), sends);

                    for (DotPrimary send : sends) {
                        Expression left = send.getLeft();
                        String targetName = resolveTarget(left, instName, fullKrNames, bindings);

                        if (targetName == null) {
                            // Target is statically unresolvable (e.g. sender keyword, cast
                            // expression) — we can't pin it to one instance, so conservatively
                            // record this instance as a potential sender to every target.
                            // This prevents other msgsrvs from being incorrectly marked safe
                            // because the unknown-target send was invisible to sendersOf.
                            hasUnknownSend[instIdx][msgId] = true;
                            for (int i = 0; i < instanceCount; i++)
                                sendersOf[i].add(instIdx);
                        } else {
                            Integer targetIdx = instanceIndex.get(targetName);
                            if (targetIdx == null) {
                                hasUnknownSend[instIdx][msgId] = true;
                                for (int i = 0; i < instanceCount; i++)
                                    sendersOf[i].add(instIdx);
                            } else {
                                sendersOf[targetIdx].add(instIdx);
                            }
                        }
                    }
                }
                ancestor = ancestor.getExtends() != null
                        ? typeToRcd.get(ancestor.getExtends().getTypeName()) : null;
            }
        }

        // ── determine safety per (instance, msgId) ────────────────────────────
        // Allocate the result table: rows = instances, columns = [0..maxMsgId]
        boolean[][] safe = new boolean[instanceCount][maxMsgId + 1];

        // Index 0 is the "empty queue" marker — never executed, treat as safe.
        for (int i = 0; i < instanceCount; i++)
            safe[i][0] = true;

        for (String instName : instanceNames) {
            int instIdx = instanceIndex.get(instName);
            String typeName = instanceTypeName.get(instName);
            ReactiveClassDeclaration concreteRcd = typeToRcd.get(typeName);
            if (concreteRcd == null) continue;

            List<String> fullKrNames = fullKnownRebecNames.get(typeName);
            List<String> bindings = instanceBindings.get(instName);

            // Walk the full inheritance chain.
            ReactiveClassDeclaration ancestor = concreteRcd;
            while (ancestor != null) {
                int base = startMsgId.get(ancestor.getName());
                List<MsgsrvDeclaration> msgsrvs = ancestor.getMsgsrvs();
                for (int mi = 0; mi < msgsrvs.size(); mi++) {
                    MsgsrvDeclaration msgsrv = msgsrvs.get(mi);
                    int msgId = base + mi;

                    if (hasUnknownSend[instIdx][msgId]) {
                        safe[instIdx][msgId] = false;
                        continue;
                    }

                    // Check all sends: each target must have exactly one sender instance.
                    List<DotPrimary> sends = new ArrayList<>();
                    if (msgsrv.getBlock() != null)
                        collectSends(msgsrv.getBlock(), sends);

                    boolean allSafe = true;
                    for (DotPrimary send : sends) {
                        Expression left = send.getLeft();
                        String targetName = resolveTarget(left, instName, fullKrNames, bindings);
                        if (targetName == null) { allSafe = false; break; }
                        Integer targetIdx = instanceIndex.get(targetName);
                        if (targetIdx == null) { allSafe = false; break; }
                        if (sendersOf[targetIdx].size() != 1) { allSafe = false; break; }
                    }
                    safe[instIdx][msgId] = allSafe;
                }
                ancestor = ancestor.getExtends() != null
                        ? typeToRcd.get(ancestor.getExtends().getTypeName()) : null;
            }
        }

        // ── convert to List<List<Boolean>> for Velocity ───────────────────────
        List<List<Boolean>> table = new ArrayList<>();
        for (int i = 0; i < instanceCount; i++) {
            List<Boolean> row = new ArrayList<>();
            for (int j = 0; j <= maxMsgId; j++)
                row.add(safe[i][j]);
            table.add(row);
        }

        return new Result(table, maxMsgId);
    }

    // ── helpers ───────────────────────────────────────────────────────────────

    /**
     * Collect all known-rebec variable names for a type in inheritance order
     * (root ancestor first, concrete class last).  This ordering matches the
     * binding order in main{} so that slot indices align with instance bindings.
     */
    private static List<String> collectAllKnownRebecNames(ReactiveClassDeclaration rcd,
            Map<String, ReactiveClassDeclaration> typeToRcd) {
        // Build the chain from root to concrete class.
        List<ReactiveClassDeclaration> chain = new ArrayList<>();
        ReactiveClassDeclaration current = rcd;
        while (current != null) {
            chain.add(0, current); // prepend so root is first
            current = current.getExtends() != null
                    ? typeToRcd.get(current.getExtends().getTypeName()) : null;
        }
        List<String> names = new ArrayList<>();
        for (ReactiveClassDeclaration ancestor : chain)
            for (FieldDeclaration fd : ancestor.getKnownRebecs())
                for (VariableDeclarator vd : fd.getVariableDeclarators())
                    names.add(vd.getVariableName());
        return names;
    }

    /**
     * Resolve the target instance name of a send's left expression.
     * Returns null if the target cannot be statically determined.
     */
    private static String resolveTarget(Expression left, String senderInstance,
            List<String> krNames, List<String> bindings) {
        if (!(left instanceof TermPrimary))
            return null; // complex expression — conservatively unknown

        String name = ((TermPrimary) left).getName();
        if ("self".equals(name))
            return senderInstance;
        if ("sender".equals(name))
            return null; // dynamically determined — unknown

        // Check known-rebec names (full inherited list)
        int slot = krNames.indexOf(name);
        if (slot >= 0 && bindings != null && slot < bindings.size())
            return bindings.get(slot); // may be null if binding was unresolvable

        return null; // unknown local variable or state var holding a rebec reference
    }

    /**
     * Recursively collect all send DotPrimary nodes from a statement.
     * A send is a DotPrimary whose left sub-expression has a reactive-class type.
     */
    private static void collectSends(Statement stmt, List<DotPrimary> sends) {
        if (stmt == null) return;
        if (stmt instanceof BlockStatement) {
            for (Statement s : ((BlockStatement) stmt).getStatements())
                collectSends(s, sends);
        } else if (stmt instanceof ConditionalStatement) {
            ConditionalStatement cs = (ConditionalStatement) stmt;
            collectSendsFromExpr(cs.getCondition(), sends);
            collectSends(cs.getStatement(), sends);
            if (cs.getElseStatement() != null)
                collectSends(cs.getElseStatement(), sends);
        } else if (stmt instanceof WhileStatement) {
            WhileStatement ws = (WhileStatement) stmt;
            collectSendsFromExpr(ws.getCondition(), sends);
            collectSends(ws.getStatement(), sends);
        } else if (stmt instanceof ForStatement) {
            collectSends(((ForStatement) stmt).getStatement(), sends);
        } else if (stmt instanceof Expression) {
            collectSendsFromExpr((Expression) stmt, sends);
        }
    }

    private static void collectSendsFromExpr(Expression expr, List<DotPrimary> sends) {
        if (expr == null) return;
        if (expr instanceof DotPrimary) {
            DotPrimary dp = (DotPrimary) expr;
            Expression left = dp.getLeft();
            if (left != null && isReactiveClassType(left.getType())) {
                sends.add(dp);
                // Also recurse into message arguments (in the right TermPrimary)
                collectSendsFromExpr(dp.getRight(), sends);
            } else {
                collectSendsFromExpr(left, sends);
                collectSendsFromExpr(dp.getRight(), sends);
            }
        } else if (expr instanceof BinaryExpression) {
            BinaryExpression be = (BinaryExpression) expr;
            collectSendsFromExpr(be.getLeft(), sends);
            collectSendsFromExpr(be.getRight(), sends);
        } else if (expr instanceof UnaryExpression) {
            collectSendsFromExpr(((UnaryExpression) expr).getExpression(), sends);
        } else if (expr instanceof TermPrimary) {
            TermPrimary tp = (TermPrimary) expr;
            if (tp.getParentSuffixPrimary() != null) {
                for (Expression arg : tp.getParentSuffixPrimary().getArguments())
                    collectSendsFromExpr(arg, sends);
            }
        }
    }

    private static boolean isReactiveClassType(Type t) {
        if (t == null) return false;
        Type base = t;
        if (base instanceof ArrayType)
            base = ((ArrayType) base).getOrdinaryPrimitiveType();
        return base.canTypeUpCastTo(CoreRebecaTypeSystem.REACTIVE_CLASS_TYPE);
    }

    /**
     * Compute the base message-ID offset for a reactive class
     * (replicates CoreRebecaFileGenerator.parentMethodCounts).
     * Returns 1 + sum of all ancestor msgsrv counts.
     */
    private static int parentMsgSrvCount(ReactiveClassDeclaration rcd,
            Map<String, ReactiveClassDeclaration> typeToRcd) {
        int cnt = 1; // 0 is reserved for "empty queue"
        ReactiveClassDeclaration current = rcd;
        while (current.getExtends() != null) {
            String parentName = current.getExtends().getTypeName();
            current = typeToRcd.get(parentName);
            if (current == null) break;
            cnt += current.getMsgsrvs().size();
        }
        return cnt;
    }
}
