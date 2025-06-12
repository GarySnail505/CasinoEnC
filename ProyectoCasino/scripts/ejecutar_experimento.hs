#!/bin/bash

# Configuración
NUM_EJECUCIONES=5
DIR_RESULTADOS="$PWD/resultados"

# Crear archivos de resultados vacíos
> "$DIR_RESULTADOS/tiempos_procesos.txt"
> "$DIR_RESULTADOS/tiempos_hilos.txt"

# Función para extraer tiempo total de un log
extraer_tiempo_total() {
    local archivo=$1
    grep "Tiempo total" "$archivo" | awk '{print $3}'
}

# Ejecutar experimentos
for ((i=1; i<=NUM_EJECUCIONES; i++)); do
    echo "Ejecución $i:"
    
    # Versión con procesos
    echo "  Procesos..."
    /usr/bin/time -f "%e" -o tmp_time.txt ./bin/casino_procesos >/dev/null 2>&1
    tiempo_procesos=$(cat tmp_time.txt)
    echo "$tiempo_procesos" >> "$DIR_RESULTADOS/tiempos_procesos.txt"
    
    # Mover y renombrar log
    mv "$DIR_RESULTADOS/procesos.log" "$DIR_RESULTADOS/procesos_$i.log"
    
    # Versión con hilos
    echo "  Hilos..."
    /usr/bin/time -f "%e" -o tmp_time.txt ./bin/casino_hilos >/dev/null 2>&1
    tiempo_hilos=$(cat tmp_time.txt)
    echo "$tiempo_hilos" >> "$DIR_RESULTADOS/tiempos_hilos.txt"
    
    # Mover y renombrar log
    mv "$DIR_RESULTADOS/hilos.log" "$DIR_RESULTADOS/hilos_$i.log"
    
    echo "  Procesos: ${tiempo_procesos}s | Hilos: ${tiempo_hilos}s"
done

# Calcular promedios
prom_procesos=$(awk '{sum+=$1} END {print sum/NR}' "$DIR_RESULTADOS/tiempos_procesos.txt")
prom_hilos=$(awk '{sum+=$1} END {print sum/NR}' "$DIR_RESULTADOS/tiempos_hilos.txt")

# Generar reporte final
echo "Resultados finales después de $NUM_EJECUCIONES ejecuciones:" > "$DIR_RESULTADOS/reporte_final.txt"
echo "========================================" >> "$DIR_RESULTADOS/reporte_final.txt"
echo "Tiempo promedio procesos: $prom_procesos segundos" >> "$DIR_RESULTADOS/reporte_final.txt"
echo "Tiempo promedio hilos:    $prom_hilos segundos" >> "$DIR_RESULTADOS/reporte_final.txt"
echo "Diferencia:               $(echo "$prom_procesos - $prom_hilos" | bc) segundos" >> "$DIR_RESULTADOS/reporte_final.txt"
echo "Mejora porcentual:        $(echo "scale=2; (($prom_procesos - $prom_hilos) / $prom_procesos) * 100" | bc)%" >> "$DIR_RESULTADOS/reporte_final.txt"

echo "Experimento completado. Resultados en $DIR_RESULTADOS"