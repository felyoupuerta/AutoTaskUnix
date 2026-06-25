#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<string.h>
/** MIOS **/

#include "config.h"
//#include "protocol.h"
#include "scheduler.h"
#include "server.h"

char msg_out[M_BUFF_S_RESPONSE] = {0};
int status_out = 0;
static void send_cliente(int cli_fd, int status, const char *mensaje)
{
    Response res = {0};
    res.status = status;
    strncpy(res.response, mensaje, sizeof(res.response) - 1);
    
    // Enviamos la estructura completa por el socket
    write(cli_fd, &res, sizeof(Response));
}

void* server_loop(void* arg)
{
    (void)arg;
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
            case CMD_ADD:
                printf("[SERVER] HAS ELEGIDO CMD_ADD\n");
                
                if(scheduler_add_task(&req) == 0)
                {
                    snprintf(msg_out,sizeof(msg_out), "[OK] Tarea añadida correctamente.\n");                
                }
                else
                {
                    status_out = -1;
                    snprintf(msg_out,sizeof(msg_out), "[BAD] No hay huecos disponibles en el scheduler.\n");                              }
                fflush(stdout);
                break;

            case CMD_LIST:
                printf("[SERVER] HAS ELEGIDO CMD_LIST\n");
                scheduler_list_task(&req);
                snprintf(msg_out, sizeof(msg_out), "[INFO] Listado solicitado (Procesando...)\n");
                fflush(stdout);
                break;

            case CMD_RUN:
                printf("[SERVER] HAS ELEGIDO CMD_RUN\n");
                scheduler_run_task(&req);
                snprintf(msg_out, sizeof(msg_out), "[OK] Realizada la ejecución de la tarea %d.\n", req.task_id);
                fflush(stdout);
                break;

            default:
                printf("[SERVER] COMANDO DESCONOCIDO\n");
                fflush(stdout);
                break;
        }
        //DEVOLVER DATOS AL CLIENTE ANTES DE CERRAR EL SOCKET

        send_cliente(cli_fd, status_out, msg_out);

        close(cli_fd);
    }
    return 0;
}






