/****************************************************/
/*    HEADER SCHEDULER.H                            */
/*                   FELIPE ANGERIZ ESTEFANELL      */
/****************************************************/
#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include<time.h>
#include "config.h"
#include "protocol.h"


//ESTADO TAREAS 
typedef enum
{
    ESTADO_ESPERANDO,
    ESTADO_ERROR,
    ESTADO_RUNNING
} TaskStatus;

//TAREAS
typedef struct
{
    int id;
    char cmd[M_BUFF_CMD];
    int intervalo;
    time_t last_run;
    TaskStatus estado;
    pid_t pid;
} Task;


void scheduler_init(void);


void* scheduler_loop(void* arg);


int scheduler_add_task(Request *req);
void scheduler_list_task(char *buffer, size_t size);


const char* state_to_text(TaskStatus estado);


void scheduler_run_task(Request *req);


void scheduler_run_task_stream(Request *req, int cli_fd);


int scheduler_delete_task(Request *req);


int scheduler_comp_run(void);


void guardar_tareas_en_archivo(void);


#endif /* scheduler.h */ 
