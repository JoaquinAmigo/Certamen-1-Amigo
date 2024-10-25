#ifndef SIMULACION_H
#define SIMULACION_H
#define MAX_CONEXIONES 100
#define MAX_REGISTROS 1000
#define MAX_PASOS 1000

typedef struct {
    int S, E, I, R; // Estados del modelo SEIR
} Estado;

typedef struct {
    Estado estado;      // Estado actual de la celda
    int ac_id;           // Identificador del autómata al que pertenece
    int x, y;            // Coordenadas de la celda en su AC
} Celda;

typedef struct {
    int filas, columnas;  // Dimensiones del AC
    Celda **celdas;       // Matriz de celdas
} AutomataCelular;

typedef struct {
    int ac_origen, x_origen, y_origen;
    int ac_destino, x_destino, y_destino;
} Conexion;

typedef struct {
    int ac_origen, x_origen, y_origen;
    int ac_destino, x_destino, y_destino;
    int individuos_transferidos;
} RegistroTransito;

typedef struct {
    int paso;
    int s_to_e;  // De S a E (susceptibles a expuestos)
    int e_to_i;  // De E a I (expuestos a infectados)
    int i_to_r;  // De I a R (infectados a recuperados)
} RegistroCambioEstado;

extern Conexion conexiones[MAX_CONEXIONES];
extern int num_conexiones;
extern int modo_confinamiento;
extern RegistroTransito registros[MAX_REGISTROS];
extern int num_registros;
extern RegistroCambioEstado registros_cambio[MAX_PASOS];
extern int paso_actual;

void crear_ac();
void definir_conexion();
void configurar_seir();
void iniciar_simulacion();
void mostrar_resultados();
void seleccionar_modo();
void mostrar_cambios_estado();

#endif
