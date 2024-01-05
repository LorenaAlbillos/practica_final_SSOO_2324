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
FILE *archivo;
int numClientes;
int numSolicitudes;
int numCajeros;
struct cliente;
{
    int ID;
    int atendido;
};
struct cliente* clientes;

void writeLogMessage(char *id, char *msg){
    time_t now= time(0);
    struct tm *tlocal=localTime(&now) ;
    char stnow[25];
    strftime(stnow, 25, "%d/%m/%y %H :%M:%S", tlocal);
    archivo = fopen(logFileName, "a");
    fprintf(archivo, "[%s] %s: %s \n", stnow, id, msg);
    fclose(archivo);


}

int calculaAleatorios(int min, int max) {
    return rand() % (max-min+1) + min;
}


public int main(int argc, char *argv[]){
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

    clientes = (struct *cliente) malloc (numClientes*sizeof(struct cliente));
    pthread_t reponedor;
    for (int i = 0; i < numClientes; i++){
        clientes[i].ID = 0;
        clientes[i].atendido = 0;
    }
    pthread_create(&reponedor, NULL, reponedor, NULL);
    for(int i = 0; i < numCajeros; i++){
        pthread_t cajero;
        pthread_create(&cajero, NULL, caja , NULL);
    }
    pthread_mutex_init(&mutex_log, NULL);
    pthread_mutex_init(&mutex_clientes, NULL);
    pthread_mutex_init(&mutex_reponedor, NULL);
    struct sigaction cliente_signal = {0}
    cliente_signal.saHandler = creaCliente;
    sigaction(SIGURSR1, &cliente_signal, NULL);
    
    
    

    
} #include <stdio.h>
