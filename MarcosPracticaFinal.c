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
char intoChar(int id);

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
    // Obtenemos el id
    int id  =*(int*) arg;
    int atendidos = 0;
    while (1){
        // Bloqueamos mutex clientes
        pthread_mutex_lock(&mutex_listaClientes);
        // Selecciono de la lista
        int idCliente = getNuevoCliente();
        // Desbloqueo mutex clientes
        pthread_mutex_unlock(&mutex_listaClientes);

        if(idCliente!=-1){
            int atencion = calcularAleatorio(1, 5);
        
            // Escribimos ue empezamos la atencion de un cliente
            char *mensaje = (char *)malloc (50* sizeOf(char));
            sprintf(mensaje, "El %s cliente comienza a ser atendido", intoChar(id));
            writeLogMessage(intoChar(id), "El %s cliente comienza a ser atendido");
            //Dormimos
            sleep(atencion);
            //Comprobamos las posibilidades
            int posibilidades= calcularAleatorio(1, 100);
            if(posibilidades<=70){
                sprintf(mensaje, "El %s cliente tiene todo correcto", intoChar(id));
                writeLogMessage(intoChar(id), "El %s cliente tiene todo correcto");
                // Calculo precio compra
                int precio = calcularAleatorio(1, 100);
            }else if(posibilidades<=95){
                sprintf(mensaje, "El %s cliente tiene que esperar", intoChar(id));
                writeLogMessage(intoChar(id), "El %s cliente tiene que esperar");
                // Calculo precio compra
                int precio = calcularAleatorio(1, 100);
            }else{
                sprintf(mensaje, "El %s cliente no puede realizar la compra", intoChar(id));
                writeLogMessage(intoChar(id), "El %s cliente no puede realizar la compra");
            }
            //Cambio estado
            pthread_mutex_lock(&mutex_listaClientes);
            setAtendido(idCliente);
            pthread_mutex_unlock(&mutex_listaClientes);
            atendidos++;
            if(atendidos%10==0){
                sprintf(mensaje, "me voy a descansar", intoChar(id));
                writeLogMessage(intoChar(id), "Me voy a descansar");

                sleep(20);
            }
        }
    }
}
char * getNuevoCliente(){
    for (int i=0; i<numClientes; i++){
        if ( clientes[i].atendido==0 && clientes[i].ID!=0){
            clientes[i].atendido=1;
            return clientes[i].ID;
        }
    }
    return -1;
}

void setAtendido(int id){
     for (int i=0; i<numClientes; i++){
        if ( clientes[i].ID==id){
            clientes[i].atendido=2;
            break;
        }
    }
}
void *Reponedor(void *arg){

}
void *accionCliente(void *arg){ // LLeva el * por que recibe los hilos recien creados
    // Primero convertimos a nuestro tipo ID
    int ID = (int*)&arg;    // Casteo para convertir a ID
    
    while(1){
        // Bloqueo el mutex
        pthread_mutex_lock(&mutex_listaClientes);
        int posicion = getPosicion(ID);
        if (clientes[posicion].atendido==1){
            //Ya no estan atendiendo
            pthread_mutex_unlock(&mutex_listaClientes);
            // salgo
            break;
        }
        pthread_mutex_unlock(&mutex_listaClientes);
        writeLogMessage(intoChar(ID), "No me estan atendiendo");
        // Duermo 10 secs 
        sleep(10);
        // Compruebo si me voy de la cola(10%)
        if(irse<10){
            //Bloquea mutex clientes
            pthread_mutex_lock(&mutex_listaClientes);
            //Eliminamos clientes
            eliminarCliente(ID);
            //Desbloqueo  M¡mutex
            pthread_mutex_unlock(&mutex_listaClientes);
            //Escribo mensaje
            //Nos morimos
            pthread_exit(NULL);
        }
    }

    // Mensaje me estoy atendiendo.
    while(2){   // Espero mientras me atienden
        // Bloqueo el mutex.
        pthread_mutex_lock(&mutex_listaClientes);
        int posicion = getPosicion(ID);
        // Si me han atendido -> salgo.
        if (clientes[posicion].atendido==0){
            eliminarCliente(ID);
            pthread_mutex_unlock(&mutex_listaClientes);
            break;
        }
        pthread_mutex_unlock(&mutex_listaClientes);

        // Si no espero.
    }
    //Mensaje de ha termindao mi atención.
    printf("Ha terminado mi asistencia y me voy");
}

char* getID(int id){
    char* identificador = (char)malloc(15*sizeof(char));
    sprintf(identificador, "Cliente_%d", id, tipo, id);
    return identificador;
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
/**
 * Metodo que pasa un dato de tipo int a uno de tipo char
 * @param id 
 * @return char 
*/
char intoChar(int id){
    return (char)id;
}
