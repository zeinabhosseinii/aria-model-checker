// Generated from erdg/grammar/Babel.g4 by ANTLR 4.13.1
package erdg.grammar;
import org.antlr.v4.runtime.tree.ParseTreeVisitor;

/**
 * This interface defines a complete generic visitor for a parse tree produced
 * by {@link BabelParser}.
 *
 * @param <T> The return type of the visit operation. Use {@link Void} for
 * operations with no return type.
 */
public interface BabelVisitor<T> extends ParseTreeVisitor<T> {
	/**
	 * Visit a parse tree produced by {@link BabelParser#model}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitModel(BabelParser.ModelContext ctx);
	/**
	 * Visit a parse tree produced by {@link BabelParser#mainBlock}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitMainBlock(BabelParser.MainBlockContext ctx);
	/**
	 * Visit a parse tree produced by {@link BabelParser#rebecInstance}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitRebecInstance(BabelParser.RebecInstanceContext ctx);
	/**
	 * Visit a parse tree produced by {@link BabelParser#priorityBlock}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitPriorityBlock(BabelParser.PriorityBlockContext ctx);
	/**
	 * Visit a parse tree produced by {@link BabelParser#classDef}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitClassDef(BabelParser.ClassDefContext ctx);
	/**
	 * Visit a parse tree produced by {@link BabelParser#vars}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitVars(BabelParser.VarsContext ctx);
	/**
	 * Visit a parse tree produced by {@link BabelParser#varDecl}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitVarDecl(BabelParser.VarDeclContext ctx);
	/**
	 * Visit a parse tree produced by {@link BabelParser#method}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitMethod(BabelParser.MethodContext ctx);
	/**
	 * Visit a parse tree produced by the {@code AssignStatement}
	 * labeled alternative in {@link BabelParser#stmt}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitAssignStatement(BabelParser.AssignStatementContext ctx);
	/**
	 * Visit a parse tree produced by the {@code SendStatement}
	 * labeled alternative in {@link BabelParser#stmt}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSendStatement(BabelParser.SendStatementContext ctx);
	/**
	 * Visit a parse tree produced by the {@code SkipStatement}
	 * labeled alternative in {@link BabelParser#stmt}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSkipStatement(BabelParser.SkipStatementContext ctx);
	/**
	 * Visit a parse tree produced by the {@code IncStatement}
	 * labeled alternative in {@link BabelParser#stmt}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitIncStatement(BabelParser.IncStatementContext ctx);
	/**
	 * Visit a parse tree produced by the {@code IfStatement}
	 * labeled alternative in {@link BabelParser#stmt}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitIfStatement(BabelParser.IfStatementContext ctx);
	/**
	 * Visit a parse tree produced by {@link BabelParser#assignStmt}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitAssignStmt(BabelParser.AssignStmtContext ctx);
	/**
	 * Visit a parse tree produced by {@link BabelParser#sendStmt}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSendStmt(BabelParser.SendStmtContext ctx);
	/**
	 * Visit a parse tree produced by {@link BabelParser#ifStmt}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitIfStmt(BabelParser.IfStmtContext ctx);
	/**
	 * Visit a parse tree produced by the {@code AndExpr}
	 * labeled alternative in {@link BabelParser#expr}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitAndExpr(BabelParser.AndExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code StringExpr}
	 * labeled alternative in {@link BabelParser#expr}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitStringExpr(BabelParser.StringExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code TrueExpr}
	 * labeled alternative in {@link BabelParser#expr}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTrueExpr(BabelParser.TrueExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code NameExpr}
	 * labeled alternative in {@link BabelParser#expr}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitNameExpr(BabelParser.NameExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code OrExpr}
	 * labeled alternative in {@link BabelParser#expr}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitOrExpr(BabelParser.OrExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code FalseExpr}
	 * labeled alternative in {@link BabelParser#expr}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitFalseExpr(BabelParser.FalseExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code GroupExpr}
	 * labeled alternative in {@link BabelParser#expr}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitGroupExpr(BabelParser.GroupExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code MulDivExpr}
	 * labeled alternative in {@link BabelParser#expr}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitMulDivExpr(BabelParser.MulDivExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code NumberExpr}
	 * labeled alternative in {@link BabelParser#expr}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitNumberExpr(BabelParser.NumberExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code CompareExpr}
	 * labeled alternative in {@link BabelParser#expr}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCompareExpr(BabelParser.CompareExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code EqExpr}
	 * labeled alternative in {@link BabelParser#expr}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitEqExpr(BabelParser.EqExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code NotExpr}
	 * labeled alternative in {@link BabelParser#expr}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitNotExpr(BabelParser.NotExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code AddSubExpr}
	 * labeled alternative in {@link BabelParser#expr}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitAddSubExpr(BabelParser.AddSubExprContext ctx);
	/**
	 * Visit a parse tree produced by {@link BabelParser#identifier}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitIdentifier(BabelParser.IdentifierContext ctx);
}