/****************************************************/
/* PROGRAMA          FECHAS.C                       */
/* FELIPE ANGERIZ ESTEFANELL                        */
/****************************************************/
#include<stdio.h>
#include<time.h>
#include <unistd.h>
//MI HEADER
#include "fechas.h"


//#define MH_ERROR ""

int comp_date(int hora,int minutos,int segundos)
{
    //ESTRUCTURA PARA SEGUNDOS EN CRUDO
    time_t tiempoRaw;
    //ESTRUCTURA PARA FECHA HORA,ETC, DE MANERA ORGANIZADA
    struct tm *infoTiempo;
    

    if(hora >= 0 && hora <= 23)
    {
        //NO HAGO NADA
    }
    else
    {
        return H_ERROR;
    }

    if(minutos >= 0 && minutos <= 59)
    {
        //NO HAGO NADA
    }
    else
    {
        return M_ERROR;
    }
    if(segundos >= 0 && segundos <= 59)
    {
        //NO HAGO NADA
    }

    else
    {
        return S_ERROR;
    }

    while (1) 
    {
        time(&tiempoRaw);                   
        infoTiempo = localtime(&tiempoRaw);  
        sleep(1);
        if(hora == infoTiempo->tm_hour && minutos == infoTiempo->tm_min && segundos == infoTiempo->tm_sec)
        {
            printf("La hora coincide Correctamente ahora mismo\n");
            printf("%02d:%02d:%02d\n", infoTiempo->tm_hour,
                    infoTiempo->tm_min, infoTiempo->tm_sec);
            return 0;
        }
    }
    return 0;
}