%{
#include <stdio.h>
#include <stdlib.h>
#include "simulacion.h"
#include <time.h>

int yylex();
void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}
%}

%token CREAR CONFIGURAR SIMULAR MOSTRAR CONECTAR SALIR

%%
menu:
    | opcion menu
    ;

opcion:
    CREAR { crear_ac(); }
| CONECTAR { definir_conexion(); }
| CONFIGURAR { configurar_seir(); }
| SIMULAR { iniciar_simulacion(); }
| MOSTRAR { mostrar_resultados(); }
| SALIR { exit(0); }
;
%%


int main() {
    srand(time(NULL));
    printf("\n--- Bienvenido a la simulación SEIR ---\n");
    seleccionar_modo();  // Seleccionar el modo antes de iniciar
    printf("Menú:\n1. Crear AC\n2. Definir conexiones de los AC\n3. Configurar SEIR\n4. Simular\n5. Mostrar resultados\n6. Salir\n");
    yyparse();
    return 0;
}
