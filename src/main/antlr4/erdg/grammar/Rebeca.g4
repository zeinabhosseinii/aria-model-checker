grammar Rebeca;

// ===== Parser Rules =====

model
    : envDeclaration* reactiveClassDeclaration* mainDeclaration EOF
    ;

envDeclaration
    : 'env' type IDENTIFIER '=' literal ';'
    ;

mainDeclaration
    : 'main' '{' mainRebecDefinition* '}'
    ;

mainRebecDefinition
    : type IDENTIFIER '(' bindingList? ')' ':' '(' argList? ')' ';'
    ;

bindingList
    : IDENTIFIER (',' IDENTIFIER)*
    ;

argList
    : argItem (',' argItem)*
    ;

argItem
    : '(' type ')' expression
    | expression
    ;

reactiveClassDeclaration
    : 'abstract'? 'reactiveclass' IDENTIFIER ('extends' IDENTIFIER)? '(' DECIMAL_LITERAL ')'
      '{'
      knownRebecsDeclaration?
      stateVarsDeclaration?
      constructorDeclaration?
      msgsrvDeclaration*
      '}'
    ;

knownRebecsDeclaration
    : 'knownrebecs' '{' fieldDeclaration* '}'
    ;

stateVarsDeclaration
    : 'statevars' '{' fieldDeclaration* '}'
    ;

fieldDeclaration
    : type IDENTIFIER ';'
    ;

constructorDeclaration
    : IDENTIFIER '(' formalParams? ')' block
    ;

msgsrvDeclaration
    : 'abstract'? 'msgsrv' IDENTIFIER '(' formalParams? ')' (block | ';')
    ;

formalParams
    : formalParam (',' formalParam)*
    ;

formalParam
    : type IDENTIFIER
    ;

type
    : IDENTIFIER ('[' DECIMAL_LITERAL? ']')*
    ;

block
    : '{' statement* '}'
    ;

statement
    : expression ';'                                          # ExprStatement
    | type IDENTIFIER ('=' expression)? ';'                   # VarDeclStatement
    | 'if' '(' expression ')' statement ('else' statement)?   # IfStatement
    | 'while' '(' expression ')' statement                    # WhileStatement
    | 'for' '(' forInit? ';' expression? ';' expressionList? ')' statement  # ForStatement
    | 'return' expression? ';'                                # ReturnStatement
    | 'break' ';'                                             # BreakStatement
    | 'continue' ';'                                          # ContinueStatement
    | 'assertion' '(' expression ')' ';'                      # AssertStatement
    | block                                                   # BlockStatement
    | ';'                                                     # EmptyStatement
    ;

forInit
    : type IDENTIFIER '=' expression
    | expression
    ;

expressionList
    : expression (',' expression)*
    ;

expression
    : '(' type ')' expression                                 # CastExpr
    | '(' expression ')'                                      # ParenExpr
    | expression '.' IDENTIFIER arguments? after? deadline?   # MemberCallExpr
    | expression '[' expression ']'                           # ArrayExpr
    | expression 'instanceof' IDENTIFIER                      # InstanceofExpr
    | expression op=('*'|'/'|'%') expression                  # MulExpr
    | expression op=('+'|'-') expression                      # AddExpr
    | expression op=('<'|'<='|'>'|'>=') expression            # CompareExpr
    | expression op=('=='|'!=') expression                    # EqExpr
    | expression '&&' expression                              # AndExpr
    | expression '||' expression                              # OrExpr
    | <assoc=right> expression '?' expression ':' expression   # TernaryExpr
    | <assoc=right> expression op=('='|'+='|'-='|'*='|'/=') expression  # AssignExpr
    | op=('!'|'-'|'+'|'++'|'--') expression                   # UnaryExpr
    | expression op=('++'|'--')                               # PostfixExpr
    | '?' '(' expression (',' expression)+ ')'                # NondetExpr
    | IDENTIFIER arguments?                                   # CallOrVarExpr
    | 'self'                                                  # SelfExpr
    | 'sender'                                                # SenderExpr
    | 'null'                                                  # NullExpr
    | 'new' IDENTIFIER '(' expressionList? ')' ':' '(' expressionList? ')'  # NewExpr
    | literal                                                 # LiteralExpr
    ;

arguments
    : '(' (expression (',' expression)*)? ')'
    ;

after
    : 'after' '(' expression ')'
    ;

deadline
    : 'deadline' '(' expression ')'
    ;

literal
    : DECIMAL_LITERAL
    | FLOAT_LITERAL
    | BOOL_LITERAL
    | STRING_LITERAL
    ;

// ===== Lexer Rules =====

DECIMAL_LITERAL : ('0' | [1-9][0-9]*) [lL]? ;
FLOAT_LITERAL   : [0-9]+ '.' [0-9]* | '.' [0-9]+ ;
BOOL_LITERAL    : 'true' | 'false' ;
STRING_LITERAL  : '"' (~["\\\r\n] | '\\' .)* '"' ;

IDENTIFIER      : [a-zA-Z_$] [a-zA-Z0-9_$]* ;

WS              : [ \t\r\n]+ -> skip ;
COMMENT         : '/*' .*? '*/' -> skip ;
LINE_COMMENT    : '//' ~[\r\n]* -> skip ;