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
pthread_cond_t avisaReponedor;

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
int getNuevoCliente();
void setAtendido(int id);


struct cliente{
    int ID;
    int atendido;
};

struct cliente *clientes;

int main(int argc, char* argv[]) {

    srand(time(NULL));
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

    //Inicializamos la variable condicion
    pthread_cond_init(&avisaReponedor,NULL);

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
    while(1) {
        //Esperamos por la señal
        //Bloqueamos el mutex
        pthread_mutex_lock(&mutexReponedor);
        //Esperamos con wait
        pthread_cond_wait(&avisaReponedor, &mutexReponedor);
        //Mensaje: Voy a mirar un precio
        writeLogMessage("Reponedor", "Voy a revisar un precio.");
        //Calculo el tiempo atencion
        int atencion = calculaAleatorios(1,5);
        //Duermo el tiempo de atencion
        char* mensaje = (char*)malloc(50*sizeof(char));
        sprintf(mensaje, "Me va a llevar comprobarlo un total de %d segundos.", atencion);
        writeLogMessage("Reponedor", mensaje);
        sleep(atencion);
        //Liberamos el mutex
        pthread_mutex_unlock(&mutexReponedor);
        writeLogMessage("Reponedor", "He terminado.");

    }
}

void *handlerCajero(void *arg) {
    //Obtenemos nuestro ID
    int id = *(int*)arg;
    int atendidos = 0;
    while(1) {
        //Bloqueamos el mutex de clientes
        pthread_mutex_lock(&mutexListaClientes);
        getNuevoCliente();
        //Desbloqueamos el mutex
        pthread_mutex_unlock(&mutexListaClientes);
        if(id != -1) {
            //Vamos a calcular el tiempo de ejecucion
            int ejecucion = calculaAleatorios(1,5);
            sleep(ejecucion);
            //Escribimos en el log que comenzamos la atencion de un cliente
            writeLogMessage(generaID(id, "Cliente"), "Comienza la atencion del cliente");
            //Comprobamos las posibilidades
            int aleatorio = calculaAleatorios(1,100);
            if(aleatorio > 70 < 95) {
                /*
                El 25 % tiene algún problema con el precio de alguno de los productos que ha comprado y es necesario que el
                reponedor vaya a comprobarlo, si está ocupado cliente y cajero deberán esperar
                */
                writeLogMessage(generaID(id, "Cajero"), "Vamos a avisar al reponedor.");
                pthread_cond_signal(&avisaReponedor);
            }else if(aleatorio <= 70) {
                //De los clientes atendidos el 70 % no tiene problemas
                writeLogMessage(generaID(id, "Cliente"), "El cliente tiene todo correcto.");
            }else {
               //El último 5% no puede realizar la compra por algún motivo (no tiene dinero, no funciona su tarjeta, etc.)
                writeLogMessage(generaID(id, "Cliente"), "El cliente ha tenido algún problema y no ha podido realizar la compra.");
            }
        }
    }
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

//Metodo que busque el primer cliente sin atender, le ponga atendido a 1 y me devuelva su id

int getNuevoCliente() {
    int id = -1;
    for(int i = 0; i < maxClientesCola; i++) {
        if(clientes[i].atendido == 0 && clientes[i].ID != 0) {
            id = clientes[i].ID;
            clientes[i].atendido = 1;
            break;
        }
    }
    return id;
}

//Metodo que reciba el id y cambie atendido a 2

void setAtendido(int id) {
    for(int i = 0; i < maxClientesCola; i++) {
        if(clientes[i].ID == id) {
            clientes[i].atendido = 2;
            break;
        }
    }
}