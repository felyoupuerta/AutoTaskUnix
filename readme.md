# AutoTaskUnix / TaskMon

Proyecto en C para practicar la creación de un **demonio de tareas** y un **cliente de control**, usando sockets UNIX (`AF_UNIX`), hilos POSIX (`pthread`), procesos (`fork`/`exec`) y un protocolo de mensajes propio.

Un programa servidor (**demonio**, `taskd`) corre en segundo plano, guarda tareas programadas y las ejecuta según un intervalo. Un programa cliente (**taskctl**, `taskctl`) se conecta al demonio por socket para listar, añadir, ejecutar y borrar tareas.

---

## Tabla de contenidos

1. [Qué hace el proyecto](#qué-hace-el-proyecto)
2. [Arquitectura](#arquitectura)
3. [Estructura de carpetas](#estructura-de-carpetas)
4. [Compilar](#compilar)
5. [Ejecutar el demonio](#ejecutar-el-demonio)
6. [Comandos del cliente](#comandos-del-cliente)
7. [Persistencia y logging](#persistencia-y-logging)
8. [Protocolo cliente-demonio](#protocolo-cliente-demonio)
9. [Programa auxiliar: compfechas](#programa-auxiliar-compfechas)
10. [Limitaciones actuales](#limitaciones-actuales)
11. [Problemas comunes](#problemas-comunes)

---

## Qué hace el proyecto

El proyecto compila tres binarios:

| Programa | Ruta | Función |
| --- | --- | --- |
| Demonio | `bin/taskd` | Corre en segundo plano, escucha por socket UNIX y ejecuta tareas programadas. |
| Cliente | `bin/taskctl` | CLI para listar, añadir, ejecutar y borrar tareas. |
| Comparador de horas | `bin/comp` | Programa standalone de práctica, sin relación con el daemon (ver [más abajo](#programa-auxiliar-compfechas)). |

Cada tarea tiene un comando de shell y un intervalo en segundos. El scheduler del demonio revisa la lista cada segundo y, cuando corresponde, **ejecuta el comando de verdad** con `fork()` + `execl("/bin/sh", "sh", "-c", cmd, NULL)`.

---

## Arquitectura

```text
Cliente taskctl
     |
     | Request por socket UNIX
     v
/tmp/taskDemonio.sock
     |
     v
Demonio taskd
     |
     +-- Hilo servidor: acepta clientes, procesa comandos, responde
     |
     +-- Hilo scheduler: revisa la lista de tareas cada segundo y las lanza con fork/exec
```

Ambos hilos comparten un array fijo de tareas en memoria, protegido con `pthread_mutex_t`.

---

## Estructura de carpetas

```text
.
├── makefile
├── tasks.conf              # persistencia de tareas (intervalo:comando)
├── logs/
│   └── serv_log.log        # log de cada request procesada por el demonio
└── src/
    ├── client/              # taskctl: main.c, client.c/h
    ├── common/               # config.h, protocol.h (compartidos)
    ├── daemon/               # taskd: main.c, server.c/h, scheduler.c/h, guarda_server_log.c/h
    └── compfechas/           # programa auxiliar independiente
```

La compilación genera además `obj/` (objetos) y `bin/` (ejecutables).

---

## Compilar

```bash
make            # compila taskd y taskctl
make clean      # borra obj/ y bin/
make clean && make
```

También: `make daemon` o `make client` para compilar uno solo.

---

## Ejecutar el demonio

```bash
./bin/taskd
```

Queda corriendo en primer plano, crea el socket `/tmp/taskDemonio.sock` y carga tareas guardadas desde `tasks.conf` si existe. `Ctrl+C` para detenerlo, o `SIGHUP` para recargar `tasks.conf` sin reiniciar.

---

## Comandos del cliente

Cada comando tiene un atajo corto:

| Comando | Atajo | Uso |
| --- | --- | --- |
| `list` | `ls` | Lista las tareas registradas |
| `add` | `a` | Pide por teclado el comando y el intervalo, y crea la tarea |
| `run <id>` | `x <id>` | Ejecuta una tarea ya y transmite su salida en vivo |
| `delete <id>` | `rm <id>` | Elimina una tarea por su ID |
| `help` / sin argumentos | — | Muestra la ayuda |

Ejemplos:

```bash
./bin/taskctl ls
./bin/taskctl a
./bin/taskctl x 1
./bin/taskctl rm 1
```

Al usar `add`, si el intervalo ingresado es `0`, se usa `5` segundos por defecto. Al usar `run`, la salida del comando se envía línea por línea al cliente en tiempo real (vía `popen`), terminando con el código de salida del proceso.

> El demonio debe estar corriendo antes de usar el cliente.

---

## Persistencia y logging

- **`tasks.conf`**: cada línea tiene el formato `intervalo:comando`. El demonio lo lee al arrancar (`scheduler_init`) y lo reescribe cada vez que se añade o borra una tarea (`guardar_tareas_en_archivo`), así que las tareas sobreviven a un reinicio del demonio.
- **`logs/serv_log.log`**: cada request procesada por el servidor queda registrada con fecha, hora, file descriptor del cliente, status y salida.

---

## Protocolo cliente-demonio

Definido en `src/common/protocol.h`:

```c
typedef enum
{
    CMD_LIST,
    CMD_ADD,
    CMD_RUN,
    CMD_DELETE
} CmdType;

typedef struct
{
    int task_id;
    CmdType comando;
    char cmd[M_BUFF_CMD];
    int s_intervalo;
} Request;

typedef struct
{
    int status;
    char response[M_BUFF_S_RESPONSE];
} Response;
```

El cliente manda un `Request` con `write()`; el demonio responde con uno o varios `Response` (el caso de `CMD_RUN` puede mandar varios, uno por línea de salida del comando).

La ruta del socket está fija en `src/common/config.h`:

```c
#define SOCK_F "/tmp/taskDemonio.sock"
```

---

## Programa auxiliar: compfechas

Programa de consola **independiente**, sin relación funcional (todavía) con `taskd`/`taskctl`. Pide una hora, minuto y segundo por teclado, valida el rango de cada valor, y luego espera en un bucle comparando cada segundo la hora del sistema hasta que coincide exactamente con la indicada. Es un ejercicio de práctica con `time.h` / `localtime`, pensado como base para una futura función de "ejecutar tarea a una hora exacta" en el scheduler.

---

## Limitaciones actuales

1. El array de tareas en memoria es de tamaño fijo (`MAX_CL`) — no crece dinámicamente.
2. `compfechas` todavía no está integrado al scheduler del demonio.
3. No hay confirmación al borrar una tarea (`rm`) desde el cliente — cualquier ID válido se elimina sin preguntar.
4. No hay validación de que el ID pasado por argumento sea realmente numérico antes de enviarlo al demonio.

---

## Problemas comunes

**`[ERROR] ERROR AL CONECTAR AL SOCKET`** → el demonio no está corriendo. Ejecutá `./bin/taskd` primero.

**Socket viejo en `/tmp`** → el demonio ya intenta borrarlo solo con `unlink()` antes de hacer `bind()`. Si persiste el problema:

```bash
rm -f /tmp/taskDemonio.sock
./bin/taskd
```

**Warnings al compilar** → mientras `make` termine sin errores y genere los binarios en `bin/`, son solo advertencias del compilador (`-Wall -Wextra -Wpedantic`), no rompen la compilación.