#ifndef _SCHEDULER_H
#include "config.h"
#include "protocol.h"
/*orquestador*/

typedef enum
{
    ESTADO_ESPERANDO,
    ESTADO_ERROR,
    ESTADO_RUNNING
} TaskStatus;

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

// FUNCIONES DE CMD
int scheduler_add_task(Request *req);
void scheduler_list_task(Request *req);

// FUNC PARA EL ENUM DE ESTADOS
const char* state_to_text(TaskStatus estado);

//FUNC PARA EL RUN DE LAS TAREAS
void scheduler_run_task(Request *req);



#endif /* scheduler.h */ 
