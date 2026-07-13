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
#include "errores.h"


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
        printf("  • list       | ls           Muestra la lista de tareas\n"        );
        printf("  • add        | a            Tarea con INTERVALO\n"               );
        printf("  • at         | at           Tarea con HORA FIJA\n"               );
        printf("  • run        | x <id>       Ejecuta una tarea por su ID\n"       );
        printf("  • delete     | rm <id>      Elimina una tarea por su ID\n\n"     );
        printf("----------------------------------------------------------------\n");
        printf("Usar: './bin/taskctl help' para ver la ayuda detallada.\n"         );
        printf("================================================================\n");
        exit(argc < 2 ? EC_FALTA_ARGUMENTO : EC_OK);
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
            req.s_intervalo = 5;
        }
        else
        {
            if (valor == 0) req.s_intervalo = 5;
            else req.s_intervalo = (int)valor;
        }
    }
    
    else if(strncmp(argv[1],"at",2) == 0 )
    {
        int hor, min, sec;
        printf("Opcion escogida: AT\n");
        req.comando = CMD_ADD;
        req.tipo = TIPO_FIJO;
        printf("Comando: \n");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;
        strcpy(req.cmd,command);
        printf("Añadir hora exacta de ejecucion: \n");
        fflush(stdout);
        printf("\n > Hora: ");
        fflush(stdout);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) exit(EC_ERROR_LECTURA);
        hor = strtol(buffer, &end, 10);
        while (*end == ' ' || *end == '\t') end++;
        if (*end != '\n' && *end != '\0')
        {
            printf("Error en la hora ingresada, debe ser un numero entre 0 y 23\n");
            exit(EC_NUMERO_INVALIDO);
        }
        if (hor < 0 || hor > 23)
        {
            printf("Error en la hora ingresada, debe ser entre 0 y 23\n");
            exit(EC_NUMERO_INVALIDO);
        }
        printf("\n > Minutos: ");
        fflush(stdout);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) exit(EC_ERROR_LECTURA);
        min = strtol(buffer, &end, 10);
        while (*end == ' ' || *end == '\t') end++;
        if (*end != '\n' && *end != '\0')
        {
            printf("Error en el minuto ingresado, debe ser un numero entre 0 y 59\n");
            exit(EC_NUMERO_INVALIDO);
        }
        if (min < 0 || min > 59)
        {
            printf("Error en el minuto ingresado, debe estar entre el 0 y el 59\n");
            exit(EC_NUMERO_INVALIDO);
        }
        printf("\n > Segundos: ");
        fflush(stdout);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) exit(EC_ERROR_LECTURA);
        sec = strtol(buffer, &end, 10);
        while (*end == ' ' || *end == '\t') end++;
        if (*end != '\n' && *end != '\0')
        {
            printf("Error en los segundos ingresados, deben ser un numero entre 0 y 59\n");
            exit(EC_NUMERO_INVALIDO);
        }
        if (sec < 0 || sec > 59)
        {
            printf("Error en los segundos ingresados, deben estar entre 0 y 59\n");
            exit(EC_NUMERO_INVALIDO);
        }
        req.h = hor;
        req.m = min;
        req.s = sec;
        }

    else if(strncmp(argv[1],"run",3) == 0 || strcmp(argv[1],"x") == 0)
    {
        if(argc< 3)
        {
            printf("Falta el argumento de el ID\n");
            printf("Ejemplo: ./bin/taskctl run <ID>\n");
            exit(EC_FALTA_ARGUMENTO);
        }
        req.comando = CMD_RUN;
        int id_maped = atoi(argv[2]);

        if (id_maped < 0)
        {
            printf("[ERROR] El ID no puede ser negativo.\n");
            exit(EC_ID_INVALIDO);
        }

        req.task_id = id_maped;
    }
    else if(strncmp(argv[1],"delete",6) == 0 || strcmp(argv[1],"rm") == 0)
    {
        if(argc< 3)
        {
            printf("Falta el argumento de el ID\n");
            printf("Ejemplo: ./bin/taskctl delete <ID-tarea>\n");
            exit(EC_FALTA_ARGUMENTO);
        }
        char confirmacion[8] = {0};
        printf("Estas seguro de que quieres eliminar la tarea con ID %s? (s/n): ", argv[2]);
        fflush(stdout);
        fgets(confirmacion,sizeof(confirmacion), stdin);
        
        

        if(confirmacion[0] != 's' && confirmacion[0] != 'S')
        {
            printf("Operacion Cancelada.\n");
            exit(EC_OK);
        }

        req.comando = CMD_DELETE;
        int id_maped = atoi(argv[2]);
        req.task_id = id_maped;
    }
    else if(strncmp(argv[1],"status",6) == 0 || strcmp(argv[1],"st") == 0)
    {
        if(argc<3)
        {
            printf("Falta el argumento de el ID\n");
            printf("Ejemplo: ./bin/taskctl status <ID>\n");
            exit(EC_FALTA_ARGUMENTO);
        }

        int id_searched = 0;
        id_searched = atoi(argv[2]);
        req.comando = CMD_STATUS;
        req.task_id = id_searched;
    }
    else
    {
        printf("[ERROR] COMANDO NO VALIDO: %s\n", argv[1]);
        printf("Usá './bin/taskctl help' para ver los comandos disponibles.\n");
        exit(EC_COMANDO_DESCONOCIDO);
    }

    int rc = send_request(&req);

    return rc;

    
}