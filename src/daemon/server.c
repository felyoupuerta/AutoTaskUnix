/****************************************************/
/*    PROGRAMA SERVER.C                             */
/*                   FELIPE ANGERIZ ESTEFANELL      */
/****************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<string.h>

/** MIOS **/
#include "config.h"
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
        int comparar = scheduler_comp_run(&req);
        //SI ES UN ES QUE TUVO EXITO LA COMPARACION
        //Y HAY QUE EJECUTAR NUEVAMENTE LÑA TAREA
        if(comparar == 1)
        {
            printf("[TAREA] la tarea ha cumplido su intervalo\n");
            printf("[TAREA] Se acaba de ejecutar\n");
        }
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
                printf("[INFO] Listado solicitado (Procesando...)\n");

                status_out = 0;
                
                memset(msg_out, 0, sizeof(msg_out)); // Limpio el buffer antes de llenarlo
                printf("[SERVER] buffer puesto a 0\n");
                
                scheduler_list_task(msg_out, sizeof(msg_out));
                
                fflush(stdout);
                break;
            case CMD_DELETE:
                if(scheduler_delete_task(&req) == -1)
                {
                    printf("Error al borrar la tarea con ID: %d\n",req.task_id);
                    snprintf(msg_out,sizeof(msg_out), "[BAD] Error al borrar la tarea.\n");
                }
                else
                {
                    snprintf(msg_out,sizeof(msg_out), "[OK]Tarea eliminada de la lista correcrtamente.\n");
                }
                break;
            case CMD_RUN:
                printf("[SERVER] HAS ELEGIDO CMD_RUN\n");
                
                scheduler_run_task_stream(&req, cli_fd);

                close(cli_fd);
                continue;

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






