#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>




pthread_mutex_t mutex_clientes;
pthread_mutex_t mutex_log;
pthread_mutex_t mutex_reponedor;
pthread_cond_t avisaReponedor;
FILE *archivo;
int numClientes;
int numSolicitudes;
int numCajeros;
struct cliente{
    int ID;
    int atendido;
};
struct cliente* clientes;


char* generaId(int id, char*tipo);
int getPosicion(int id);
void eliminar(int id);

void writeLogMessage(char *id, char *msg){
    time_t now= time(0);
    struct tm *tlocal=localtime(&now) ;
    char stnow[25];
    strftime(stnow, 25, "%d/%m/%y %H :%M:%S", tlocal);
    //Bloquear mutex
    archivo = fopen("logs.txt", "a");
    fprintf(archivo, "[%s] %s: %s \n", stnow, id, msg);
    fclose(archivo);
    //Desbloquear mutex
}

int calculaAleatorios(int min, int max) {
    return rand() % (max-min+1) + min;
}
void *caja(void *arg){
    int id = *(int*)&arg;
    int atendido = 0;
    while(1){
        pthread_mutex_lock(&mutex_clientes);
        int idCliente = getNuevoCliente();
        pthread_mutex_unlock(&mutex_clientes);
        if(idCliente != -1){
            int atencion = calculaAleatorios(1,5);
            char* mensaje =(char*)malloc(50*sizeof(char));
            sprintf(mensaje, "El cliente %d comienza a ser atendido", id);
            writeLogMessage(generaId(id, "Cajero"), mensaje);
            sleep(atencion);
            int posibilidades = calculaAleatorios(1, 100);
            if(posibilidades <= 70){
                sprintf(mensaje, "El %d cliente tiene todo correcto", id);
                writeLogMessage(generaId(id, "cliente"),mensaje);
                int precioCompra = calculaAleatorios(1,100);
            }else if(posibilidades <= 95){
                sprintf(mensaje, "El %d cliente tiene que esperar", id);
                writeLogMessage(generaId(id, "cliente"), mensaje);
                pthread_cond_signal(&avisaReponedor);       
                int precio = calculaAleatorios(1,100);

            }else{
                sprintf(mensaje, "El %d cliente no puede realizar la compra",id);
                writeLogMessage(generaId(id, "cliente"), mensaje);

            }
            pthread_mutex_lock(&mutex_clientes);
            setAtendido(idCliente);
            pthread_mutex_unlock(&mutex_clientes);
            atendido++;
            if(atendido % 10 == 0){
                sprintf(mensaje, " %d me voy a descansar", id);
                writeLogMessage(generaId(id, "cliente"),mensaje);

                sleep(20);
            }
        }
    }
}
void *reponedor(void *arg){
    while(1){
        pthread_mutex_lock(&mutex_clientes);
        pthread_cond_wait(&avisaReponedor, &mutex_reponedor);
        int atencion = calculaAleatorios(1,5);
        sleep(atencion);
        pthread_mutex_unlock(&mutex_clientes); 
    }
}
void *AccionesCliente(void *arg){
int id=*(int*)arg;
while(1){
    pthread_mutex_lock(&mutex_clientes);
    int posicion = getPosicion(id);
    if(clientes[posicion].atendido == 1){
        pthread_mutex_unlock(&mutex_clientes);
        writeLogMessage(generaId(id, "cliente"), "Me estan antendiendo");
        break;
    }else{
        pthread_mutex_unlock(&mutex_clientes);
        sleep(10);
        int aleatorio = calculaAleatorios(1,100);
        if(aleatorio <= 10){
            pthread_mutex_lock(&mutex_clientes);
            eliminar(id);
            pthread_mutex_unlock(&mutex_clientes);
            writeLogMessage(generaId(id, "cliente"), "Me voy, me canse de esperar");
            pthread_exit(NULL);
        }
    }
}
while(1){
    pthread_mutex_lock(&mutex_clientes);
    int posicion = getPosicion(id);
    if(clientes[posicion].atendido == 2){
        pthread_mutex_lock(&mutex_clientes);
        break;
    }else{
    pthread_mutex_unlock(&mutex_clientes);
    }
    writeLogMessage(generaId(id, "cliente"), "Me han antendiendo");
    eliminar(id);
    pthread_exit(NULL);


}
}
char* generaId(int id, char*tipo){
    char *ident = (char* )malloc(20*sizeof(char));
    sprintf(ident, "%s_%d", tipo, id);
    return ident;

}
int getPosicion(int id){
    for(int i = 0; i < numClientes; i++){
        if(clientes[i].ID == id){
            return i;
        }
    }
    return -1;

}
void setAtendido(int id){
    for(int i = 0; i < numClientes; i++){
        if(clientes[i].ID == id){
            clientes[i].atendido = 2;
            break;
        }
    }
}
int getNuevoCliente(){
    for(int i = 0; i < numClientes; i++){
        if(clientes[i].atendido == 0 && clientes[i].ID !=0){
            clientes[i].atendido = 1;
            return clientes[i].ID;
        }
    }
    return 1;
}
void eliminar(int id){
    int posicion = getPosicion(id);
    for(int i = posicion; i < numClientes;i++ ){
        clientes[i].ID = clientes[i + 1].ID;
        clientes[i].atendido = clientes[i + 1].atendido;
        clientes[numClientes - 1].ID = 0;
        clientes[numClientes].atendido = 0;

    }
}

void creaCliente(int signal){
    printf("Cliente Nuevo\n");
    pthread_mutex_lock(&mutex_clientes);
     for (int i = 0; i < numClientes; i++){
        if(clientes[i].ID == 0){
            clientes[i].ID = numSolicitudes;
            numSolicitudes++;
            pthread_t cliente;
            pthread_create(&cliente, NULL, AccionesCliente , (void*)&clientes[i].ID);
                pthread_mutex_unlock(&mutex_clientes);

            return ;
        }
    }

     pthread_mutex_unlock(&mutex_clientes);

}
void incrementaClientes(int signal){
    printf("Introduce cuantos clientes quieres aumentar");
    int num = 0 ;
    scanf("%d", clientes);
    numClientes += num;
    clientes = (struct cliente*)realloc(clientes, numClientes);
    pthread_mutex_unlock(&mutex_clientes);
        writeLogMessage("Main", "La lista de clientes se ha aumentado");

}
void incrementaCajeros(int signal){
    printf("Introduce cuantos cajeros quieres aumentar");
    int num = 0 ;
    scanf("%d", clientes);
   for(int i = 0 ; i <num; i++){
    pthread_t thread;
    int* ident = (int*)malloc(sizeof(int));
    *ident =i + numCajeros+1;
    pthread_create(&thread, NULL, cajero, (void*)ident)
   }
   writeLogMessage("Main", "La lista de cajeros se ha aumentado");
}


int main(int argc, char *argv[]){
    struct sigaction masclientes = {0};
    masclientes.sa_handler = incrementaClientes();
    sigaction(SIGUSR2,&masclientes,NULL)
   if(argc == 1){
    numClientes = 20;
    numCajeros = 3;
   }else if(argc == 3){
    numClientes = atoi(argv[1]);
    numCajeros = atoi(argv[2]);

   }else{
    perror("Error");
    exit(-1);
   }
   srand(time(NULL));
    pthread_cond_init(&avisaReponedor, NULL);
    printf("PID %d\n", getpid());
    clientes = (struct cliente*) malloc (numClientes*sizeof(struct cliente));
    pthread_t repo;
    for (int i = 0; i < numClientes; i++){
        clientes[i].ID = 0;
        clientes[i].atendido = 0;
    }
    pthread_create(&repo, NULL, reponedor, NULL);
    for(int i = 0; i < numCajeros; i++){
        pthread_t cajero;
        pthread_create(&cajero, NULL, caja , NULL);
    }
    pthread_mutex_init(&mutex_log, NULL);
    pthread_mutex_init(&mutex_clientes, NULL);
    pthread_mutex_init(&mutex_reponedor, NULL);
    struct sigaction cliente_signal = {0};
    cliente_signal.sa_handler = creaCliente;
    sigaction(SIGUSR1, &cliente_signal, NULL);
    while(1){
        pause();
    }
    printf("Fin");
    return 0;
} 