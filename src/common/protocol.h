#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include "config.h"
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
} Request;


typedef struct
{
    int status;
    char response[M_BUFF_S_RESPONSE];

} Response;

#endif /* protocol.h */ 
