/****************************************************/
/*    CLIENTE MAIN.C                                */
/*                   FELIPE ANGERIZ ESTEFANELL      */
/****************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<string.h>

/* MIS HEADERS */
#include "config.h"
#include "protocol.h"
#include "client.h"


int main(int argc, char **argv)
{
    char command[M_BUFF_CMD] = {0};
    char buffer[100] = {0};
    char *end;
    Request req = {0}; //LE RESERVAMOS MEMORIA REAL

    if(argc < 2)
    {
        printf("==================================================================\n");
        printf("Error en el paso de argumentos\n");
        printf("Para que el programa funcione debes pasar al menos 1 argumento\n");
        printf("1- ./bin/taskctl list\n");
        printf("2- ./bin/taskctl add <comando>\n");
        printf("3- ./bin/taskctl run <id>\n");
        printf("4- ./bin/taskctl delete <id>\n");
        printf("==================================================================\n");
        exit(EXIT_FAILURE);
    }


    if(strncmp(argv[1],"list",4) == 0)
    {
        printf("Opcion List seleccionada\n");
        req.comando = CMD_LIST;
    }
    else if(strncmp(argv[1],"add",3) == 0)
    {
        printf("Opcion escogida: ADD\n");
        req.comando = CMD_ADD;
        printf("Introduce el comando a ejecutar: \n");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;

        strcpy(req.cmd,command);
        printf("Deseas añadir intervalo?(por defecto 5) si pones 0 se pondrá por defecto, si no debes poner un entero\n");
        fgets(buffer, sizeof(buffer), stdin);

        
        long valor = strtol(buffer,&end,10);
        if(*end != '\n' && *end != '\0')
        {
            printf("No es un numero valido\n");
        }
        else
        {
            if (valor == 0) req.s_intervalo = 5;
            else req.s_intervalo = (int)valor;
        }




    }
    else if(strncmp(argv[1],"run",3) == 0)
    {
        if(argc< 3)
        {
            printf("Falta el argumento de el ID\n");
            printf("Ejemplo: ./bin/taskctl run <ID>\n");
            exit(EXIT_FAILURE);
        }
        req.comando = CMD_RUN;
        int id_maped = atoi(argv[2]);
        req.task_id = id_maped;
    }
    else if(strncmp(argv[1],"delete",6) == 0)
    {
        if(argc< 3)
        {
            printf("Falta el argumento de el ID\n");
            printf("Ejemplo: ./bin/taskctl delete <ID-tarea>\n");
            exit(EXIT_FAILURE);
        }
        
        req.comando = CMD_DELETE;
        //PASAMOS EL ID DE STRING A ENTERO PARA GUARDARLO 
        //COMO INT EN LA STRUCT DE REQUEST
        int id_maped = atoi(argv[2]);
        req.task_id = id_maped;
    }

    send_request(&req);


    return 0;
}
