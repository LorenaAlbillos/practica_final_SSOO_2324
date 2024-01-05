#include <Stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/type.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

/*------------------VARIABLES GLOBALES------------------*/
/*Semaforos*/
pthread_mutex_t semaforoListaClientes;
pthread_mutex_t semaforoLog;
pthread_mutex_t semaforoReponedor;

/*Fichero*/
FILE *fileLong;
char logFileName[20];

/*Enteros*/
int numClientes, numSolicitudesClientes;

/*Structs*/
struct cliente {
    int ID;
    int atendido;
}

struct cliente *listaClientes;

int main(int argc, char *argv[]){
    int numCajeros = atoi(argv[1]);

    if(argc == 1){
        numCajeros = 3;
        numClientes = 20;
    } else if(argc == 3){
        numClientes = atoi(argv[1]);
        numCajeros = atoi(argv[2]);
    }else{
        perror("Numero de parametros incorrectos\n");
        exit(-1);
    }

    /*Comprobar que el numero de clientes y cajeros es mayor a 0 para continuar*/

    /*Inicializamos nuestra cola clientes*/
    listaClientes = (struct cliente *)malloc(numClientes
    *sizeof(struct cliente));

    /*Inicializar la cola*/
    for(int i = 0; i < numClientes; i++){
        cliente[i].ID = 0;
        cliente[i].atendido = 0;
    }

    /*Inicializar hilos*/
    pthread_t reponedor;

    /*Crear los hilos*/
    pthread_t create(&reponedor, NULL, reponedor, NULL);

    for(int i = 0; i < numCajeros; i++){
        pthread_t cajero;
    }

    pthread_create(&cajero, NULL, caja, NULL);

    /*Inicializar mutex*/
    pthread_mutex_init(&semaforoLog, NULL);
    pthread_mutex_init(&semaforoReponedor, NULL);
    pthread_mutex_init(&semaforoListaClientes, NULL);

    /*Crear senales*/
    struct sigaction cliente_signal = {0};

    /*Definir la manejadora de la senal*/
    cliente_signal.sa_handler = crearCliente;

    /*Armamos la senal*/
    sigaction(SIGUSR1, &cliente_signal, NULL);

    /*Esperar senal*/
    while(1){
        pause();
    }

    printf("Fin");

    return 0;
}

/*Metodos hilos*/
void *caja(void *arg){

}

void *reponedor(void *arg){

}

void *cliente(void *arg){
    
}

/*Metodo para calcular aleatorios*/
int calcularAleatorios(int min, int max){
    return rand()%(max-min + 1) + min;
}

/*Metodo para escribir en el log*/
void writeLogMessage(char *id, char *msg){
    /*Calcular la hora actual*/
    time_t hora = time(0);
    struct tm *tLocal = localtime(&hora);
    char sthora[25];
    strftime(sthora, 25, "%d/ %m/ %y  %H/: %M: %S", tLocal);

    /*Escribimos en el log*/
    fileLog = fopen("file.txt", "a");
    fprintf(fileLog, "[%s] %s: %s\n", sthora, id, msg);
    fclose(fileLog);
}