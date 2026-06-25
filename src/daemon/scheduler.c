#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

/* MIAS */
#include"scheduler.h"

    
static Task lista_tareas[MAX_CL];
static pthread_mutex_t mutex;

void scheduler_init(void)
{
    pthread_mutex_init(&mutex, NULL);
    
    for(int i = 0;i < MAX_CL; i ++)
    {
        lista_tareas[i].id = -1;
    }

}


void* scheduler_loop(void* arg)
{
    while(1)
    {
        time_t ahora = time(NULL);
        pthread_mutex_lock(&mutex);
        for(int i = 0;i < MAX_CL;i++)
        {
            if(lista_tareas[i].id != -1)
            {
                if(lista_tareas[i].estado == ESTADO_ESPERANDO)
                    if((ahora - lista_tareas[i].last_run) >= lista_tareas[i].intervalo)
                    {
                        lista_tareas[i].estado = ESTADO_RUNNING;
                        lista_tareas[i].last_run = ahora;
                        printf("Ejecutando: %s\n",lista_tareas[i].cmd);
                    }
            }
        }

        /*TAREA DE PRUEBA
        lista_tareas[0].id = 1;
        lista_tareas[0].intervalo = 5; // Cada 5 segundos
        lista_tareas[0].last_run = 0;  //Volver despues? time(NULL);
        lista_tareas[0].estado = ESTADO_ESPERANDO;
        snprintf(lista_tareas[0].cmd, M_BUFF_CMD, "echo 'Hola Felipe'");
        fflush(stdout);
        */
        //DESBLOQUEO EL HILO
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    return NULL;
}

int scheduler_add_task(Request *req)
{
    pthread_mutex_lock(&mutex);
    int i = 0;

    for(i = 0;i<MAX_CL;i++)
    {
        if(lista_tareas[i].id == -1)
        {
            lista_tareas[i].id = lista_tareas[i].id + 1;
            strncpy(lista_tareas[i].cmd, req->cmd,M_BUFF_CMD -1);
            lista_tareas[i].intervalo = req->s_intervalo;
            lista_tareas[i].last_run = 0;
            lista_tareas[i].estado = ESTADO_ESPERANDO;
            pthread_mutex_unlock(&mutex);
            return 0;
        }
    }
    return -1;
}


void scheduler_list_task(Request *req)
{
    (void)req;
    //BLOQUEO HILO
    pthread_mutex_lock(&mutex);
    
    int i = 0;
    for(i = 0;i < MAX_CL;i++)
    {
        if(lista_tareas[i].id == -1)
        {
            continue;
        }
        printf("--------------------------------------------\n");
        printf("Id TAREA: %d\n", lista_tareas[i].id);
        printf("Intervalo de ejecucion: %d\n",lista_tareas[i].intervalo);
        printf("Última ejecución: %lld\n", (long long)lista_tareas[i].last_run);
        const char *texto = state_to_text(lista_tareas[i].estado);
        printf("Estado de la tarea: %s\n", texto);
        printf("PID: %d\n", lista_tareas[i].pid);
    }
    printf("---------------------------------------------------\n");
    //DESBLOQUEAMOS EL HILO
    pthread_mutex_unlock(&mutex);
}

const char* state_to_text(TaskStatus estado)
{

    char *estado_texto;

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
    printf("Estado de la tarea: %s\n", estado_texto);
}