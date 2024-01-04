//Includes
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

FILE *archivo;

//Variables globales
int maxClientesCola;
int numSolicitudes = 0;
char logFileName[20];

//Mutex globales
pthread_mutex_t mutexReponedor;
pthread_mutex_t mutexLog;
pthread_mutex_t mutexListaClientes;

void writeLogMessage(int id, char*msg);
int calculaAleatorios();

struct cliente{
    int ID;
    int atendido;
};

struct cliente *clientes;

int main(int argc, char* argv[]) {
    int numCajeros;
    if(argc == 1) {
        maxClientesCola = 20;
        numCajeros = 3;
    }else if(argc == 3) {
        maxClientesCola = atoi(argv[1]);
        numCajeros = atoi(argv[2]);
    }else {
        perror("Error al introducir los parámetros.");
        exit(-1);
    }

    //Comprobar que el numero de clientes y de cajeros es > 0 para continuar
    if(maxClientesCola < 0 || numCajeros < 0) {
        perror("Error: Estos valores no pueden ser negativos.");
        exit(-1);
    }
    //Inicializamos nuestra cola de clientes
    clientes = (struct cliente*) malloc(maxClientesCola*sizeof(struct cliente));
    //Inicializar hilos
    pthread_t reponedor;
    pthread_create(&reponedor, NULL, handlerReponedor, NULL);
    for(int i = 0; i < maxClientesCola; i++) {
        pthread_t cliente;
    }
    for(int i = 0; i < numCajeros; i++) {
        pthread_t cajero;
        pthread_create(&cajero, NULL, handlerCajero, NULL);
    }
    //Inicializamos los mutex, los 3 de la misma manera
    pthread_mutex_init(&mutexLog, NULL);
    pthread_mutex_init(&mutexReponedor, NULL);
    pthread_mutex_init(&mutexListaClientes, NULL);
    //Armamos las señales
    struct sigaction cliente_signal = {0};
    //Definimos la manejadora de la señal
    cliente_signal.sa_handler = metodoCliente;
    //Armamos la señal 
    sigaction(SIGUSR1, &cliente_signal, NULL);
    //Inicializamos la cola (Antes pthread_create)
    for(int i = 0; i < maxClientesCola; i++) {
        clientes[i].ID = 0;
        clientes[i].atendido = 0;
    }
    //Esperar señal
    while(1) {
        pause();
    }
    pritnf("Fin");
    return 0;
} //Fin del main

//Creamos los métodos hilos

void *handlerReponedor(void *arg) {

}

void *handlerCajero(void *arg) {

}

void *metodoCliente(void *arg) {

}

void creaCliente(int signal) {
    printf("Nuevo cliente");
}

void writeLogMessage(int id, char*msg) {
    //Calculamos la hora actual
    time_t now = time(0);
    struct tm *tlocal = localtime(&now);
    char stnow[25];
    strftime(stnow, 25, "%d/%m/%y %H: %M: %S", tlocal);

    //Escribimos en el log
    archivo = fopen(logFileName, "a");
    fprintf(archivo, "[%s] %s: %s\n", stnow, id, msg);
    fclose(archivo);
}

int calculaAleatorios(int min, int max) {
    return rand()%(max-min+1) + min;
}