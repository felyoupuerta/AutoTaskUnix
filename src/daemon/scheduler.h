#ifndef _SCHEDULER_H
#include "config.h"
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


#endif /* scheduler.h */
