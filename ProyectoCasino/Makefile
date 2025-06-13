CC = gcc
CFLAGS = -Wall -O3
LIBS = -lm -lpthread
SRC_DIR = src
BIN_DIR = bin
RESULTADOS_DIR = resultados

all: preparar $(BIN_DIR)/casino_procesos $(BIN_DIR)/casino_hilos

preparar:
	 mkdir -p $(BIN_DIR) $(RESULTADOS_DIR)

$(BIN_DIR)/casino_procesos: $(SRC_DIR)/casino_procesos.c $(SRC_DIR)/funciones_comunes.h
	 $(CC) $(CFLAGS) $< -o $@ $(LIBS)

$(BIN_DIR)/casino_hilos: $(SRC_DIR)/casino_hilos.c $(SRC_DIR)/funciones_comunes.h
	 $(CC) $(CFLAGS) $< -o $@ $(LIBS)

experimento: all
	 chmod +x scripts/ejecutar_experimento.sh
	 ./scripts/ejecutar_experimento.sh

clean:
	 rm -rf $(BIN_DIR) $(RESULTADOS_DIR)/*.log
	 rm -f $(RESULTADOS_DIR)/tiempos_*.txt
	 rm -f $(RESULTADOS_DIR)/reporte_final.txt

.PHONY: all clean preparar experimento