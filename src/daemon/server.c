#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/un.h>

/** MIOS **/

#include "config.h"
//#include "protocol.h"
#include "scheduler.h"



void* server_loop(void* arg)
{
    
    int sock = socket(AF_UNIX, SOCK_STREAM,0);
    if(sock < 0)
    {
        perror("[ERROR] AL crear el socket\n");
        exit(EXIT_FAILURE);
    }
    
    struct sockaddr_un addr = {0};
    struct sockaddr_un cl_addr = {0};
    socklen_t tam = sizeof(cl_addr);

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCK_F,sizeof(addr.sun_path) -1);
    
    //DESLINKAR EL SOCK_F POR SI LA ULTIMA VEZ NO SE CERRÓ
    unlink(SOCK_F);

    if(bind(sock,(struct sockaddr*)&addr,sizeof(addr)) != 0)
    {
        perror("[ERROR] ejecutando el BIND\n");
        exit(EXIT_FAILURE);
    }
   
    if(listen(sock,BACKLOG) != 0)
    {
        perror("[ERROR] Al ejecutar el listen()\n");
        exit(EXIT_FAILURE);
    }
    
    while(1)
    {
        int cli_fd = accept(sock,(struct sockaddr *)&cl_addr,&tam);
        if(cli_fd < 0)
        {
            perror("[ERROR] Al aceptar cliente\n");
        }
        printf("! Cliente conectado!\n");

        //Inicializamos la estructura de REQUEST de tasks declarada en --> protocol.h
        Request req; 
        
        ssize_t bytes_leidos = read(cli_fd,&req,sizeof(req));
        
        if(bytes_leidos <= 0)
        {
            perror("[ERROR] al recibir los datos por el socket\n");
            close(cli_fd);
        }
        switch(req.comando)
        {
            case CMD_LIST:
                printf("[SERVER] HAS ELEGIDO CMD_LIST\n");
                scheduler_list_task(&req);
                fflush(stdout);
                break;
            case CMD_ADD:
                printf("[SERVER] HAS ELEGIDO CMD_ADD\n");
                scheduler_add_task(&req);
                fflush(stdout);
                break;
            case CMD_RUN:
                printf("[SERVER] HAS ELEGIDO CMD_RUN\n");
                fflush(stdout);
                break;
            default:
                printf("[SERVER] COMANDO DESCONOCIDO\n");
                fflush(stdout);
                break;
        }
        close(cli_fd);

         






    }
    return 0;
}
