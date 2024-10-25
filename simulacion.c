#include <stdio.h>
#include <stdlib.h>
#include "simulacion.h"
#include <time.h>
#define MAX_AUTOMATAS 10

AutomataCelular acs[MAX_AUTOMATAS];
int num_acs = 0;
double beta, sigma, gamma_rate;
Conexion conexiones[MAX_CONEXIONES];  // Definición real
int num_conexiones = 0;
int modo_confinamiento = 1;
RegistroTransito registros[MAX_REGISTROS];
int num_registros = 0;
RegistroCambioEstado registros_cambio[MAX_PASOS];
int paso_actual = 0;

// Función para seleccionar el modo de simulación al inicio
void seleccionar_modo() {
    int opcion;
    printf("Seleccione el modo de simulación:\n");
    printf("1. Confinamiento (sin tránsito entre celdas)\n");
    printf("2. Tránsito entre celdas conectadas\n");
    printf("Opción: ");
    scanf("%d", &opcion);

    if (opcion == 1) {
        modo_confinamiento = 1;
        printf("Modo: Confinamiento seleccionado.\n");
    } else if (opcion == 2) {
        modo_confinamiento = 0;
        printf("Modo: Tránsito entre celdas seleccionado.\n");
    } else {
        printf("Opción inválida. Por defecto: Confinamiento.\n");
        modo_confinamiento = 1;
    }
}
void transferir_individuos(Celda *origen, Celda *destino) {
    // Transferir 10% de los susceptibles del origen al destino
    int individuos_transferidos = origen->estado.S / 10;

    // Registrar el tránsito
    registros[num_registros++] = (RegistroTransito){
        origen->ac_id, origen->x, origen->y,
        destino->ac_id, destino->x, destino->y,
        individuos_transferidos
    };

    // Actualizar los estados de las celdas
    destino->estado.S += individuos_transferidos;
    origen->estado.S -= individuos_transferidos;
}
void mostrar_transito() {
    printf("Tránsito entre celdas en este paso:\n");

    if (num_registros == 0) {
        printf("No hubo tránsito de individuos en este paso.\n");
    } else {
        for (int i = 0; i < num_registros; i++) {
            RegistroTransito r = registros[i];
            printf("AC%d(%d, %d) -> AC%d(%d, %d): %d individuos transferidos\n",
                r.ac_origen, r.x_origen, r.y_origen,
                r.ac_destino, r.x_destino, r.y_destino,
                r.individuos_transferidos);
        }
    }

    // Reiniciar los registros para el siguiente paso
    num_registros = 0;
}
void inicializar_aleatoriamente() {
    srand(time(NULL));  // Inicializa la semilla aleatoria

    for (int k = 0; k < num_acs; k++) {
        int x = rand() % acs[k].filas;
        int y = rand() % acs[k].columnas;
        acs[k].celdas[x][y].estado.I = 5;  // Asignar 5 infectados aleatorios
        acs[k].celdas[x][y].estado.S -= 5; // Restar 5 de los susceptibles
        printf("Celda (%d, %d) en AC %d inicia con 5 infectados.\n", x, y, k + 1);
    }
}
// Crear un nuevo AC
void crear_ac() {
    if (num_acs >= MAX_AUTOMATAS) {
        printf("No se pueden crear más autómatas.\n");
        return;
    }

    int filas, columnas;
    printf("Ingrese el número de filas y columnas para el AC %d:\n", num_acs);
    scanf("%d %d", &filas, &columnas);

    AutomataCelular *ac = &acs[num_acs++];
    ac->filas = filas;
    ac->columnas = columnas;

    // Inicializar celdas con estados aleatorios
    ac->celdas = malloc(filas * sizeof(Celda *));
    for (int i = 0; i < filas; i++) {
        ac->celdas[i] = malloc(columnas * sizeof(Celda));
        for (int j = 0; j < columnas; j++) {
            int infectados_iniciales = rand() % 10;  // Aleatorio entre 0 y 9
            // Asignar el estado inicial SEIR a cada celda
            ac->celdas[i][j] = (Celda){{100 - infectados_iniciales, 0, infectados_iniciales, 0}, num_acs - 1, i, j};
            printf("Celda (%d, %d) en AC %d inicia con %d infectados.\n",
                i, j, ac->celdas[i][j].ac_id, infectados_iniciales);
        }
    }

    printf("Autómata %d creado con dimensiones %dx%d.\n", ac->celdas[0][0].ac_id, filas, columnas);
}
// Definir conexiones entre celdas de diferentes ACs
void definir_conexion() {
    if (num_conexiones >= MAX_CONEXIONES) {
        printf("Error: Se alcanzó el número máximo de conexiones.\n");
        return;
    }

    printf("Ingrese el AC de origen, las coordenadas (x, y) del origen,\n");
    printf("el AC de destino y las coordenadas (x, y) del destino:\n");

    int ac_origen, x_origen, y_origen;
    int ac_destino, x_destino, y_destino;

    scanf("%d %d %d %d %d %d",
        &ac_origen, &x_origen, &y_origen,
        &ac_destino, &x_destino, &y_destino);

    if (ac_origen >= num_acs || ac_destino >= num_acs ||
        x_origen >= acs[ac_origen].filas || y_origen >= acs[ac_origen].columnas ||
        x_destino >= acs[ac_destino].filas || y_destino >= acs[ac_destino].columnas) {
        printf("Error: Coordenadas o IDs inválidos.\n");
        return;
    }

    conexiones[num_conexiones++] = (Conexion){
        ac_origen, x_origen, y_origen,
        ac_destino, x_destino, y_destino
    };

    printf("Conexión creada: AC%d(%d, %d) <-> AC%d(%d, %d)\n", 
        ac_origen, x_origen, y_origen, ac_destino, x_destino, y_destino);
}
// Configurar los parámetros beta, sigma y gama para el modelo SEIR
void configurar_seir() {
    printf("Ingrese la tasa de infección (beta): ");
    scanf("%lf", &beta);
    printf("Ingrese la tasa de progresión (sigma): ");
    scanf("%lf", &sigma);
    printf("Ingrese la tasa de recuperación (gamma): ");
    scanf("%lf", &gamma_rate);
}
// Verifica si las coordenadas (x, y) están dentro de los límites del AC
int dentro_de_limites(int filas, int columnas, int x, int y) {
    return x >= 0 && x < filas && y >= 0 && y < columnas;
}
//Actualiza el estado de cada celda
void actualizar_estado(Celda *celda, Estado *nuevo_estado, AutomataCelular *ac) {
    int infecciones_vecinas = 0;

    // Revisar los vecinos en la vecindad de Moore
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue;

            int vecino_x = celda->x + dx;
            int vecino_y = celda->y + dy;

            if (dentro_de_limites(ac->filas, ac->columnas, vecino_x, vecino_y)) {
                infecciones_vecinas += ac->celdas[vecino_x][vecino_y].estado.I;
            }
        }
    }

    // Verificar las conexiones adicionales entre ACs
    if (!modo_confinamiento) {  // Solo ejecutar si NO es confinamiento
        for (int i = 0; i < num_conexiones; i++) {
            Conexion conn = conexiones[i];

            // Transferencia del origen al destino
            if (conn.ac_origen == celda->ac_id &&
                conn.x_origen == celda->x &&
                conn.y_origen == celda->y) {

                transferir_individuos(
                    &acs[conn.ac_origen].celdas[conn.x_origen][conn.y_origen],
                    &acs[conn.ac_destino].celdas[conn.x_destino][conn.y_destino]
                );
            }

            // Transferencia del destino al origen
            if (conn.ac_destino == celda->ac_id &&
                conn.x_destino == celda->x &&
                conn.y_destino == celda->y) {

                transferir_individuos(
                    &acs[conn.ac_destino].celdas[conn.x_destino][conn.y_destino],
                    &acs[conn.ac_origen].celdas[conn.x_origen][conn.y_origen]
                );
            }
        }
    }

    // Calcular los cambios de estado SEIR
    int nuevos_expuestos = beta * celda->estado.S * infecciones_vecinas / 100.0;
    int nuevos_infectados = sigma * celda->estado.E;
    int nuevos_recuperados = gamma_rate * celda->estado.I;

    // Registrar los cambios de estado SEIR
    registros_cambio[paso_actual].s_to_e += nuevos_expuestos;
    registros_cambio[paso_actual].e_to_i += nuevos_infectados;
    registros_cambio[paso_actual].i_to_r += nuevos_recuperados;

    // Aplicar los cambios a los estados
    nuevo_estado->S = celda->estado.S - nuevos_expuestos;
    nuevo_estado->E = celda->estado.E + nuevos_expuestos - nuevos_infectados;
    nuevo_estado->I = celda->estado.I + nuevos_infectados - nuevos_recuperados;
    nuevo_estado->R = celda->estado.R + nuevos_recuperados;

    // Asegurarse de que los valores no sean negativos
    if (nuevo_estado->S < 0) nuevo_estado->S = 0;
    if (nuevo_estado->E < 0) nuevo_estado->E = 0;
    if (nuevo_estado->I < 0) nuevo_estado->I = 0;
    if (nuevo_estado->R < 0) nuevo_estado->R = 0;
}
// Ejecuta la simulación
void iniciar_simulacion() {
    int pasos;
    printf("Ingrese el número de pasos de la simulación: ");
    scanf("%d", &pasos);

    for (paso_actual = 0; paso_actual < pasos; paso_actual++) {
        printf("\nPaso %d de la simulación...\n", paso_actual + 1);

        for (int k = 0; k < num_acs; k++) {
            Estado **nuevos_estados = malloc(acs[k].filas * sizeof(Estado *));
            for (int i = 0; i < acs[k].filas; i++) {
                nuevos_estados[i] = malloc(acs[k].columnas * sizeof(Estado));
            }

            // Actualizar el estado de cada celda
            for (int i = 0; i < acs[k].filas; i++) {
                for (int j = 0; j < acs[k].columnas; j++) {
                    actualizar_estado(&acs[k].celdas[i][j], &nuevos_estados[i][j], &acs[k]);
                }
            }

            // Aplicar los nuevos estados
            for (int i = 0; i < acs[k].filas; i++) {
                for (int j = 0; j < acs[k].columnas; j++) {
                    acs[k].celdas[i][j].estado = nuevos_estados[i][j];
                }
                free(nuevos_estados[i]);
            }
            free(nuevos_estados);
        }
        // Mostrar los cambios de estado en este paso
        mostrar_cambios_estado();
        // Mostrar el tránsito entre celdas en este paso
        mostrar_transito();
    }
    printf("\nSimulación completada.\n");
}
// Muestra los resultados finales de la simulacion
void mostrar_resultados() {
    for (int k = 0; k < num_acs; k++) {
        printf("\nResultados del AC %d:\n", k);
        for (int i = 0; i < acs[k].filas; i++) {
            for (int j = 0; j < acs[k].columnas; j++) {
                Celda *c = &acs[k].celdas[i][j];
                printf("Celda (%d, %d) -> S: %d, E: %d, I: %d, R: %d\n",
                    i, j, c->estado.S, c->estado.E, c->estado.I, c->estado.R);
            }
        }
    }

    // Mostrar las conexiones entre ACs
    printf("\nConexiones entre ACs:\n");
    if (num_conexiones == 0) {
        printf("No hay conexiones definidas.\n");
    } else {
        for (int i = 0; i < num_conexiones; i++) {
            Conexion conn = conexiones[i];
            printf("AC%d(%d, %d) <-> AC%d(%d, %d)\n",
                conn.ac_origen, conn.x_origen, conn.y_origen,
                conn.ac_destino, conn.x_destino, conn.y_destino);
        }
    }
}
// Muestra los cambios de estado los individuos en cada paso de la simulacion
void mostrar_cambios_estado() {
    printf("\nCambios de estado en el paso %d:\n", paso_actual + 1);
    printf("De S a E: %d individuos\n", registros_cambio[paso_actual].s_to_e);
    printf("De E a I: %d individuos\n", registros_cambio[paso_actual].e_to_i);
    printf("De I a R: %d individuos\n", registros_cambio[paso_actual].i_to_r);
}