#include "funciones_comunes.h"

// Argumentos para el hilo con padding
struct ThreadArgs {
    const char *grupo;
    struct ResultadoGlobal *resultado;
    pthread_mutex_t *mutex;
    char padding[64];  // Padding para evitar false sharing
};

// Función optimizada para el hilo
void *procesar_grupo(void *arg) {
    struct ThreadArgs *args = (struct ThreadArgs *)arg;
    struct timespec inicio, fin;
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    
    double ganancia_grupo = 0.0;
    
    for (int i = 0; i < TRANSACCIONES_POR_GRUPO; i++) {
        procesar_transaccion(args->grupo, i, &ganancia_grupo);
        
        // Actualizar global con menos frecuencia
        if (i % BLOQUE_ACTUALIZACION == 0) {
            pthread_mutex_lock(args->mutex);
            args->resultado->ganancias_total += ganancia_grupo;
            args->resultado->transacciones_procesadas += BLOQUE_ACTUALIZACION;
            ganancia_grupo = 0.0;
            pthread_mutex_unlock(args->mutex);
        }
    }
    
    // Actualizar el resto
    pthread_mutex_lock(args->mutex);
    args->resultado->ganancias_total += ganancia_grupo;
    args->resultado->transacciones_procesadas += TRANSACCIONES_POR_GRUPO % BLOQUE_ACTUALIZACION;
    pthread_mutex_unlock(args->mutex);
    
    clock_gettime(CLOCK_MONOTONIC, &fin);
    registrar_tiempo("resultados/hilos.log", "GRUPO", args->grupo, &inicio, &fin);
    
    pthread_exit(NULL);
}

int main() {
    struct timespec inicio_total, fin_total;
    
    // Resultados globales
    struct ResultadoGlobal resultado;
    resultado.ganancias_total = 0.0;
    resultado.transacciones_procesadas = 0;
    
    // Mutex para sincronización
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    
    // Crear hilos
    pthread_t hilos[NUM_GRUPOS];
    struct ThreadArgs args[NUM_GRUPOS];
    
    // Inicializar argumentos
    for (int i = 0; i < NUM_GRUPOS; i++) {
        args[i].grupo = GRUPOS[i];
        args[i].resultado = &resultado;
        args[i].mutex = &mutex;
    }
    
    // Iniciar medición de tiempo DESPUÉS de inicialización
    clock_gettime(CLOCK_MONOTONIC, &inicio_total);
    
    // Crear hilos
    for (int i = 0; i < NUM_GRUPOS; i++) {
        if (pthread_create(&hilos[i], NULL, procesar_grupo, &args[i])) {
            perror("Error al crear hilo");
            return 1;
        }
    }
    
    // Esperar a que todos los hilos terminen
    for (int i = 0; i < NUM_GRUPOS; i++) {
        pthread_join(hilos[i], NULL);
    }
    
    // Detener medición de tiempo
    clock_gettime(CLOCK_MONOTONIC, &fin_total);
    
    // Resultados finales
    double elapsed_total = (fin_total.tv_sec - inicio_total.tv_sec);
    elapsed_total += (fin_total.tv_nsec - inicio_total.tv_nsec) / 1000000000.0;
    
    FILE *log = fopen("resultados/hilos.log", "a");
    fprintf(log, "\n[RESULTADO FINAL]\n");
    fprintf(log, "Tiempo total: %.6f segundos\n", elapsed_total);
    fprintf(log, "Transacciones procesadas: %ld\n", resultado.transacciones_procesadas);
    fprintf(log, "Ganancias totales del casino: $%.2f\n", resultado.ganancias_total);
    fclose(log);
    
    // Limpiar recursos
    pthread_mutex_destroy(&mutex);
    
    return 0;
}