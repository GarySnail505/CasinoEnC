# Sistemas Operativos: Simulación de Casino Online
### Integrantes:
- Anahí Achote
- Anthony Alangasí
- Gary Defas
- Lindsay Guzmán

## Descripción del Proyecto

Este proyecto busca resolver un problema de optimización de transacciones dentro de un casino online. Es por ello que se va a implementar un sistema de simulación para comparar el rendimiento entre dos enfoques de programación paralela:

- **Versión con procesos:** Usa `fork()` para crear múltiples procesos.
- **Versión con hilos:** Usa `pthreads` para crear múltiples hilos.

El sistema simula 800,000 transacciones de apuestas distribuidas en 8 grupos de juegos diferentes (Blackjack, Ruleta, Poker, etc.), con 100,000 transacciones por grupo. El objetivo principal es demostrar empíricamente las diferencias de rendimiento entre ambos enfoques.

## Características Clave

- Simulación realista de transacciones de casino
- Mecanismo de apuestas con ventaja de la casa del 21.25%
- Sistema de medición precisa de tiempos con `clock_gettime()`
- Sincronización con `mutex` para acceso seguro a recursos compartidos
- Generación automática de reportes comparativos

## Requisitos Previos

- Sistema operativo basado en Linux (Ubuntu Server de recomendación)
- Compilador GCC instalado
- Biblioteca de hilos POSIX (normalmente incluida en Linux)
- Bash shell

## Instrucciones de Compilación

### 1. Clonar el repositorio

```bash
git clone https://github.com/GarySnail505/CasinoEnC.git
cd proyecto_casino
```

### 2. Compilar el proyecto

```bash
make
```

Este comando:

- Creará los directorios necesarios (`bin/` y `resultados/`)
- Compilará ambos programas:
  - `bin/casino_procesos` (versión con procesos)
  - `bin/casino_hilos` (versión con hilos)

## Ejecución del Experimento

### Ejecutar el experimento completo

```bash
./scripts/ejecutar_experimento.sh
```

Este comando:

- Ejecutará 5 iteraciones de ambas versiones
- Medirá y recolectará los tiempos de ejecución
- Generará reportes estadísticos en el directorio `resultados/`

## Visualización de Resultados

### Resultados directos

Después de cada ejecución individual, los resultados se guardan en:

- `resultados/procesos.log`: Resultados detallados de la versión con procesos
- `resultados/hilos.log`: Resultados detallados de la versión con hilos

### Reporte final del experimento

El experimento completo genera:

```bash
cat resultados/reporte_final.txt
```

#### Ejemplo de salida:

```
Resultados finales después de 5 ejecuciones:
========================================
Tiempo promedio procesos: 8.92 segundos
Tiempo promedio hilos:    3.15 segundos
Diferencia:               5.77 segundos
Mejora porcentual:        35.31%
```

### Resultados detallados por ejecución

- `resultados/tiempos_procesos.txt`: Tiempos de cada ejecución (procesos)
- `resultados/tiempos_hilos.txt`: Tiempos de cada ejecución (hilos)
- `resultados/procesos_1.log`: Log detallado de la primera ejecución (procesos)
- `resultados/hilos_1.log`: Log detallado de la primera ejecución (hilos)

## Estructura del Proyecto

```text
proyecto_casino/
├── bin/                    # Ejecutables generados (creado al compilar)
├── src/                    # Código fuente
│   ├── casino_procesos.c   # Versión con procesos
│   ├── casino_hilos.c      # Versión con hilos
│   └── funciones_comunes.h # Funciones compartidas
├── scripts/                # Scripts de automatización
│   └── ejecutar_experimento.sh
├── resultados/             # Resultados generados (creado al compilar)
├── Makefile                # Sistema de construcción
└── README.md               # Este archivo
```

## Interpretación de Resultados

Los resultados mostrarán típicamente:

- Tiempos menores para la versión con hilos.
- Mejora porcentual significativa.
- Consistencia en las diferencias a través de múltiples ejecuciones.

Esto demuestra que para cargas de trabajo con alta necesidad de compartir estado (como un casino online), el enfoque con hilos es más eficiente debido a:

- Menor costo de creación y cambio de contexto
- Acceso más eficiente a memoria compartida
- Menor overhead de comunicación

## Limpieza

Para eliminar todos los archivos generados:

```bash
make clean
```
Así, podrás ejecutar nuevamente el código mediante lo dicho anteriormente.


