%{
#include "parser.tab.h"
%}

%option noyywrap

/* Tokens para el menú y comandos */
SALIR       exit|salir
CREAR       crear
CONFIGURAR  configurar
SIMULAR     simular
MOSTRAR     mostrar
CONECTAR    conectar

%%

"1"         { return CREAR; }
"2"         { return CONECTAR; }
"3"         { return CONFIGURAR; }
"4"         { return SIMULAR; }
"5"         { return MOSTRAR; }
"6"         { return SALIR; }
.           { return yytext[0]; }

%%
