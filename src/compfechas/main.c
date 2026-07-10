/****************************************************/
/* PROGRAMA MAIN DE FECHAS                          */
/* FELIPE ANGERIZ ESTEFANELL                        */
/****************************************************/
#include<stdio.h>
#include<unistd.h>
#include<time.h>
//MI HEADER
#include "fechas.h"




int main(void)
{
    int h,min,sec;

    printf("Intruduzca hora:\n");
    scanf("%d",&h);
    printf("Intruduzca minuto:\n");
    scanf("%d",&min);
    printf("Intruduzca segundos:\n");
    scanf("%d",&sec);
    
    int resultado;
    resultado = comp_date(h,min,sec);
    switch(resultado)
    {
        case H_ERROR:
            printf("Error en la hora ingresada, debe se entre 0 y 23\n");
            break;
        case M_ERROR:
            printf("Error en el minuto ingresado, debe estar entre el 0 y el 59\n");
            break;
        case S_ERROR:
            printf("Error en los segundos ingresados, deben estar entre 0 y 59\n");
            break;
        default:
            printf("Todo funcionó correctamente\n");
            break;
            




    }
    return 0;
}