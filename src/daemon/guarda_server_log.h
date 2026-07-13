/****************************************************/
/*    HEADER guarda_server_log                      */
/*                   FELIPE ANGERIZ ESTEFANELL      */
/****************************************************/
#include <stdio.h>

#ifndef _GUARDA_SERVER_LOG_H

#define _GUARDA_SERVER_LOG_H

//USO CON DOCKER 
/*#define R_SERVER_LOG "/usr/src/app/logs/serv_log.log"*/

#define R_SERVER_LOG "./logs/serv_log.log"


void open_serv_log(int cli_fd, int status,char cmd, const char *mensaje);


#endif /* .guarda_server_log.h */
