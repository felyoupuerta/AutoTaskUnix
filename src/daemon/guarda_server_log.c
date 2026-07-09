/****************************************************/
/* FICHER C guarda_server_log                       */
/* FELIPE ANGERIZ ESTEFANELL                        */
/****************************************************/
#include "guarda_server_log.h"
#include <stdio.h>
#include <time.h>

void open_serv_log(int cli_fd, int status,char cmd, const char *mensaje)
{
    time_t tiempoRaw;
    struct tm *infoTiempo;
    FILE *f;

    time(&tiempoRaw);
    infoTiempo = localtime(&tiempoRaw);

    //Abro fichero modo append
    f = fopen(R_SERVER_LOG, "a");

    if (f == NULL)
    {
        printf("[LOG ERROR] Error al abrir el fichero de log del servidor.\n");
        return; // Salgo no se pudo abrir
    }
    else 
    {
        printf("[OK] Archivo de log abierto correctamente\n");
    }
    //AÑADIR EL char cmd[M_BUFF_CMD]; PARA VER EXACTAMENTE QUE COMANDO SE AÑADIÓ
    fprintf(f, "[FECHA %02d/%02d/%02d] [HORA: %02d:%02d:%02d] [FD: %d] [STATUS: %d] [CMD: %d]-> [SALIDA]%s\n",
            infoTiempo->tm_mday,
            infoTiempo->tm_mon + 1,      // tm_mon va de 0 a 11
            infoTiempo->tm_year + 1900,  // tm_year cuenta desde 1900
            infoTiempo->tm_hour,
            infoTiempo->tm_min,
            infoTiempo->tm_sec,
            cli_fd,
            status,
            cmd,
            mensaje);
              
    printf("Cerrando FD del fichero de log\n");
    fclose(f);
}