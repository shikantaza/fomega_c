/**
  Copyright 2020 Rajesh Jayaprakash <rajesh.jayaprakash@gmail.com>

  This file is part of fomega_c.

  fomega_c is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  fomega_c is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with fomega_c.  If not, see <http://www.gnu.org/licenses/>.
**/

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

