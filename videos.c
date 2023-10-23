#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#define T 4     //T = Número de nucleos del CPU

struct Tarea{
    char nombre[100];
    struct Tarea* sig;
}typedef Tarea;

struct Cola{
    Tarea* Cabeza;
}typedef Cola;

Cola* Trabajos;
sem_t lugares;
sem_t productos;
pthread_mutex_t mutex;

Tarea* crear_tarea(char* name){
    Tarea* aux = (Tarea*)malloc(sizeof(Tarea));
    strcpy(aux->nombre, name);
    aux->sig=NULL;
    return aux;
}

Cola* crear_cola(){
    Cola* c = (Cola*)malloc(sizeof(Cola));
    c->Cabeza = NULL;
    return c;
}

void queue(Cola* Cola, Tarea* aux){
    Tarea* cola;
    cola = Cola->Cabeza;
    if(cola == NULL){
        Cola->Cabeza = aux;
    }else{
        while(cola->sig != NULL){
            cola = cola->sig;
        }
        cola->sig = aux;
        cola = aux;
    }
}

Tarea* deque(Cola* Cola){
    if(Cola->Cabeza == NULL)
        return NULL;
    Tarea* aux = Cola->Cabeza;
    Cola->Cabeza = Cola->Cabeza->sig;
    return aux;
}

void descargar_videos(){
    Tarea* aux;
    char comando[100];
    while(1){
        sem_wait(&productos);
        pthread_mutex_lock(&mutex);
        aux = deque(Trabajos);
        pthread_mutex_unlock(&mutex);
        sem_post(&lugares);
        //Si la cadena es detener_sistema, se realiza la insercion de la cadena
        //nuevamente y se detiene la ejecución de la función.
        //La inserción en este caso es idéntica a la que realiza el hilo principal
        if(!strcmp(aux->nombre, "detener_sistema")){
            printf("Hilo termina su ejecución\n");
            fflush(stdout);
            sem_wait(&lugares);
            pthread_mutex_lock(&mutex);
            queue(Trabajos, aux);
            pthread_mutex_unlock(&mutex);
            sem_post(&productos);
            break;
        }else{
            printf("Un hilo comienza a procesar %s\n", aux->nombre);
            fflush(stdout);
            //Generar el comando
            strcpy(comando, "cd ./Videos && youtube-dl -f best ");
            strcat(comando, aux->nombre);
            //Enviar el comando
            system(comando);
            printf("Termina de procesarse %s\n",aux->nombre);
            fflush(stdout);
            free(aux);
        }
    }
}

//Comando para recuperar lista de URLs de una playlist
//youtube-dl -e -j --flat-playlist 'playlist' | cut -d " " -f 8 | awk -F'"' '{print $2}' | sed 's_^_https://youtube.com/watch?v=_'

int main(){
    //El número de lugares disponibles comienza en T
    sem_init(&lugares, 0, T);
    //El número de productos en el buffer comienza en 0
    sem_init(&productos, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    srand(time(NULL));

    char video[100][100];
    int archivo_actual = -1;

    Trabajos = crear_cola();

    //Leer archivo
    FILE* archivo = fopen("videos.txt", "r+");
        do{
        archivo_actual++;
        fscanf(archivo, "%s\n", video[archivo_actual]);
    }while(strcmp(video[archivo_actual], "detener_sistema"));

    system("mkdir ./Videos");

    pthread_t Hilos[T];

    //Crear Hilos
    for(int i = 0; i < T; i++){
        pthread_create(&Hilos[i], NULL, (void *) descargar_videos, NULL);
    }

    archivo_actual = 0;

    //Ciclo principal del productor
    while(1){
        //sleep(rand() % 31);
        sem_wait(&lugares);
        pthread_mutex_lock(&mutex);
        queue(Trabajos, crear_tarea(video[archivo_actual]));
        pthread_mutex_unlock(&mutex);
        sem_post(&productos);
        //Si se alcanza detener_sistema, se detiene el programa
        if(strcmp(video[archivo_actual], "detener_sistema") == 0){
            break;
        }
        archivo_actual++;
    }
    
    //Esperar la terminación de los hilos
    for(int i = 0; i < T; i++){
        pthread_join(Hilos[i], NULL);
    }

    free(Trabajos);
    fclose(archivo);
    return 0;
}