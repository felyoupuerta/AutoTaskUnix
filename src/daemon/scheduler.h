/****************************************************/
/*    HEADER SCHEDULER.H                            */
/*                   FELIPE ANGERIZ ESTEFANELL      */
/****************************************************/
#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include<time.h>
#include "config.h"
#include "protocol.h"
/*orquestador*/

//ENUM PARA EL ESTADO DE LAS TAREAS 
typedef enum
{
    ESTADO_ESPERANDO,
    ESTADO_ERROR,
    ESTADO_RUNNING
} TaskStatus;
//ESTRUCTUAR PARA LAS TAREAS
typedef struct
{
    int id;
    char cmd[M_BUFF_CMD];
    int intervalo;
    time_t last_run;
    TaskStatus estado;
    pid_t pid;
} Task;

//FUNC INICIALIZARR STRUCTS
void scheduler_init(void);

//FUNC LOOP INFINITO
void* scheduler_loop(void* arg);

// FUNCIONES DE CMD
int scheduler_add_task(Request *req);
void scheduler_list_task(char *buffer, size_t size);

// FUNC PARA EL ENUM DE ESTADOS
const char* state_to_text(TaskStatus estado);

//FUNC PARA EL RUN DE LAS TAREAS
void scheduler_run_task(Request *req);

// FUNC PARA EJECUTAR Y ENVIAR SALIDA POR SOCKET AL CLIENTE
void scheduler_run_task_stream(Request *req, int cli_fd);

//FUNC PARA BORRAR TAREAS USANDO SU ID
int scheduler_delete_task(Request *req);

//FUNCION PARA VERIFICAR EL INTERVALO Y SI HA PASADO EJECUTO LA FUNCION DE RUN
int scheduler_comp_run(void);

//FUNC GUARDAR DATOS DE TAREAS EN EL CONF
void guardar_tareas_en_archivo(void);


#endif /* scheduler.h */ 
