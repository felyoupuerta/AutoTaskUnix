# AutoTaskUnix / TaskMon

AutoTaskUnix es un proyecto en C para practicar la creación de un **demonio de tareas** y un **cliente de control** usando:

- sockets UNIX (`AF_UNIX`),
- hilos POSIX (`pthread`),
- estructuras compartidas,
- un protocolo simple de mensajes,
- y un `makefile` para compilar todo el proyecto.

La idea principal es tener un programa servidor, llamado **demonio**, que queda ejecutándose en segundo plano y recibe órdenes desde otro programa, llamado **cliente**.

---

## Tabla de contenidos

1. [Qué hace el proyecto](#qué-hace-el-proyecto)
2. [Arquitectura general](#arquitectura-general)
3. [Estructura de carpetas](#estructura-de-carpetas)
4. [Requisitos](#requisitos)
5. [Cómo compilar](#cómo-compilar)
6. [Cómo ejecutar el demonio](#cómo-ejecutar-el-demonio)
7. [Cómo ejecutar el cliente](#cómo-ejecutar-el-cliente)
8. [Comandos disponibles del cliente](#comandos-disponibles-del-cliente)
9. [Cómo funciona la comunicación](#cómo-funciona-la-comunicación)
10. [Explicación de archivos importantes](#explicación-de-archivos-importantes)
11. [Explicación de funciones principales](#explicación-de-funciones-principales)
12. [Flujo completo de uso](#flujo-completo-de-uso)
13. [Limitaciones actuales](#limitaciones-actuales)
14. [Solución de problemas comunes](#solución-de-problemas-comunes)

---

## Qué hace el proyecto

El proyecto compila dos programas:

| Programa | Ruta generada | Función |
| --- | --- | --- |
| Demonio | `bin/taskd` | Servidor que escucha peticiones por socket UNIX y gestiona una lista de tareas. |
| Cliente | `bin/taskctl` | Programa de consola que envía comandos al demonio. |

El cliente permite enviar tres tipos de órdenes al demonio:

1. **Listar tareas** registradas.
2. **Añadir una tarea** indicando un comando y un intervalo.
3. **Marcar una tarea para ejecución manual** usando su ID.

> Importante: en el estado actual del código, el scheduler imprime qué tarea debería ejecutarse y cambia estados internos, pero todavía no lanza procesos reales con `fork`, `exec`, `system` u otra llamada equivalente.

---

## Arquitectura general

El proyecto se divide en tres partes:

```text
Cliente taskctl
     |
     | Request por socket UNIX
     v
Socket /tmp/taskDemonio.sock
     |
     v
Demonio taskd
     |
     +-- Hilo servidor: acepta clientes y procesa comandos
     |
     +-- Hilo scheduler: revisa periódicamente la lista de tareas
```

El demonio crea dos hilos:

- un hilo para el **servidor**, que espera conexiones de clientes;
- un hilo para el **scheduler**, que revisa las tareas guardadas.

Ambos hilos trabajan sobre una lista global de tareas protegida con un `pthread_mutex_t`.

---

## Estructura de carpetas

```text
.
├── makefile
├── readme.md
├── tasks.conf
├── script-git/
│   └── subida.sh
└── src/
    ├── client/
    │   ├── client.c
    │   ├── client.h
    │   └── main.c
    ├── common/
    │   ├── config.h
    │   └── protocol.h
    └── daemon/
        ├── main.c
        ├── scheduler.c
        ├── scheduler.h
        ├── server.c
        └── server.h
```

Durante la compilación se generan también estas carpetas:

```text
obj/    # archivos objeto .o
bin/    # ejecutables finales
```

---

## Requisitos

Para compilar y ejecutar el proyecto necesitas un sistema tipo Unix/Linux con:

- `gcc`,
- `make`,
- soporte para sockets UNIX,
- soporte para pthreads.

En Debian, Ubuntu o derivados puedes instalar lo básico con:

```bash
sudo apt update
sudo apt install build-essential
```

---

## Cómo compilar

Desde la raíz del proyecto ejecuta:

```bash
make
```

También puedes usar explícitamente:

```bash
make all
```

Ambos comandos compilan:

- el demonio: `bin/taskd`,
- el cliente: `bin/taskctl`.

Si quieres recompilar desde cero:

```bash
make clean
make
```

O en una sola línea:

```bash
make clean && make
```

### Compilar solo el demonio

```bash
make daemon
```

### Compilar solo el cliente

```bash
make client
```

### Limpiar archivos generados

```bash
make clean
```

Esto elimina:

- `obj/`,
- `bin/`.

---

## Cómo ejecutar el demonio

Primero compila el proyecto:

```bash
make
```

Después ejecuta el demonio:

```bash
./bin/taskd
```

El demonio queda ejecutándose en primer plano y crea el socket UNIX:

```text
/tmp/taskDemonio.sock
```

Mientras el demonio esté activo, podrás usar el cliente desde otra terminal.

Para detener el demonio, pulsa:

```bash
Ctrl + C
```

---

## Cómo ejecutar el cliente

El cliente se ejecuta con:

```bash
./bin/taskctl <comando>
```

Ejemplos:

```bash
./bin/taskctl list
./bin/taskctl add
./bin/taskctl run 1
```

El cliente se conecta al socket UNIX del demonio y le envía una estructura `Request` con la orden seleccionada.

> El demonio debe estar ejecutándose antes de lanzar el cliente. Si no está activo, el cliente fallará al conectar con el socket.

---

## Comandos disponibles del cliente

### 1. Listar tareas

```bash
./bin/taskctl list
```

Este comando envía `CMD_LIST` al demonio.

El demonio recorre su lista interna de tareas y muestra información como:

- ID de la tarea,
- intervalo,
- última ejecución,
- estado,
- PID guardado.

---

### 2. Añadir una tarea

```bash
./bin/taskctl add
```

El cliente pedirá por teclado:

1. el comando que quieres registrar;
2. el intervalo de ejecución.

Ejemplo de uso:

```text
$ ./bin/taskctl add
Opcion escogida: ADD
Introduce el comando a ejecutar:
echo hola
Deseas añadir intervalo?(por defecto 5) si pones 0 se pondrá por defecto, si no debes poner un entero
10
```

En este caso se envía al demonio una tarea con:

- comando: `echo hola`,
- intervalo: `10` segundos.

Si introduces `0` como intervalo, el cliente usa `5` segundos por defecto.

---

### 3. Ejecutar una tarea por ID

```bash
./bin/taskctl run <id>
```

Ejemplo:

```bash
./bin/taskctl run 1
```

Este comando envía `CMD_RUN` al demonio con el ID indicado.

El demonio busca la tarea con ese ID y la marca como `ESTADO_RUNNING`.

---

## Cómo funciona la comunicación

La comunicación entre cliente y demonio se hace mediante un **socket UNIX**.

La ruta del socket está definida en `src/common/config.h`:

```c
#define SOCK_F "/tmp/taskDemonio.sock"
```

El cliente y el demonio deben usar la misma ruta para poder comunicarse.

### Protocolo de mensajes

El archivo `src/common/protocol.h` define los comandos disponibles:

```c
typedef enum
{
    CMD_LIST,
    CMD_ADD,
    CMD_RUN
} CmdType;
```

También define la estructura enviada desde el cliente al demonio:

```c
typedef struct
{
    int task_id;
    CmdType comando;
    char cmd[M_BUFF_CMD];
    int s_intervalo;
} Request;
```

Cada vez que ejecutas `taskctl`, el cliente rellena una estructura `Request` y la manda al demonio usando `write`.

---

## Explicación de archivos importantes

### `makefile`

Define cómo compilar el proyecto.

Variables importantes:

```make
CC       := gcc
CFLAGS   := -Wall -Wextra -Wpedantic -O2 -Isrc/common -Isrc/daemon -Isrc/client
LDFLAGS  := -pthread
```

- `CC`: compilador usado.
- `CFLAGS`: flags de compilación y rutas de headers.
- `LDFLAGS`: flags de enlazado; aquí se usa `-pthread` porque el demonio usa hilos.

Targets principales:

| Target | Acción |
| --- | --- |
| `make` / `make all` | Compila demonio y cliente. |
| `make daemon` | Compila solo `bin/taskd`. |
| `make client` | Compila solo `bin/taskctl`. |
| `make clean` | Elimina `obj/` y `bin/`. |

---

### `src/common/config.h`

Contiene constantes globales usadas por cliente y demonio.

Constantes principales:

| Constante | Significado |
| --- | --- |
| `SOCK_F` | Ruta del socket UNIX. |
| `MAX_CL` | Número máximo de tareas guardadas en memoria. |
| `M_BUFF_CMD` | Tamaño máximo del comando de una tarea. |
| `M_BUFF_S_RESPONSE` | Tamaño máximo para respuestas. |
| `BACKLOG` | Tamaño de la cola de conexiones pendientes del socket. |

---

### `src/common/protocol.h`

Define el protocolo común entre cliente y servidor.

Incluye:

- el enum `CmdType`,
- la estructura `Request`,
- la estructura `Response`.

Actualmente el cliente envía principalmente `Request`. La estructura `Response` está definida para futuras respuestas del demonio al cliente.

---

### `src/daemon/main.c`

Es el punto de entrada del demonio.

Responsabilidades:

1. Inicializar el scheduler con `scheduler_init()`.
2. Crear el hilo del scheduler con `pthread_create()`.
3. Crear el hilo del servidor con `pthread_create()`.
4. Esperar ambos hilos con `pthread_join()`.

Como los hilos tienen bucles infinitos, el demonio queda ejecutándose hasta que el usuario lo detenga.

---

### `src/daemon/server.c`

Implementa el servidor UNIX socket.

Responsabilidades:

1. Crear el socket con `socket(AF_UNIX, SOCK_STREAM, 0)`.
2. Configurar la dirección `struct sockaddr_un`.
3. Eliminar un socket anterior con `unlink(SOCK_F)`.
4. Asociar el socket con `bind()`.
5. Escuchar conexiones con `listen()`.
6. Aceptar clientes con `accept()`.
7. Leer una estructura `Request` con `read()`.
8. Ejecutar la acción correspondiente según `req.comando`.

Comandos que procesa:

| Comando | Función llamada |
| --- | --- |
| `CMD_LIST` | `scheduler_list_task(&req)` |
| `CMD_ADD` | `scheduler_add_task(&req)` |
| `CMD_RUN` | `scheduler_run_task(&req)` |

---

### `src/daemon/scheduler.h`

Declara las estructuras y funciones del scheduler.

Define el enum de estados:

```c
typedef enum
{
    ESTADO_ESPERANDO,
    ESTADO_ERROR,
    ESTADO_RUNNING
} TaskStatus;
```

Define la estructura `Task`:

```c
typedef struct
{
    int id;
    char cmd[M_BUFF_CMD];
    int intervalo;
    time_t last_run;
    TaskStatus estado;
    pid_t pid;
} Task;
```

Cada tarea tiene:

- un ID,
- un comando,
- un intervalo,
- una fecha de última ejecución,
- un estado,
- un PID.

---

### `src/daemon/scheduler.c`

Implementa la lógica de tareas.

Contiene:

```c
static Task lista_tareas[MAX_CL];
static pthread_mutex_t mutex;
```

Esto significa que las tareas se guardan en memoria en un array fijo y se protegen con un mutex para evitar problemas entre hilos.

---

### `src/client/main.c`

Es el punto de entrada del cliente.

Responsabilidades:

1. Leer argumentos de línea de comandos.
2. Decidir si el usuario pidió `list`, `add` o `run`.
3. Rellenar una estructura `Request`.
4. Llamar a `send_request(&req)`.

Ejemplos:

```bash
./bin/taskctl list
./bin/taskctl add
./bin/taskctl run 1
```

---

### `src/client/client.c`

Implementa la función que conecta con el demonio.

Responsabilidades:

1. Crear un socket UNIX.
2. Configurar la ruta `/tmp/taskDemonio.sock`.
3. Conectar con el demonio usando `connect()`.
4. Enviar el `Request` usando `write()`.
5. Cerrar el socket.

---

### `script-git/subida.sh`

Script auxiliar para tareas de Git.

Permite:

1. subir cambios a la rama `DEV`,
2. pasar cambios de `DEV` a `main`,
3. salir.

No es necesario para compilar ni ejecutar el programa.

---

## Explicación de funciones principales

### `scheduler_init()`

Inicializa el mutex y marca todas las posiciones de la lista de tareas como vacías.

Una tarea vacía se identifica con:

```c
id = -1
```

---

### `scheduler_loop(void *arg)`

Es el bucle principal del scheduler.

Hace lo siguiente:

1. obtiene la hora actual con `time(NULL)`;
2. bloquea el mutex;
3. recorre la lista de tareas;
4. busca tareas en estado `ESTADO_ESPERANDO`;
5. comprueba si ya pasó su intervalo;
6. cambia la tarea a `ESTADO_RUNNING`;
7. actualiza `last_run`;
8. imprime el comando asociado;
9. libera el mutex;
10. espera un segundo con `sleep(1)`.

---

### `scheduler_add_task(Request *req)`

Añade una nueva tarea a la primera posición libre de `lista_tareas`.

Usa datos del `Request`:

- `req->cmd`: comando de la tarea;
- `req->s_intervalo`: intervalo de ejecución.

Si encuentra espacio, devuelve `0`.

Si la lista está llena, devuelve `-1`.

---

### `scheduler_list_task(Request *req)`

Lista las tareas registradas.

Por cada tarea muestra:

- ID,
- intervalo,
- última ejecución,
- estado,
- PID.

Actualmente imprime la información en la salida estándar del demonio.

---

### `state_to_text(TaskStatus estado)`

Convierte un estado interno a texto legible.

Ejemplos:

| Estado | Texto |
| --- | --- |
| `ESTADO_ESPERANDO` | `ESPERANDO` |
| `ESTADO_RUNNING` | `RUNNING` |
| `ESTADO_ERROR` | `ESTADO_ERROR` |

---

### `scheduler_run_task(Request *req)`

Busca una tarea por ID usando:

```c
req->task_id
```

Si la encuentra:

1. imprime el comando de la tarea;
2. imprime el ID;
3. cambia el estado a `ESTADO_RUNNING`.

Actualmente no ejecuta todavía el comando como proceso real.

---

### `server_loop(void *arg)`

Es el bucle principal del servidor.

Hace lo siguiente:

1. crea el socket UNIX;
2. hace `bind()` sobre `/tmp/taskDemonio.sock`;
3. queda escuchando con `listen()`;
4. acepta clientes con `accept()`;
5. lee un `Request`;
6. llama al scheduler según el comando recibido.

---

### `send_request(Request *req)`

Función del cliente que envía peticiones al demonio.

Hace lo siguiente:

1. crea un socket UNIX;
2. conecta con `/tmp/taskDemonio.sock`;
3. envía la estructura `Request` al demonio;
4. cierra el socket.

---

## Flujo completo de uso

Terminal 1:

```bash
make clean && make
./bin/taskd
```

Terminal 2:

```bash
./bin/taskctl add
```

Introduce, por ejemplo:

```text
echo hola
5
```

Después lista las tareas:

```bash
./bin/taskctl list
```

Y ejecuta manualmente la tarea con ID 1:

```bash
./bin/taskctl run 1
```

En la terminal donde corre el demonio verás los mensajes generados por el servidor y el scheduler.

---

## Limitaciones actuales

Este proyecto está en desarrollo. Algunas limitaciones importantes son:

1. **Las tareas no se ejecutan como procesos reales todavía.**
   - El scheduler imprime el comando y cambia el estado, pero no hace `fork()`/`exec()`.

2. **El cliente no recibe una respuesta estructurada del demonio.**
   - Existe `Response`, pero todavía no se usa para devolver resultados al cliente.

3. **Las tareas se guardan solo en memoria.**
   - Si detienes el demonio, la lista de tareas se pierde.

4. **`tasks.conf` todavía no se usa.**
   - Podría utilizarse en el futuro para persistencia.

5. **El listado se imprime en el demonio, no en el cliente.**
   - Al ejecutar `taskctl list`, mira la terminal donde está corriendo `taskd`.

---

## Solución de problemas comunes

### Error: no conecta con el socket

Si ves un error parecido a:

```text
[ERROR] ERROR AL CONECTAR AL SOCKET
```

probablemente el demonio no está ejecutándose.

Solución:

```bash
./bin/taskd
```

Luego, en otra terminal:

```bash
./bin/taskctl list
```

---

### Error: socket viejo en `/tmp`

El demonio intenta borrar el socket anterior con `unlink(SOCK_F)` antes de hacer `bind()`.

Si aun así tienes problemas, puedes borrar manualmente el socket:

```bash
rm -f /tmp/taskDemonio.sock
```

Y volver a iniciar:

```bash
./bin/taskd
```

---

### Warnings al compilar

Puede que aparezcan warnings como variables no usadas en el cliente.

Mientras el proceso termine con:

```text
Compilación completada con éxito.
```

entonces los binarios se generaron correctamente.

---

## Resumen rápido

Compilar:

```bash
make clean && make
```

Ejecutar demonio:

```bash
./bin/taskd
```

Listar tareas:

```bash
./bin/taskctl list
```

Añadir tarea:

```bash
./bin/taskctl add
```

Ejecutar tarea por ID:

```bash
./bin/taskctl run 1
```

Limpiar compilación:

```bash
make clean
