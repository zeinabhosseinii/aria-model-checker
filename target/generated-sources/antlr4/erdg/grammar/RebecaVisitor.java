// Generated from erdg/grammar/Rebeca.g4 by ANTLR 4.13.1
package erdg.grammar;
import org.antlr.v4.runtime.tree.ParseTreeVisitor;

/**
 * This interface defines a complete generic visitor for a parse tree produced
 * by {@link RebecaParser}.
 *
 * @param <T> The return type of the visit operation. Use {@link Void} for
 * operations with no return type.
 */
public interface RebecaVisitor<T> extends ParseTreeVisitor<T> {
	/**
	 * Visit a parse tree produced by {@link RebecaParser#model}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitModel(RebecaParser.ModelContext ctx);
	/**
	 * Visit a parse tree produced by {@link RebecaParser#envDeclaration}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitEnvDeclaration(RebecaParser.EnvDeclarationContext ctx);
	/**
	 * Visit a parse tree produced by {@link RebecaParser#mainDeclaration}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitMainDeclaration(RebecaParser.MainDeclarationContext ctx);
	/**
	 * Visit a parse tree produced by {@link RebecaParser#mainRebecDefinition}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitMainRebecDefinition(RebecaParser.MainRebecDefinitionContext ctx);
	/**
	 * Visit a parse tree produced by {@link RebecaParser#bindingList}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitBindingList(RebecaParser.BindingListContext ctx);
	/**
	 * Visit a parse tree produced by {@link RebecaParser#argList}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitArgList(RebecaParser.ArgListContext ctx);
	/**
	 * Visit a parse tree produced by {@link RebecaParser#argItem}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitArgItem(RebecaParser.ArgItemContext ctx);
	/**
	 * Visit a parse tree produced by {@link RebecaParser#reactiveClassDeclaration}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitReactiveClassDeclaration(RebecaParser.ReactiveClassDeclarationContext ctx);
	/**
	 * Visit a parse tree produced by {@link RebecaParser#knownRebecsDeclaration}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitKnownRebecsDeclaration(RebecaParser.KnownRebecsDeclarationContext ctx);
	/**
	 * Visit a parse tree produced by {@link RebecaParser#stateVarsDeclaration}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitStateVarsDeclaration(RebecaParser.StateVarsDeclarationContext ctx);
	/**
	 * Visit a parse tree produced by {@link RebecaParser#fieldDeclaration}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitFieldDeclaration(RebecaParser.FieldDeclarationContext ctx);
	/**
	 * Visit a parse tree produced by {@link RebecaParser#constructorDeclaration}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitConstructorDeclaration(RebecaParser.ConstructorDeclarationContext ctx);
	/**
	 * Visit a parse tree produced by {@link RebecaParser#msgsrvDeclaration}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitMsgsrvDeclaration(RebecaParser.MsgsrvDeclarationContext ctx);
	/**
	 * Visit a parse tree produced by {@link RebecaParser#formalParams}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitFormalParams(RebecaParser.FormalParamsContext ctx);
	/**
	 * Visit a parse tree produced by {@link RebecaParser#formalParam}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitFormalParam(RebecaParser.FormalParamContext ctx);
	/**
	 * Visit a parse tree produced by {@link RebecaParser#type}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitType(RebecaParser.TypeContext ctx);
	/**
	 * Visit a parse tree produced by {@link RebecaParser#block}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitBlock(RebecaParser.BlockContext ctx);
	/**
	 * Visit a parse tree produced by the {@code ExprStatement}
	 * labeled alternative in {@link RebecaParser#statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitExprStatement(RebecaParser.ExprStatementContext ctx);
	/**
	 * Visit a parse tree produced by the {@code VarDeclStatement}
	 * labeled alternative in {@link RebecaParser#statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitVarDeclStatement(RebecaParser.VarDeclStatementContext ctx);
	/**
	 * Visit a parse tree produced by the {@code IfStatement}
	 * labeled alternative in {@link RebecaParser#statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitIfStatement(RebecaParser.IfStatementContext ctx);
	/**
	 * Visit a parse tree produced by the {@code WhileStatement}
	 * labeled alternative in {@link RebecaParser#statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitWhileStatement(RebecaParser.WhileStatementContext ctx);
	/**
	 * Visit a parse tree produced by the {@code ForStatement}
	 * labeled alternative in {@link RebecaParser#statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitForStatement(RebecaParser.ForStatementContext ctx);
	/**
	 * Visit a parse tree produced by the {@code ReturnStatement}
	 * labeled alternative in {@link RebecaParser#statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitReturnStatement(RebecaParser.ReturnStatementContext ctx);
	/**
	 * Visit a parse tree produced by the {@code BreakStatement}
	 * labeled alternative in {@link RebecaParser#statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitBreakStatement(RebecaParser.BreakStatementContext ctx);
	/**
	 * Visit a parse tree produced by the {@code ContinueStatement}
	 * labeled alternative in {@link RebecaParser#statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitContinueStatement(RebecaParser.ContinueStatementContext ctx);
	/**
	 * Visit a parse tree produced by the {@code AssertStatement}
	 * labeled alternative in {@link RebecaParser#statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitAssertStatement(RebecaParser.AssertStatementContext ctx);
	/**
	 * Visit a parse tree produced by the {@code BlockStatement}
	 * labeled alternative in {@link RebecaParser#statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitBlockStatement(RebecaParser.BlockStatementContext ctx);
	/**
	 * Visit a parse tree produced by the {@code EmptyStatement}
	 * labeled alternative in {@link RebecaParser#statement}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitEmptyStatement(RebecaParser.EmptyStatementContext ctx);
	/**
	 * Visit a parse tree produced by {@link RebecaParser#forInit}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitForInit(RebecaParser.ForInitContext ctx);
	/**
	 * Visit a parse tree produced by {@link RebecaParser#expressionList}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitExpressionList(RebecaParser.ExpressionListContext ctx);
	/**
	 * Visit a parse tree produced by the {@code CallOrVarExpr}
	 * labeled alternative in {@link RebecaParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCallOrVarExpr(RebecaParser.CallOrVarExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code MulExpr}
	 * labeled alternative in {@link RebecaParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitMulExpr(RebecaParser.MulExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code AndExpr}
	 * labeled alternative in {@link RebecaParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitAndExpr(RebecaParser.AndExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code NondetExpr}
	 * labeled alternative in {@link RebecaParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitNondetExpr(RebecaParser.NondetExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code SelfExpr}
	 * labeled alternative in {@link RebecaParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSelfExpr(RebecaParser.SelfExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code InstanceofExpr}
	 * labeled alternative in {@link RebecaParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitInstanceofExpr(RebecaParser.InstanceofExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code AddExpr}
	 * labeled alternative in {@link RebecaParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitAddExpr(RebecaParser.AddExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code SenderExpr}
	 * labeled alternative in {@link RebecaParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitSenderExpr(RebecaParser.SenderExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code UnaryExpr}
	 * labeled alternative in {@link RebecaParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitUnaryExpr(RebecaParser.UnaryExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code OrExpr}
	 * labeled alternative in {@link RebecaParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitOrExpr(RebecaParser.OrExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code NullExpr}
	 * labeled alternative in {@link RebecaParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitNullExpr(RebecaParser.NullExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code AssignExpr}
	 * labeled alternative in {@link RebecaParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitAssignExpr(RebecaParser.AssignExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code PostfixExpr}
	 * labeled alternative in {@link RebecaParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitPostfixExpr(RebecaParser.PostfixExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code ArrayExpr}
	 * labeled alternative in {@link RebecaParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitArrayExpr(RebecaParser.ArrayExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code CompareExpr}
	 * labeled alternative in {@link RebecaParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCompareExpr(RebecaParser.CompareExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code EqExpr}
	 * labeled alternative in {@link RebecaParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitEqExpr(RebecaParser.EqExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code CastExpr}
	 * labeled alternative in {@link RebecaParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCastExpr(RebecaParser.CastExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code NewExpr}
	 * labeled alternative in {@link RebecaParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitNewExpr(RebecaParser.NewExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code LiteralExpr}
	 * labeled alternative in {@link RebecaParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitLiteralExpr(RebecaParser.LiteralExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code ParenExpr}
	 * labeled alternative in {@link RebecaParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitParenExpr(RebecaParser.ParenExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code TernaryExpr}
	 * labeled alternative in {@link RebecaParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitTernaryExpr(RebecaParser.TernaryExprContext ctx);
	/**
	 * Visit a parse tree produced by the {@code MemberCallExpr}
	 * labeled alternative in {@link RebecaParser#expression}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitMemberCallExpr(RebecaParser.MemberCallExprContext ctx);
	/**
	 * Visit a parse tree produced by {@link RebecaParser#arguments}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitArguments(RebecaParser.ArgumentsContext ctx);
	/**
	 * Visit a parse tree produced by {@link RebecaParser#after}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitAfter(RebecaParser.AfterContext ctx);
	/**
	 * Visit a parse tree produced by {@link RebecaParser#deadline}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitDeadline(RebecaParser.DeadlineContext ctx);
	/**
	 * Visit a parse tree produced by {@link RebecaParser#literal}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitLiteral(RebecaParser.LiteralContext ctx);
}