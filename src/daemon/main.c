#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

/* MIAS */
#include"scheduler.h"
#include"server.h"

int main(void)
{
    //INICIO EL SCHEDULER, LITA DE TAREAS CON ID -1, ETC ...
    scheduler_init();
    
    //IDENTIFICADORES DEL HILO
    pthread_t hilo_scheduler;
    pthread_t hilo_server;
    
    //CREO EL HILO CON LA SYSCALL
    if(pthread_create(&hilo_scheduler,NULL,scheduler_loop,NULL) != 0)
    {
        perror("[ERROR] al crear el hilo de schduler_loop()\n");
        exit(EXIT_FAILURE);
    }
    if(pthread_create(&hilo_server,NULL,server_loop,NULL) != 0)
    {
        perror("[ERROR] al crear el hilo de server_loop()\n");
        exit(EXIT_FAILURE);
    }
    

    //ME QUEDO ESPERANDO A QUE EL HILO TERMINE, NO LO HARA NUNCA PQ ES UN WHILE(1)
    if(pthread_join(hilo_scheduler,NULL) != 0)
    {
        perror("[ERROR] al hacer el Join");
        exit(EXIT_FAILURE);
    }


    if(pthread_join(hilo_server,NULL) != 0)
    {
        perror("[ERROR] al hacer el Join del server");
        exit(EXIT_FAILURE);
    }

    
    return 0;
}
