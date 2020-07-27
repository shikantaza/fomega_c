#!/bin/sh
bison -d fomega.y
flex fomega.l
gcc util.c env.c fomega.c fomega.tab.c lex.yy.c -lgc -o fomega
