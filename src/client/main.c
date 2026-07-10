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

    if(argc < 2 || strcmp(argv[1], "help") == 0 || strcmp(argv[1], "--help") == 0)
    {
        if(argc < 2)
        {
            printf("[ERROR] Debes pasar al menos 1 argumento.\n");
        }

        printf("==================================================================\n");
        printf(" COMANDOS DISPONIBLES (Puedes usar el comando o su atajo)\n");
        printf("==================================================================\n\n");

        printf("  Formato: ./bin/taskctl [comando] <argumentos>\n\n");

        printf("  [Comando]    / [Atajo]      Descripción\n");
        printf("  --------------------------------------------------------------\n");
        printf("  • list       | ls           Muestra la lista de tareas\n");
        printf("  • add        | a            Añade una nueva tarea\n");
        printf("  • run        | x <id>       Ejecuta una tarea por su ID\n");
        printf("  • delete     | rm <id>      Elimina una tarea por su ID\n\n");

        printf("------------------------------------------------------------------\n");
        printf("Usar: './bin/taskctl help' para ver la ayuda detallada.\n");
        printf("==================================================================\n");
        exit(argc < 2 ? EXIT_FAILURE : EXIT_SUCCESS);
    }


    if(strncmp(argv[1],"list",4) == 0 || strcmp(argv[1],"ls") == 0)
    {
        printf("Opcion List seleccionada\n");
        req.comando = CMD_LIST;
    }
    else if(strncmp(argv[1],"add",3) == 0 || strcmp(argv[1],"a") == 0)
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
    else if(strncmp(argv[1],"run",3) == 0 || strcmp(argv[1],"x") == 0)
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
    else if(strncmp(argv[1],"delete",6) == 0 || strcmp(argv[1],"rm") == 0)
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
    else
    {
        printf("[ERROR] COMANDO NO VALIDO: %s\n", argv[1]);
        printf("Usá './bin/taskctl help' para ver los comandos disponibles.\n");
        exit(EXIT_FAILURE);
    }

    send_request(&req);


    return 0;
}
