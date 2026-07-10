/****************************************************/
/*    HEADER CONFIG.H                               */
/*                   FELIPE ANGERIZ ESTEFANELL      */
/****************************************************/
#ifndef _CONFIG_H
#define _CONFIG_H


//RUTA DEL SOCKET UNIX
#define SOCK_F "/tmp/taskDemonio.sock"
//FICHER DE ALAMCENAMIENTO DE TAREAS
#define TASK_CONF "tasks.conf"
//DEFINICION DE CANTIDAD DE  TAREAS 
#define MAX_CL 200

//TAMAÑO DEL BUFFER PARA LOS COMANDOS QUE SE ALMACENEN EN LA STRUCT
#define M_BUFF_CMD 256
//TAMAÑO MAXIMO DE LA STRUCT DE RESPUESTA HACIA EL SOCKET
#define M_BUFF_S_RESPONSE 4096
//DEFINICION DE CANTIDAD DE CONEXIONES SIULTANEAS O CLIENTES
#define BACKLOG 50
//DEINICION DEL FICHERO PARA LOS LOGS
#define S_LOG "logs/C_LOG.log"
#define C_LOG "logs/S_LOG.log"

//RETORNOS
#define H_ERROR 11 //RETORNO ERROR DE HORAS
#define M_ERROR 22 //RETORNO ERROR DE MINUTOS
#define S_ERROR 33 //RETORNO ERROR DE SEGUNDOS


#endif /* config.h */
