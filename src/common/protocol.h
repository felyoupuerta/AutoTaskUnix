/****************************************************/
/*    HEADER PROTOCOL.H                            */
/*                   FELIPE ANGERIZ ESTEFANELL      */
/****************************************************/
#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include "config.h"

typedef enum
{
    TIPO_INTERVALO,
    TIPO_FIJO
} TaskType;

typedef enum
{
    CMD_LIST,
    CMD_ADD,
    CMD_RUN,
    CMD_DELETE
} CmdType;

typedef struct
{
    int task_id;
    CmdType comando;
    char cmd[M_BUFF_CMD];
    int s_intervalo;
    TaskType tipo;
    int h;
    int m;
    int s;
} Request;

typedef struct
{
    int status;
    char response[M_BUFF_S_RESPONSE];

} Response;

#endif /* protocol.h */ 
