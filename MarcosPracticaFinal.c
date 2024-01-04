// Importo las librerias necesarias
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

void writeLogMessage (char *id, char *msg);
int calcularAleatorio(int min, int max);
void *Caja (void *arg);
void *Reponedor(void *arg);
void *Cliente(void *arg);
void creaCliente(int signal);

// Declaración de variables globales
pthread_mutex_t mutex_listaClientes;
pthread_mutex_t mutex_log;
pthread_mutex_t mutex_reponedor;

FILE *archivo;
int numsolicitudes;
int numClientes;

struct cliente{
    int ID;
    int atendido;
}

struct cliente *clientes;


void writeLogMessage (char *id, char *msg){
    // Calculamos la hora actual
    time_t now = time(0);
    struct tm *tlocal = localtime(&now);
    char stnow[25];
    strftime(stnow, 25, "%d/%m/%y %H:%M:%S", tlocal);

    // Escribimos en el log
    archivo = fopen("logs.txt", "a");
    fprintf(archivo, "[%s] %s: %s\n", stnow, id, msg);
    fclose(archivo);

}

int calcularAleatorio(int min, int max){
    return rand()%(max-min+1)+min;
}

int main(int argc, char *argv[]){
    int numCajeros;
    
    if (argc==1){
        numCajeros=3;
        numClientes=20;
    }else if(argc=3){
        numClientes= atoi(argv[1]);
        numCajeros = atoi(argv[2]);
    }else{
        perror("Error.");
        exit(-1);
    }
    // Comprobar que el numClientes y numcajeros es >0 para continuar
    if(numCajeros<=0 || numClientes<=0){ 
        perror("Error.");
        exit(-1);
    }
    // Inicializamos la cola de clientes
    clientes  = (struct cliente*) malloc (numClientes*sizeof(struct cliente)); 

    // Inicializar hilos
    pthread_t reponedor;
    pthread_create (&reponedor, NULL, Reponedor, NULL);   

    // Inicializamos la cola
    for(int i=0; i<numClientes;i++){
        clientes[i].ID=0;
        clientes[i].atendido=0;
    }

    for (int i=0; i<numCajeros;i++){
        pthread_t cajero;
        pthread_create(&cajero, NULL, Caja, NULL);
    }
    // Inicializamos los 3 mutex
    pthread_mutex_init(&mutex_log, NULL);
    pthread_mutex_init(&mutex_listaClientes, NULL);
    pthread_mutex_init(&mutex_reponedor, NULL);

    //Armamos las señales
    struct sigaction cliente_signal = {0};

    // Definimos el manejador de la señal
    cliente_signal.sa_handler = creaCliente;

    // Armamos la señal
    sigaction(SIGUSR1, &cliente_signal, NULL);

    //Esperamos señal
    while(1){
    pause();

    }
    printf("Fin");
    return 0;
    
}

void *Caja (void *arg){

}

void *Reponedor(void *arg){

}
void *Cliente(void *arg){
    printf("Nuevo cliente");
}
void creaCliente(int signal){
    printf("Nuevo Cliente");
}