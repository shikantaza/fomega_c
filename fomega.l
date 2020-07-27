%option noyywrap

%{
#include "util.h"
#include "fomega.h"

#include "fomega.tab.h"

%}

%%

[a-z]+          { yylval.atom_value = clone_string(yytext); return T_VAR; }

[A-Z]+          { yylval.atom_value = clone_string(yytext); return T_TYPEVAR; }

[ ]+            { return T_SPACES; }

\(              { return T_LEFT_PAREN; }

\)              { return T_RIGHT_PAREN; }

\\              { return T_ABS; }

\\\/            { return T_FORALL; }

\/\\            { return T_PABS; }

\:              { return T_COLON; } 

\*              { return T_BASEKIND; }

\->             { return T_OPERATOR; }

\.              { return T_DOT; }

\=              { return T_EQUAL; }

%%

