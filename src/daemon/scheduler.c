/****************************************************/
/*    PROGRAMA SCHEDULER.C                          */
/*                   FELIPE ANGERIZ ESTEFANELL      */
/****************************************************/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<time.h>
#include<sys/wait.h>
#include<signal.h>
#include"scheduler.h"
#include"server.h"

//función env respuestas por socket cliente
static void send_cliente(int cli_fd, int status, const char *mensaje);


static Task lista_tareas[MAX_CL];
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void scheduler_init(void)
{
    pthread_mutex_lock(&mutex);

    // 1. Inicialización normal de todas las tareas a valores vacíos/por defecto
    for (int i = 0; i < MAX_CL; i++)
    {
        lista_tareas[i].id = -1;
        memset(lista_tareas[i].cmd, 0, sizeof(lista_tareas[i].cmd));
        lista_tareas[i].intervalo = 5;
        lista_tareas[i].last_run = 0;
        lista_tareas[i].pid = 0;
        lista_tareas[i].estado = ESTADO_ESPERANDO;
    }

    // 2. Intentar abrir el archivo tasks.conf para leer las tareas guardadas
    FILE *f = fopen(TASK_CONF, "r");
    if (f == NULL)
    {
        // Si no existe, no es un error crítico; inicializamos por defecto y retornamos
        printf("[SERVER] El archivo %s no existe o no se pudo abrir. Inicialización limpia.\n", TASK_CONF);
        pthread_mutex_unlock(&mutex);
        return;
    }

    char linea[512];
    int i = 0;
    int intervalo;
    char cmd_aux[M_BUFF_CMD];
    while (fgets(linea, sizeof(linea), f) != NULL)
    {
        if (i >= MAX_CL) break;
        // Analizar intervalo y comando
        if (sscanf(linea, "%d:%[^\n]", &intervalo, cmd_aux) == 2)
        {
            lista_tareas[i].id = i + 1;
            lista_tareas[i].intervalo = intervalo;
            strncpy(lista_tareas[i].cmd, cmd_aux, sizeof(lista_tareas[i].cmd) - 1);
            lista_tareas[i].cmd[sizeof(lista_tareas[i].cmd) - 1] = '\0';
            lista_tareas[i].estado = ESTADO_ESPERANDO;
            lista_tareas[i].last_run = 0;
            lista_tareas[i].pid = 0;
            i++;
        }
    }
    fclose(f);

    printf("[SERVER] Se han cargado %d tareas del archivo %s.\n", i, TASK_CONF);
    pthread_mutex_unlock(&mutex);
}


void* scheduler_loop(void* arg)
{
    (void)arg;

    while(1)
    {
        scheduler_comp_run();
        sleep(1);
    }

    return NULL;
}


int scheduler_add_task(Request *req)
{
    pthread_mutex_lock(&mutex);
    
    for(int i = 0; i < MAX_CL; i++)
    {
        if(lista_tareas[i].id == -1)
        {
            lista_tareas[i].id = i + 1;

            snprintf(lista_tareas[i].cmd,
                    sizeof(lista_tareas[i].cmd),
                    "%s",
                    req->cmd);

            lista_tareas[i].cmd[M_BUFF_CMD - 1] = '\0';

            lista_tareas[i].intervalo = req->s_intervalo;
            lista_tareas[i].last_run = 0;
            lista_tareas[i].estado = ESTADO_ESPERANDO;
            lista_tareas[i].pid++;

            pthread_mutex_unlock(&mutex);

            //ESCRIBIMOS EN EL TASKS.CONF LO ACTUAL
            guardar_tareas_en_archivo();

            return 0;
        }
    }
    pthread_mutex_unlock(&mutex);

    return -1;
}


void scheduler_list_task(char *buffer, size_t size)
{
    //(void)req;

    pthread_mutex_lock(&mutex);
    //LIMPIAMOS EL BUFFER POR SI TIENE BASURA
    buffer[0] = '\0';

    for(int i = 0; i < MAX_CL; i++)
    {
        if(lista_tareas[i].id == -1)
        {
            continue;
        }
        char temp[256];


        snprintf(temp, sizeof(temp),
            "----------------------\n"
            "ID: %d\n"
            "CMD: %s\n"
            "Intervalo: %d\n"
            "Estado: %s\n"
            "PID: %d\n",
            lista_tareas[i].id,
            lista_tareas[i].cmd,
            lista_tareas[i].intervalo,
            state_to_text(lista_tareas[i].estado),
            lista_tareas[i].pid
            
        );
        
        fflush(stdout);
        strncat(buffer, temp, size - strlen(buffer) - 1);
    }
    printf("--------------------------------------------\n");


    pthread_mutex_unlock(&mutex);
}



const char* state_to_text(TaskStatus estado)
{
    switch(estado)
    {
        case ESTADO_ESPERANDO:
            return "ESPERANDO";

        case ESTADO_RUNNING:
            return "RUNNING";

        case ESTADO_ERROR:
            return "ESTADO_ERROR";

        default:
            return "DESCONOCIDO";
    }
}



void scheduler_run_task(Request *req)
{
    pthread_mutex_lock(&mutex);

    for(int i = 0; i < MAX_CL; i++)
    {
        if(lista_tareas[i].id != -1 &&
           lista_tareas[i].id == req->task_id)
        {
            printf("TAREA -----> %s\n",
                   lista_tareas[i].cmd);

            printf("Ejecutando Tarea con ID: %d\n",
                   lista_tareas[i].id);
            system(lista_tareas[i].cmd);

            lista_tareas[i].estado = ESTADO_RUNNING;

            fflush(stdout);
            break;
        }
    }

    pthread_mutex_unlock(&mutex);
} 

void scheduler_run_task_stream(Request *req, int cli_fd)
{
    char cmd_buf[M_BUFF_CMD + 32] = {0};
    int found = 0;

    pthread_mutex_lock(&mutex);
    for(int i = 0; i < MAX_CL; i++)
    {
        if(lista_tareas[i].id != -1 && lista_tareas[i].id == req->task_id)
        {
            snprintf(cmd_buf, sizeof(cmd_buf), "%s 2>&1", lista_tareas[i].cmd);
            lista_tareas[i].estado = ESTADO_RUNNING;
            lista_tareas[i].last_run = time(NULL);
            found = 1;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);

    if(!found)
    {
        send_cliente(cli_fd, -1, "[ERROR] Tarea no encontrada.\n");
        return;
    }

    FILE *fp = popen(cmd_buf, "r");
    if(!fp)
    {
        send_cliente(cli_fd, -1, "[ERROR] No se pudo iniciar la tarea.\n");
        return;
    }

    char buff[M_BUFF_S_RESPONSE] = {0};
    while(fgets(buff, sizeof(buff), fp) != NULL)
    {
        send_cliente(cli_fd, 0, buff);
        memset(buff, 0, sizeof(buff));
    }

    int rc = pclose(fp);
    char final_msg[128] = {0};

    if(rc == -1)
    {
        snprintf(final_msg, sizeof(final_msg), "[ERROR] Fallo al cerrar proceso.\n");
    }
    else
    {
        if(WIFEXITED(rc))
            snprintf(final_msg, sizeof(final_msg), "[OK] Tarea finalizada, código %d\n", WEXITSTATUS(rc));
        else
            snprintf(final_msg, sizeof(final_msg), "[OK] Tarea finalizada\n");
    }

    send_cliente(cli_fd, 0, final_msg);

    pthread_mutex_lock(&mutex);
    for(int i = 0; i < MAX_CL; i++)
    {
        if(lista_tareas[i].id != -1 && lista_tareas[i].id == req->task_id)
        {
            lista_tareas[i].estado = ESTADO_ESPERANDO;
            lista_tareas[i].pid = 0;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
}

static void send_cliente(int cli_fd, int status, const char *mensaje)
{
    Response res = {0};
    res.status = status;

    // Copio el mensaje a buffer del struct
    snprintf(res.response, sizeof(res.response), "%s", mensaje);
    //strncpy(res.response, mensaje, sizeof(res.response) - 1);
    
    // Envwio la struct completa por el socket del cliente
    if (write(cli_fd, &res, sizeof(Response)) < 0) {
        perror("[ERROR] al enviar respuesta al cliente\n");
    }
}
int scheduler_delete_task(Request *req)
{
    pthread_mutex_lock(&mutex);
    int rc = -1;
    int i = 0;


    for(i = 0;i < MAX_CL;i++)
    {
        if(lista_tareas[i].id == req->task_id)
        {
            lista_tareas[i].id = -1;
            memset(lista_tareas[i].cmd, 0, sizeof(lista_tareas[i].cmd));
            lista_tareas[i].intervalo = -1;
            lista_tareas[i].last_run = 0;
            lista_tareas[i].pid = -1;
            rc = 0;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
    
    guardar_tareas_en_archivo();

    return rc;
}
/*
typedef struct
{
    int id;
    char cmd[M_BUFF_CMD];
    int intervalo;
    time_t last_run;
    TaskStatus estado;
    pid_t pid;
} Task;
*/
int scheduler_comp_run(void)
{
    pthread_mutex_lock(&mutex);

    int resultado = -1;
    time_t ahora = time(NULL);

    for(int i = 0; i < MAX_CL; i++)
    {
        if(lista_tareas[i].id == -1)
        {
            continue;
        }
        if(lista_tareas[i].estado != ESTADO_ESPERANDO)
        {
            continue;
        }

        if((ahora - lista_tareas[i].last_run) >= lista_tareas[i].intervalo)
        {
            printf("TAREA -----> %s\n", lista_tareas[i].cmd);
            printf("Ejecutando Tarea con ID: %d\n", lista_tareas[i].id);

            lista_tareas[i].estado = ESTADO_RUNNING;
            lista_tareas[i].last_run = ahora;
            pid_t pid = fork();
            if(pid < 0)
            {
                perror("[FORK ERROR]\n");
            }
            else if(pid == 0)
            {
                printf("Ejecutando tarea\n");
                execl("/bin/sh", "sh", "-c", lista_tareas[i].cmd, (char *)NULL);
                //el comando no existe o no se encontro
                _exit(127);
            }
            printf("\n\n");
            printf("Proceso Hijo para ejecucion de comandos creado: [%d]\n",pid);

            lista_tareas[i].estado = ESTADO_ESPERANDO;
            resultado = 0;
        }
    }

    pthread_mutex_unlock(&mutex);
    return resultado;
}
void guardar_tareas_en_archivo(void)
{
    FILE *fp = fopen(TASK_CONF, "w");
    if (!fp) {
        perror("[ERROR] No se pudo escribir en tasks.conf");
        return;
    }

    for (int i = 0; i < MAX_CL; i++) 
    {
        
        if (lista_tareas[i].id != -1)
        {
            fprintf(fp, "%d:%s\n", lista_tareas[i].intervalo, lista_tareas[i].cmd);
        }
    }

    fclose(fp);
    printf("[SERVER] Archivo %s actualizado con éxito.\n", TASK_CONF);
}
