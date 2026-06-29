/****************************************************/
/*    HEADER SERVER.H                               */
/*                   FELIPE ANGERIZ ESTEFANELL      */
/****************************************************/

#include<signal.h>

#ifndef _SERVER_H
#define _SERVER_H

//FUNC PARA EL LOOP CONSTANTE DEL SERVER
void* server_loop(void* arg);

//MANEJADOR PARA LA SIGHUP
void sighup_handler(int signum);

#endif /* server.h */ 
