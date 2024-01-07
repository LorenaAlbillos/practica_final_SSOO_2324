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

void writeLogMessage(int *id, char*msg);
int calculaAleatorios();
int getPosicionLista(int ID);
void *handlerCajero(void *arg);
void *handlerReponedor(void *arg);
void *metodoCliente(void *arg);
char* generaID(int ID, char* tipo);


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
    printf("Fin");
    return 0;
} //Fin del main

//Creamos los métodos hilos

void *handlerReponedor(void *arg) {

}

void *handlerCajero(void *arg) {

}

void *metodoCliente(void *arg) {
    //Convertirlo a nuestro tipo ID (int)
    int id = *(int*)arg;
    //Ponemos un mensaje de nuevo cliente creado
    printf("Cliente ID: %d | He sido creado.", id);
    while(1) {
        //Obtenemos la posicion de mi solicitud en la lista y comprobar si estoy siendo atendido(atendido == 1)
        pthread_mutex_lock(&mutexListaClientes);
        int posicion = getPosicionLista(id);
        if(clientes[posicion].atendido == 1) { //Esto significa que nos están atendiendo
            pthread_mutex_unlock(&mutexListaClientes);
            //Ponemos un mensaje en el log de que nos están atendiendo
            writeLogMessage(generaID(id,"Cliente"), "Me están atendiendo.");
            //Salimos del bucle
            break;
        }
        else {
            //Liberamos el mutex
            pthread_mutex_unlock(&mutexListaClientes);
            //Dormimos 10 segundos
            sleep(10);
            //Comprobamos si nos tenemos que ir (10%)
            int num = calculaAleatorios(10,100);
                if(num <= 10) {
                    //El cliente abandona el establecimiento
                    pthread_mutex_lock(&mutexListaClientes);
                    eliminar(id);
                    pthread_mutex_unlock(&mutexListaClientes);
                    //Escribimos en el log que el cliente se ha cansado de esperar y abandona el establecimiento
                    writeLogMessage(generaID(id, "Cliente"), "Me he cansado de esperar y abandono el establecimiento.");
                    pthread_exit(NULL);
                }
        }
    }
    //Significa que nos están atendiendo
    while(1) {
        //Bloqueamos el mutex
        pthread_mutex_lock(&mutexListaClientes);
        int pos = getPosicionLista(id);
        if(clientes[pos].atendido == 2) {         //Compruebo si ya me han atendido
            //Desbloqueamos el mutex
            pthread_mutex_unlock(&mutexListaClientes);
            //Si me han atendido me salgo
            break;    
        }else {
            pthread_mutex_unlock(&mutexListaClientes);
        }
    }
    //Pongo un mensaje de que me han atendido
    writeLogMessage(generaID(id, "Cliente"), " Ya he sido atendido.");
    //Me borro de la lista,el cliente abandona el establecimiento
    pthread_mutex_lock(&mutexListaClientes);
    eliminar(id);
    pthread_mutex_unlock(&mutexListaClientes);
    pthread_exit(NULL);
}

void creaCliente(int signal) {
    //Comprobar si hay sitio en la fila
    pthread_mutex_lock(&mutexListaClientes);
    for(int i = 0; i < maxClientesCola; i++) {
        if(clientes[i].ID == 0) {
            clientes[i].ID = numSolicitudes; //Le ponemos el siguiente al ultimo id asignado, secuencial.
            numSolicitudes++;
            printf("Nuevo cliente");
            pthread_t cliente;
            pthread_create(&cliente,NULL,metodoCliente,(void*)clientes[i].ID);
            pthread_mutex_unlock(&mutexListaClientes);
            return;
        }
    }
    pthread_mutex_unlock(&mutexListaClientes);
    printf("No hay espacio.");
}

void writeLogMessage(int *id, char*msg) {
    //Bloquear mutex
    pthread_mutex_lock(&mutexLog);
    //Calculamos la hora actual
    time_t now = time(0);
    struct tm *tlocal = localtime(&now);
    char stnow[25];
    strftime(stnow, 25, "%d/%m/%y %H: %M: %S", tlocal);

    //Escribimos en el log
    archivo = fopen(logFileName, "a");
    fprintf(archivo, "[%s] %s: %s\n", stnow, id, msg);
    fclose(archivo);
    //Liberar 
    pthread_mutex_unlock(&mutexLog);
}

int calculaAleatorios(int min, int max) {
    return rand()%(max-min+1) + min;
}

int getPosicionLista(int ID) {
    for(int i = 0; i < maxClientesCola; i++) {
        if(clientes[i].ID == ID) {
            return i;
        }
    }
    return -1;
}

void eliminar(int ID) {
    int posicion = getPosicionLista(ID);
    for(int i = posicion; i < maxClientesCola-1; i++) {
        clientes[i].ID = clientes[i+1].ID;
        clientes[i].atendido = clientes[i+1].atendido;
    }
    clientes[maxClientesCola-1].ID = 0;
    clientes[maxClientesCola-1].atendido = 0;
}

char* generaID(int ID, char* tipo) {
    char* ident = (char)malloc(20*sizeof(char));
    sprintf(ident,"%s_%d" ,tipo,ID);
    return ident;
}