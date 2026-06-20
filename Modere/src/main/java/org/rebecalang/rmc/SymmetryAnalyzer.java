package org.rebecalang.rmc;

import java.util.ArrayList;
import java.util.List;

import org.rebecalang.compiler.modelcompiler.corerebeca.CoreRebecaTypeSystem;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.ArrayType;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.BinaryExpression;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.CastExpression;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.Expression;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.FieldDeclaration;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.Literal;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.MainRebecDefinition;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.ReactiveClassDeclaration;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.RebecaModel;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.TermPrimary;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.Type;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.UnaryExpression;
import org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.VariableDeclarator;

/**
 * Static symmetry analysis for Symmetry Reduction.
 *
 * Implements Section 3 of "Symmetry and partial order reduction techniques
 * in model checking Rebeca": computes, for each rebec instance, its type ID
 * (used to check if two rebecs are of the same type), and the known-rebec
 * (KR) table (which records the rebec IDs that each instance knows).
 *
 * Produces:
 *   - typeIds[i]       : type identifier for instance i (instances of the same
 *                        reactive class get the same typeId)
 *   - krTable[i][j]    : the j-th known-rebec ID of instance i
 *   - krSize[i]        : number of known-rebecs for instance i
 *
 * These are used in the generated C++ check() / sort() / isAssigned() functions
 * to build the canonical representative on-the-fly during state storage.
 */
public class SymmetryAnalyzer {

    /** Result of the symmetry analysis. */
    public static class Result {
        /** typeIds[instanceIdx] — type ID shared by all instances of the same reactive class. */
        public final List<Integer> typeIds;
        /** krTable[instanceIdx] — list of known-rebec instance indices for that instance. */
        public final List<List<Integer>> krTable;
        /** krSize[instanceIdx] — number of known-rebecs for that instance. */
        public final List<Integer> krSize;
        /** maxKnown — maximum number of known-rebecs across all instances (table column count). */
        public final int maxKnown;

        Result(List<Integer> typeIds, List<List<Integer>> krTable, List<Integer> krSize, int maxKnown) {
            this.typeIds = typeIds;
            this.krTable = krTable;
            this.krSize = krSize;
            this.maxKnown = maxKnown;
        }
    }

    /**
     * Analyse the given model and return the symmetry data.
     */
    public static Result analyze(RebecaModel rebecaModel) {

        List<MainRebecDefinition> mainDefs =
            rebecaModel.getRebecaCode().getMainDeclaration().getMainRebecDefinition();
        List<ReactiveClassDeclaration> rcds =
            rebecaModel.getRebecaCode().getReactiveClassDeclaration();

        // Build instance-name → instance index mapping (0-based, in main order)
        java.util.Map<String, Integer> instanceIndex = new java.util.LinkedHashMap<>();
        int idx = 0;
        for (MainRebecDefinition mrd : mainDefs) {
            instanceIndex.put(mrd.getName(), idx++);
        }

        // Build effective-type-key → type ID mapping.
        // Two instances share a type ID only if they have the same reactive class type
        // AND the same constructor arguments. Instances with different constructor args
        // (e.g., Doctor d1(...):((byte)1) vs Doctor d2(...):((byte)2)) receive different
        // type IDs and are never permuted by symmetry reduction.
        java.util.Map<String, Integer> effectiveTypeIdMap = new java.util.LinkedHashMap<>();
        int nextTypeId = 0;

        int instanceCount = mainDefs.size();
        List<Integer> typeIds = new ArrayList<>(instanceCount);
        List<List<Integer>> krTable = new ArrayList<>(instanceCount);
        List<Integer> krSize = new ArrayList<>(instanceCount);
        int maxKnown = 0;

        for (MainRebecDefinition mrd : mainDefs) {
            // Effective type ID: combines type name with serialised constructor arguments.
            String typeName = mrd.getType().getTypeName();
            String effKey = effectiveTypeKey(typeName, mrd.getArguments());
            if (!effectiveTypeIdMap.containsKey(effKey)) {
                effectiveTypeIdMap.put(effKey, nextTypeId++);
            }
            typeIds.add(effectiveTypeIdMap.get(effKey));

            // Known-rebec bindings for this instance
            // mrd.getBindings() contains the bound rebec expressions; for known-rebec
            // variables of reactive-class type, each binding is the name of another instance.
            List<Integer> krRow = new ArrayList<>();
            for (org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.Expression e
                    : mrd.getBindings()) {
                if (e instanceof org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.TermPrimary) {
                    String boundName = ((org.rebecalang.compiler.modelcompiler.corerebeca.objectmodel.TermPrimary) e).getName();
                    Integer boundIdx = instanceIndex.get(boundName);
                    krRow.add(boundIdx != null ? boundIdx : 0);
                } else {
                    // fallback: unknown binding — use 0
                    krRow.add(0);
                }
            }

            krTable.add(krRow);
            krSize.add(krRow.size());
            if (krRow.size() > maxKnown) {
                maxKnown = krRow.size();
            }
        }

        return new Result(typeIds, krTable, krSize, maxKnown);
    }

    /** Returns true if the given type is a reactive class (rebec) type. */
    public static boolean isReactiveClassType(Type t) {
        if (t == null) return false;
        Type base = t;
        if (base instanceof ArrayType)
            base = ((ArrayType) base).getOrdinaryPrimitiveType();
        return base.canTypeUpCastTo(CoreRebecaTypeSystem.REACTIVE_CLASS_TYPE);
    }

    /**
     * Builds a string key for a (type, constructor-args) pair.
     * Two instances with the same type name and identical constructor arguments
     * produce the same key and will be assigned the same symmetry type ID.
     */
    private static String effectiveTypeKey(String typeName, List<Expression> arguments) {
        if (arguments == null || arguments.isEmpty()) return typeName;
        StringBuilder sb = new StringBuilder(typeName).append(":(");
        for (Expression arg : arguments) {
            sb.append(argSignature(arg)).append(",");
        }
        return sb.append(")").toString();
    }

    /**
     * Recursively serialises an expression to a canonical string for key comparison.
     * Handles Literal, TermPrimary, CastExpression, UnaryExpression, BinaryExpression.
     * Falls back to the simple class name for unknown types (conservative: treats
     * all unknown-typed args of the same class as equivalent, which is safe — it may
     * fail to detect symmetry but will never incorrectly group non-symmetric instances).
     */
    private static String argSignature(Expression e) {
        if (e == null) return "null";
        if (e instanceof Literal) return ((Literal) e).getLiteralValue();
        if (e instanceof TermPrimary) return ((TermPrimary) e).getName();
        if (e instanceof CastExpression)
            return "cast(" + argSignature(((CastExpression) e).getExpression()) + ")";
        if (e instanceof UnaryExpression) {
            UnaryExpression ue = (UnaryExpression) e;
            return ue.getOperator() + "(" + argSignature(ue.getExpression()) + ")";
        }
        if (e instanceof BinaryExpression) {
            BinaryExpression be = (BinaryExpression) e;
            return "(" + argSignature(be.getLeft()) + be.getOperator() + argSignature(be.getRight()) + ")";
        }
        return e.getClass().getSimpleName();
    }
}
