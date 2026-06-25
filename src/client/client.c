#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/un.h>

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
        perror("[ERROR] ERROR CONFIGURANDO EL SOCKET\n");
        exit(EXIT_FAILURE);
    }
    //Definimos el Struct con el nombre addr
    struct sockaddr_un addr = {0};
    //calculamos el tamaño del struct completo
    socklen_t tam = sizeof(addr);
    //familia de socket linux, porque voy a usar un fichero .sock
    addr.sun_family=AF_UNIX;
    //COPIO MI SOCK_F(RUTA DE EL .sock) a el sun path del struct
    strncpy(addr.sun_path,SOCK_F,sizeof(addr.sun_path) -1 );

    //CONECTAMOS Y VALIDAMOS LA CONEXION CON EL SOCKET
    if(connect(fd_sck,(struct sockaddr*)&addr,sizeof(addr)) < 0)
    {
        perror("[ERROR] ERROR AL CONECTAR AL SOCKET\n");
        exit(EXIT_FAILURE);
    }
    //USAMOS WRITE PARA ENV INFO POR EL SOCKET
    write(fd_sck,req,sizeof(Request));
    close(fd_sck);


    return 0;
} 
