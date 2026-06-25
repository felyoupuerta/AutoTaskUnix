#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<time.h>

#include"scheduler.h"


static Task lista_tareas[MAX_CL];
static pthread_mutex_t mutex;


void scheduler_init(void)
{
    pthread_mutex_init(&mutex, NULL);

    for(int i = 0; i < MAX_CL; i++)
    {
        lista_tareas[i].id = -1;
    }
}


void* scheduler_loop(void* arg)
{
    (void)arg;

    while(1)
    {
        time_t ahora = time(NULL);

        pthread_mutex_lock(&mutex);

        for(int i = 0; i < MAX_CL; i++)
        {
            if(lista_tareas[i].id != -1)
            {
                if(lista_tareas[i].estado == ESTADO_ESPERANDO)
                {
                    if((ahora - lista_tareas[i].last_run) >= lista_tareas[i].intervalo)
                    {
                        lista_tareas[i].estado = ESTADO_RUNNING;
                        lista_tareas[i].last_run = ahora;

                        printf("Ejecutando: %s\n", lista_tareas[i].cmd);
                    }
                }
            }
        }

        pthread_mutex_unlock(&mutex);

        sleep(1);
    }

    return NULL;
}


int scheduler_add_task(Request *req)
{
    pthread_mutex_lock(&mutex);

    for(int i = 0; i < MAX_CL; i++)
    {
        if(lista_tareas[i].id == -1)
        {
            lista_tareas[i].id = i + 1;

            strncpy(lista_tareas[i].cmd,
                    req->cmd,
                    M_BUFF_CMD - 1);

            lista_tareas[i].cmd[M_BUFF_CMD - 1] = '\0';

            lista_tareas[i].intervalo = req->s_intervalo;
            lista_tareas[i].last_run = 0;
            lista_tareas[i].estado = ESTADO_ESPERANDO;

            pthread_mutex_unlock(&mutex);

            return 0;
        }
    }

    pthread_mutex_unlock(&mutex);

    return -1;
}


void scheduler_list_task(Request *req)
{
    (void)req;

    pthread_mutex_lock(&mutex);


    for(int i = 0; i < MAX_CL; i++)
    {
        if(lista_tareas[i].id == -1)
        {
            continue;
        }

        printf("--------------------------------------------\n");
        printf("Id TAREA: %d\n", lista_tareas[i].id);
        printf("Intervalo de ejecucion: %d\n", lista_tareas[i].intervalo);
        printf("Última ejecución: %lld\n",
               (long long)lista_tareas[i].last_run);

        printf("Estado de la tarea: %s\n",
               state_to_text(lista_tareas[i].estado));

        printf("PID: %d\n", lista_tareas[i].pid);
    }

    printf("--------------------------------------------\n");


    pthread_mutex_unlock(&mutex);
}



const char* state_to_text(TaskStatus estado)
{
    switch(estado)
    {
        case ESTADO_ESPERANDO:
            return "ESPERANDO";

        case ESTADO_RUNNING:
            return "RUNNING";

        case ESTADO_ERROR:
            return "ESTADO_ERROR";

        default:
            return "DESCONOCIDO";
    }
}



void scheduler_run_task(Request *req)
{
    pthread_mutex_lock(&mutex);

    for(int i = 0; i < MAX_CL; i++)
    {
        if(lista_tareas[i].id != -1 &&
           lista_tareas[i].id == req->task_id)
        {
            printf("TAREA -----> %s\n",
                   lista_tareas[i].cmd);

            printf("Ejecutando Tarea con ID: %d\n",
                   lista_tareas[i].id);


            lista_tareas[i].estado = ESTADO_RUNNING;

            fflush(stdout);

            break;
        }
    }

    pthread_mutex_unlock(&mutex);
} 
