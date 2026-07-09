/****************************************************/
/*    HEADER guarda_server_log                      */
/*                   FELIPE ANGERIZ ESTEFANELL      */
/****************************************************/
#include <stdio.h>

#ifndef _GUARDA_SERVER_LOG_H

#define _GUARDA_SERVER_LOG_H

#define R_SERVER_LOG "/home/felipe/Desktop/codigos/c/AutoTaskUnix/logs/serv_log.log"

void open_serv_log(int cli_fd, int status, const char *mensaje);

//void open_serv_log();

#endif /* .guarda_server_log.h */
