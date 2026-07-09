/****************************************************/
/*    FICHER C guarda_server_log                    */
/*                   FELIPE ANGERIZ ESTEFANELL      */
/****************************************************/
#include"guarda_server_log.h"
#include<stdio.h>
#include<time.h>

/*
int main(void)
{
  open_serv_log();
}
*/

void open_serv_log(int cli_fd, int status,const char *mensaje)
{
  time_t tiempoRaw;

  struct tm *infoTiempo;
  
  time(&tiempoRaw)
  infoTiempo = localtime(&tiempoRaw);

  /*
    ESTRUCTURA tiempoRaw

    infoTiempo -> tm_hour
    infoTiempo -> tm_min
    infoTiempo -> tm_sec
    infoTiempo -> tm_mday
    infoTiempo -> tm_mday
    infoTiempo -> tm_min

  */

  FILE *f;



  f = fopen(R_SERVER_LOG, "a");

  if(f == NULL)
  {
    printf("[LOG ERROR] Error al abrir el ficher de log del servidor.\n");
  }
  else 
  {
    printf("[OK]Archivo abierto correctamente\n");
  }
  
  fprintf(f,"%02d/%02d/%02d %02d:%02d:%02d %s",mensaje
                                              


          );
  printf("Cerrrando FD de el fichero\n");
  fclose(f);
}
