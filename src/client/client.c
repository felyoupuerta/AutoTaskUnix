/****************************************************/
/*    PROGRAMA CLIENTE.C                            */
/*                    FELIPE ANGERIZ ESTEFANELL      */
/****************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<string.h>

#include "config.h"
#include "protocol.h"
#include "client.h"

int send_request(Request *req)
{

    //GENERAMOS EL SOCKET DE TIPO UNIX
    int fd_sck = socket(AF_UNIX, SOCK_STREAM, 0);
    //VRIFICAMOS LA CREACION CORRECTA DE EL SOCKET
    if(fd_sck < 0)
    {
        perror("[ERROR] ERROR CONFIGURANDO EL SOCKET");
        exit(EXIT_FAILURE);
    }
    //Definimos el Struct con el nombre addr
    struct sockaddr_un addr = {0};
    
    //calculamos el tamaño del struct completo COMENTADO POR ACTUAL DESUSO
    //socklen_t tam = sizeof(addr);
    
    //familia de socket linux, porque voy a usar un fichero .sock
    addr.sun_family=AF_UNIX;
    //COPIO MI SOCK_F(RUTA DE EL .sock) a el sun path del struct
    strncpy(addr.sun_path,SOCK_F,sizeof(addr.sun_path) -1 );

    //CONECTAMOS Y VALIDAMOS LA CONEXION CON EL SOCKET
    if(connect(fd_sck,(struct sockaddr*)&addr,sizeof(addr)) < 0)
    {
        perror("[ERROR] ERROR AL CONECTAR AL SOCKET");
        close(fd_sck);
        exit(EXIT_FAILURE);
    }
    //USAMOS WRITE PARA ENV INFO POR EL SOCKET
    if(write(fd_sck,req,sizeof(Request)) < 0)
    {
        //VALIDAMOS QUE EL ENVIO DE LA PETICION SEA CORRECTO
        perror("[ERROR] ERROR AL ENVIAR DATOS POR EL SOCKET");
        close(fd_sck);
        exit(EXIT_FAILURE);
    }
    
    //DECLARO EL STRUCT DE RESPUESTA VACIO
    Response resp = {0};

    ssize_t n;
    while((n = read(fd_sck, &resp, sizeof(Response))) > 0)
    {
        // Imprimir cada fragmento recibido del servidor
        printf("%s", resp.response);
        fflush(stdout);
        memset(&resp, 0, sizeof(resp));
    }

    if(n < 0)
    {
        perror("[ERROR] al recibir datos desde el servidor");
    }

    close(fd_sck);
    return 0;
} 

/*
ESTRUCTURA DE RESPONSE

typedef struct
{
    int status;
    char response[M_BUFF_S_RESPONSE];
} Response;

*/