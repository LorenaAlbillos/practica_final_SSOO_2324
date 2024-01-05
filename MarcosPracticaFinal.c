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
void *accionCliente(void *arg);
void creaCliente(int signal);
int getPosicion(int *id);
void eliminar ( int id);

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
};

struct cliente *clientes; // Esto es un puntero de tipo struct cliente

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
    // Compruebo que haya Cajeros y Clientes
    if(numCajeros<=0 || numClientes<=0){ 
        perror("Error.");
        exit(-1);
    }
    // Inicializo la cola de clientes
    clientes  = (struct cliente*) malloc (numClientes*sizeof(struct cliente)); 

    // Inicializo los hilos
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
    // Inicializo los 3 mutex.
    pthread_mutex_init(&mutex_log, NULL);
    pthread_mutex_init(&mutex_listaClientes, NULL);
    pthread_mutex_init(&mutex_reponedor, NULL);

    //Armamo la señal y defino su manejador
    struct sigaction cliente_signal = {0};
    cliente_signal.sa_handler = creaCliente;

    // Armamo la señal
    sigaction(SIGUSR1, &cliente_signal, NULL);

    //Espero a la señal
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
void *accionCliente(void *arg){ // LLeva el * por que recibe los hilos recien creados
    // Primero convertimos a nuestro tipo ID
    int ID = (int*)&arg;    // Casteo para convertir a ID
    while(1){
        // Obtengo la posicion de la solicitud en la lista 
        // Compruebo si estoy atendido
    }
}
/**
 * Metodo que crea nuevos clientes si hay sitio en la cola
 * @param signal
*/
void creaCliente(int signal){
    // Compruebo si hay sitio en la cola
    for (int i = 0; i < numClientes; i++){
        if (clientes[i].ID==0){
            clientes[i].ID = numsolicitudes;
            numsolicitudes++;

            pthread_t cliente;
            pthread_create(&cliente, NULL, accionCliente, (void*)clientes[numsolicitudes-1]);
        }
        return;
    }
}
/**
 * Metodo que escribe los mensajes del log
 * @param id  id
 * @param msg mensaje 
*/
void writeLogMessage (char *id, char *msg){
    // Calculo la hora actual
    time_t now = time(0);
    struct tm *tlocal = localtime(&now);
    char stnow[25];
    strftime(stnow, 25, "%d/%m/%y %H:%M:%S", tlocal);

    // Escribo en el log
    archivo = fopen("logs.txt", "a");
    fprintf(archivo, "[%s] %s: %s\n", stnow, id, msg);
    fclose(archivo);

}
/**
 * Metodo que calcula un número aleatorio entre dos numeros
 * @param min numero minimo
 * @param max numero maximo
*/
int calcularAleatorio(int min, int max){
    return rand()%(max-min+1)+min;
}
/**
 * Metodo que devuelve la posicion de un cliente en la cola.
 * @param id del cliente.
*/
int getPosicion(int *id){
    for(int i = 0; i < numClientes; i++){
        if(clientes[i].ID==id){ // Si coincide lo encontramos
        return i;   // Devolvemos la pos
        }
    }
    return -1;
}
/**
 * Metodo que elimina un cliente de la cola.
 * @param id del cliente.
*/
void eliminar ( int id){
    int posicion = getPosicion(id);
    for (int i = posicion; i < numClientes-1; i++){
        clientes[i].ID=clientes[i+1].ID;
        clientes[i].atendido=clientes[i+1].atendido;
    }
    clientes[numClientes-1].ID=0;
    clientes[numClientes-1].atendido=0;
}
