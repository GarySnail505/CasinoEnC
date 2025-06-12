#ifndef FUNCIONES_COMUNES_H
#define FUNCIONES_COMUNES_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_GRUPOS 8
#define TRANSACCIONES_POR_GRUPO 100000
#define MAX_APUESTA 1000
#define BLOQUE_ACTUALIZACION 10000  // Aumentar el tamaño del bloque

// Grupos de juegos del casino
static const char *GRUPOS[] = {
    "Blackjack",
    "Ruleta",
    "Poker",
    "Tragamonedas",
    "Baccarat",
    "Dados",
    "Póker Video",
    "Ruleta Vivo"
};

// Estructura para resultados con padding para evitar false sharing
struct ResultadoGlobal {
    double ganancias_total;
    long transacciones_procesadas;
    char padding[64];  // Padding para alinear a 64 bytes
};

// Función optimizada para simular transacciones
void procesar_transaccion(const char *grupo, int transaccion_id, double *ganancia_local) {
    // Usar variable local para acumular resultados
    double resultado_local = 0.0;
    
    // Simular trabajo: cálculo optimizado
    for (int i = 0; i < 50; i++) {  // Reducir iteraciones
        double val = rand() / (double)RAND_MAX;
        resultado_local += sqrt(1.0 - val * val);  // Cálculo más eficiente
    }
    
    // Simular apuesta (1-1000 dólares)
    double apuesta = (rand() % MAX_APUESTA) + 1;
    
    // Determinar resultado
    double ganancia = (rand() % 100) < 45 ? apuesta * 0.75 : -apuesta;
    
    // Actualizar ganancia local
    *ganancia_local += ganancia;
    
    // Simular tiempo de procesamiento (reducido)
    usleep(rand() % 30);  // Retardo aleatorio de 0-30 microsegundos
}

// Función para registrar tiempos
void registrar_tiempo(const char *archivo, const char *fase, const char *grupo, 
                      struct timespec *start, struct timespec *end) {
    double elapsed = (end->tv_sec - start->tv_sec);
    elapsed += (end->tv_nsec - start->tv_nsec) / 1000000000.0;
    
    FILE *log = fopen(archivo, "a");
    if (log) {
        fprintf(log, "[%s] Grupo %s: %.6f segundos\n", fase, grupo, elapsed);
        fclose(log);
    }
}

#endif