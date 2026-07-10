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

    
    int fd_sck = socket(AF_UNIX, SOCK_STREAM, 0);
    
    if(fd_sck < 0)
    {
        perror("[ERROR] ERROR CONFIGURANDO EL SOCKET");
        exit(EXIT_FAILURE);
    }
    
    struct sockaddr_un addr = {0};
    

    
    
    addr.sun_family=AF_UNIX;
    
    strncpy(addr.sun_path,SOCK_F,sizeof(addr.sun_path) -1 );

    
    if(connect(fd_sck,(struct sockaddr*)&addr,sizeof(addr)) < 0)
    {
        perror("[ERROR] ERROR AL CONECTAR AL SOCKET");
        close(fd_sck);
        exit(EXIT_FAILURE);
    }
    
    if(write(fd_sck,req,sizeof(Request)) < 0)
    {
        
        perror("[ERROR] ERROR AL ENVIAR DATOS POR EL SOCKET");
        close(fd_sck);
        exit(EXIT_FAILURE);
    }
    
    
    Response resp = {0};

    ssize_t n;
    while((n = read(fd_sck, &resp, sizeof(Response))) > 0)
    {
        
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