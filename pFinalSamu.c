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
int numCajeros;
char logFileName[20];
pthread_cond_t avisaReponedor;

//Mutex globales
pthread_mutex_t mutexReponedor;
pthread_mutex_t mutexLog;
pthread_mutex_t mutexListaClientes;

void writeLogMessage(char *id, char*msg);
int calculaAleatorios(int min, int max);
void creaCliente(int signal);
int getPosicionLista(int ID);
void *handlerCajero(void *arg);
void *handlerReponedor(void *arg);
void *metodoCliente(void *arg);
char* generaID(int ID, char* tipo);
int getNuevoCliente();
void setAtendido(int id);
void eliminar(int ID);
void clientesDinamico(int signal);
void cajerosDinamico(int signal);

struct cliente{
    int ID;
    int atendido;
};

struct cliente *clientes;

int main(int argc, char* argv[]) {

    srand(time(NULL));
   
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
    writeLogMessage("*****************Main********************", "Inicio de programa.");
    printf("1. Para crear clientes ejecuta kill -10 %d\n", getpid());
    printf("2. Para aumentar el número de clientes ejecuta kill -13 %d\n", getpid());
    printf("3. Para aumentar el número de cajeros  ejecuta kill -12 %d\n", getpid());

    //Inicializamos nuestra cola de clientes
    clientes = (struct cliente*) malloc(maxClientesCola*sizeof(struct cliente));
     //Inicializamos los mutex, los 3 de la misma manera
    pthread_mutex_init(&mutexLog, NULL);
    pthread_mutex_init(&mutexReponedor, NULL);
    pthread_mutex_init(&mutexListaClientes, NULL);
        //Inicializamos la cola (Antes pthread_create)
    for(int i = 0; i < maxClientesCola; i++) {
        clientes[i].ID = 0;
        clientes[i].atendido = 0;
    }
    //Inicializar hilos

    pthread_t reponedor;
    pthread_create(&reponedor, NULL, handlerReponedor, NULL);
 
    for(int i = 0; i < numCajeros; i++) {
        pthread_t cajero;
        int *id=(int*)malloc(sizeof(int));
        *id=i;
        pthread_create(&cajero, NULL, handlerCajero, (void*)id);
    }

    //Inicializamos la variable condicion
    pthread_cond_init(&avisaReponedor,NULL);
    //Armamos las señales
    struct sigaction cliente_signal = {0};
    
    //Definimos la manejadora de la señal
    cliente_signal.sa_handler = creaCliente;
    
    //Armamos la señal 
    sigaction(SIGUSR1, &cliente_signal, NULL);

    //Asignacion dinámica
    struct sigaction masClientes = {0};
    struct sigaction masCajeros = {0};
    masClientes.sa_handler = clientesDinamico;
    masCajeros.sa_handler = cajerosDinamico;
    sigaction(SIGPIPE, &masClientes,NULL);
    sigaction(SIGUSR2, &masCajeros,NULL);
    

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
    id++;
    writeLogMessage(generaID(id, "Cajero"), "Vamos a atender");
    int atendidos = 0;
    while(1) {
        //Bloqueamos el mutex de clientes
        pthread_mutex_lock(&mutexListaClientes);
        int idC=  getNuevoCliente();
        //Desbloqueamos el mutex
        pthread_mutex_unlock(&mutexListaClientes);
        if(idC != -1) {
            //Vamos a calcular el tiempo de ejecucion
            int ejecucion = calculaAleatorios(1,5);
            sleep(ejecucion);
            //Escribimos en el log que comenzamos la atencion de un cliente
            //Comprobamos las posibilidades
            int aleatorio = calculaAleatorios(1,100);
            if(aleatorio > 70 && aleatorio<=95) {
                /*
                El 25 % tiene algún problema con el precio de alguno de los productos que ha comprado y es necesario que el
                reponedor vaya a comprobarlo, si está ocupado cliente y cajero deberán esperar
                */
                writeLogMessage(generaID(id, "Cajero"), "Vamos a avisar al reponedor.");
                pthread_cond_signal(&avisaReponedor);
                //Calculo el precio de la compra
                int precio = calculaAleatorios(1,100);
                char* mensaje = (char*)malloc(50*sizeof(char));
                sprintf(mensaje, "El precio del cliente_%d la compra es %d euros.",idC, precio);
                writeLogMessage(generaID(id, "Cajero"), mensaje);
            }else if(aleatorio <= 70) {
                //De los clientes atendidos el 70 % no tiene problemas
                char* mensajeCorreco = (char*)malloc(50*sizeof(char));
                sprintf(mensajeCorreco, "El Cliente %d , tiene todo correcto.", idC);
                writeLogMessage(generaID(id, "Cajero"), mensajeCorreco);
                int precio = calculaAleatorios(1,100);
                char* mensaje = (char*)malloc(50*sizeof(char));
                sprintf(mensaje, "El precio del Cliente_%d la compra es %d euros.",idC, precio);
                writeLogMessage(generaID(id, "Cajero"), mensaje);

            }else {
               //El último 5% no puede realizar la compra por algún motivo (no tiene dinero, no funciona su tarjeta, etc.)
                char* mensajeError = (char*)malloc(50*sizeof(char));
                sprintf(mensajeError, "El Cliente %d ha tenido algún problema y no ha podido realizar la compra.",idC);
                writeLogMessage(generaID(id, "Cajero"), mensajeError);

            }
            //Escribo en el log ha finalizado la atencion del cliente
            //Cambiamos el estado
            pthread_mutex_lock(&mutexListaClientes);
            setAtendido(idC);
            pthread_mutex_unlock(&mutexListaClientes);
            atendidos++; //Add un cliente mas atendido
            //Comprobamos descanso
            if(atendidos % 10 == 0) {
                writeLogMessage(generaID(id, "Cajero"), "Voy a tomarme un descanso.");
                sleep(20);
            }
        }
    }
}

void *metodoCliente(void *arg) {
    //Convertirlo a nuestro tipo ID (int)
    int id = *(int*)arg;
    //Ponemos un mensaje de nuevo cliente creado
    printf("Cliente ID: %d | He sido creado.\n", id);
    writeLogMessage(generaID(id,"Cliente"), "He sido creado.");

    while(1) {

        //Dormimos 10 segundos
        sleep(10);
        //Obtenemos la posicion de mi solicitud en la lista y comprobar si estoy siendo atendido(atendido == 1)
        pthread_mutex_lock(&mutexListaClientes);
        int posicion = getPosicionLista(id);
        if(clientes[posicion].atendido != 0) { //Esto significa que nos están atendiendo
            pthread_mutex_unlock(&mutexListaClientes);
            //Ponemos un mensaje en el log de que nos están atendiendo
            writeLogMessage(generaID(id,"Cliente"), "Me están atendiendo.");
            //Salimos del bucle
            break;
        }
        else {
            //Liberamos el mutex
            pthread_mutex_unlock(&mutexListaClientes);
           
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
    writeLogMessage(generaID(id, "Cliente"), "Ya he sido atendido.");
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
            numSolicitudes++;
            clientes[i].ID = numSolicitudes; //Le ponemos el siguiente al ultimo id asignado, secuencial.
            printf("Nuevo cliente.\n");
            pthread_t cliente;
            pthread_create(&cliente,NULL,metodoCliente,(void*)&clientes[i].ID);
            pthread_mutex_unlock(&mutexListaClientes);
            return;
        }
    }
    pthread_mutex_unlock(&mutexListaClientes);
    printf("No hay espacio.\n");
    writeLogMessage("Main", "No hay espacio.");
    
}

void writeLogMessage(char *id, char*msg) {
    //Bloquear mutex
    pthread_mutex_lock(&mutexLog);
    //Calculamos la hora actual
    time_t now = time(0);
    struct tm *tlocal = localtime(&now);
    char stnow[25];
    strftime(stnow, 25, "%d/%m/%y %H: %M: %S", tlocal);

    //Escribimos en el log
    archivo = fopen("app.log", "a");
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
    char* ident = (char*)malloc(20*sizeof(char));
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

void clientesDinamico(int signal) {
    int num = 0;
    printf("Introduce el nuevo número máximo de clientes: \n");
    scanf("%d" , &num);
    pthread_mutex_lock(&mutexListaClientes);
    int aux=num-maxClientesCola;
    maxClientesCola = num;
    clientes = (struct cliente*)realloc(clientes, maxClientesCola);
    for(int i = aux; i < maxClientesCola; i++) {
        clientes[i].ID=0;
        clientes[i].atendido=0;
    }
    pthread_mutex_unlock(&mutexListaClientes);

    char* msg = (char*)malloc(100*sizeof(char));
    sprintf(msg, "La lista de clientes ahora tiene un nuevo tamaño de %d clientes.",num);
    writeLogMessage("Main", msg);
}

void cajerosDinamico(int signal) {
    int num = 0;
    printf("Introduce el nuevo número de cajeros: \n");
    scanf("%d" , &num);

    for(int i = numCajeros; i < num; i++) {
        pthread_t cajero;
        int *id=(int*)malloc(sizeof(int));
        *id=i;
        pthread_create(&cajero, NULL, handlerCajero, (void*)id);
    }
    numCajeros = num;
   char* msg = (char*)malloc(100*sizeof(char));
    sprintf(msg, "La lista de cajeros ahora tiene un nuevo tamaño de %d cajeros.",num);
    writeLogMessage("Main", msg);
}