#include "funciones_comunes.h"
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

// Estructura compartida con padding
struct ResultadoCompartido {
    struct ResultadoGlobal resultado;
    pthread_mutex_t mutex;
    pthread_mutexattr_t mutex_attr;
    char padding[64];  // Padding para alinear
};

int main() {
    struct timespec inicio_total, fin_total;
    
    // Iniciar medición de tiempo
    clock_gettime(CLOCK_MONOTONIC, &inicio_total);
    
    // Crear memoria compartida
    int fd = shm_open("/casino_shm", O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(struct ResultadoCompartido));
    struct ResultadoCompartido *compartido = mmap(NULL, sizeof(struct ResultadoCompartido), 
                          PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    // Inicializar mutex en memoria compartida
    pthread_mutexattr_init(&compartido->mutex_attr);
    pthread_mutexattr_setpshared(&compartido->mutex_attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&compartido->mutex, &compartido->mutex_attr);
    
    // Inicializar resultados
    compartido->resultado.ganancias_total = 0.0;
    compartido->resultado.transacciones_procesadas = 0;
    
    // Crear procesos hijos para cada grupo
    pid_t pids[NUM_GRUPOS];
    
    for (int i = 0; i < NUM_GRUPOS; i++) {
        pids[i] = fork();
        
        if (pids[i] == 0) {
            // Proceso hijo
            struct timespec inicio_grupo, fin_grupo;
            clock_gettime(CLOCK_MONOTONIC, &inicio_grupo);
            
            double ganancia_grupo = 0.0;
            const char *grupo = GRUPOS[i];
            
            for (int j = 0; j < TRANSACCIONES_POR_GRUPO; j++) {
                procesar_transaccion(grupo, j, &ganancia_grupo);
                
                // Actualizar global con menos frecuencia
                if (j % BLOQUE_ACTUALIZACION == 0) {
                    pthread_mutex_lock(&compartido->mutex);
                    compartido->resultado.ganancias_total += ganancia_grupo;
                    compartido->resultado.transacciones_procesadas += BLOQUE_ACTUALIZACION;
                    ganancia_grupo = 0.0;
                    pthread_mutex_unlock(&compartido->mutex);
                }
            }
            
            // Actualizar el resto
            pthread_mutex_lock(&compartido->mutex);
            compartido->resultado.ganancias_total += ganancia_grupo;
            compartido->resultado.transacciones_procesadas += TRANSACCIONES_POR_GRUPO % BLOQUE_ACTUALIZACION;
            pthread_mutex_unlock(&compartido->mutex);
            
            clock_gettime(CLOCK_MONOTONIC, &fin_grupo);
            registrar_tiempo("resultados/procesos.log", "GRUPO", grupo, &inicio_grupo, &fin_grupo);
            
            exit(0);
        }
    }
    
    // Esperar a que todos los hijos terminen
    for (int i = 0; i < NUM_GRUPOS; i++) {
        waitpid(pids[i], NULL, 0);
    }
    
    // Finalizar medición de tiempo
    clock_gettime(CLOCK_MONOTONIC, &fin_total);
    
    // Resultados finales
    double elapsed_total = (fin_total.tv_sec - inicio_total.tv_sec);
    elapsed_total += (fin_total.tv_nsec - inicio_total.tv_nsec) / 1000000000.0;
    
    FILE *log = fopen("resultados/procesos.log", "a");
    fprintf(log, "\n[RESULTADO FINAL]\n");
    fprintf(log, "Tiempo total: %.6f segundos\n", elapsed_total);
    fprintf(log, "Transacciones procesadas: %ld\n", compartido->resultado.transacciones_procesadas);
    fprintf(log, "Ganancias totales del casino: $%.2f\n", compartido->resultado.ganancias_total);
    fclose(log);
    
    // Limpiar recursos
    pthread_mutex_destroy(&compartido->mutex);
    munmap(compartido, sizeof(struct ResultadoCompartido));
    shm_unlink("/casino_shm");
    
    return 0;
}