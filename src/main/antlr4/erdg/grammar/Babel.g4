grammar Babel;

model       : classDef* mainBlock? EOF ;

mainBlock   : 'main' '{' rebecInstance* '}' ;

rebecInstance
            : identifier 'rebec' ':' '(' identifier ')' priorityBlock? ';' ;

priorityBlock
            : 'priority' NUMBER ;

classDef    : 'rebecclass' identifier '{' vars? method+ '}' ;

vars        : 'statevars' varDecl* ;

varDecl     : TYPE_IDENT identifier ';' ;

method      : 'method' LOWER_IDENT priorityBlock? '{' stmt* '}' 'end' ;

stmt        : assignStmt ';'              # AssignStatement
            | sendStmt ';'                # SendStatement
            | 'skip' ';'                  # SkipStatement
            | '++' ';'                    # IncStatement
            | ifStmt                      # IfStatement
            ;

assignStmt  : identifier '=' expr ;

sendStmt    : identifier '!' LOWER_IDENT ;

ifStmt      : 'if' '(' expr ')' '{' stmt* '}' 'else' '{' stmt* '}' ;

expr        : '!' expr                                    # NotExpr
            | expr ('*' | '/') expr                       # MulDivExpr
            | expr ('+' | '-') expr                       # AddSubExpr
            | expr ('<' | '<=' | '>' | '>=') expr         # CompareExpr
            | expr ('==' | '!=') expr                     # EqExpr
            | expr '&&' expr                              # AndExpr
            | expr '||' expr                              # OrExpr
            | '(' expr ')'                                # GroupExpr
            | identifier                                  # NameExpr
            | NUMBER                                      # NumberExpr
            | ESCAPED_STRING                              # StringExpr
            | 'true'                                      # TrueExpr
            | 'false'                                     # FalseExpr
            ;

identifier  : TYPE_IDENT | LOWER_IDENT ;

TYPE_IDENT      : [A-Z] [a-zA-Z0-9_]* ;
LOWER_IDENT     : [a-z_] [a-zA-Z0-9_]* ;

NUMBER          : [0-9]+ ('.' [0-9]+)? ;
ESCAPED_STRING  : '"' (~["\\] | '\\' .)* '"' ;

WS              : [ \t\r\n]+ -> skip ;