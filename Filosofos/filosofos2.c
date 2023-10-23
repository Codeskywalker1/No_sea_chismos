#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <semaphore.h>
#include "printRC.h"
#include <stdbool.h>

/* Estados */

#define PI 3.1415926536
#define MIN_TIEMPO_COMER 4
#define MAX_TIEMPO_COMER 8
#define MIN_TIEMPO_PENSAR 2
#define MAX_TIEMPO_PENSAR 3
#define RETARDO 2
#define MAX_COL 50
#define MAX_REN 20
#define PENSANDO 1
#define HAMBRIENTO 2
#define COMIENDO 3
#define RADIO 8

sem_t *parLibre; // Semaforo donde los filosofos esperan por una par de palillos libres
pthread_mutex_t mEstados;
int *estados; // Arreglo de los estados de cada filosofo
int N; // Número de hilos
int A;
void filosofo(void *ptr);

int main(int argc, char const *argv[]) {
    if(argc != 2){
        printf("¡Ingresa un número de hilos!\n");
        exit(0);
    }

    N = atoi(argv[1]);
    srand(time(NULL));
    limpiarPantalla();
    initPantalla();
    A = 360 / N;
    /* Inicializar y reservar espacio */
    parLibre = malloc(sizeof(sem_t) * N);
    estados = malloc(sizeof(int) * N);
    pthread_mutex_init(&mEstados, NULL);

    for (int i = 0; i < N; i++) {
        sem_init(&parLibre[i], 0, 0); //No hay palillos libres si izq y der estan comiendo
        estados[i] = PENSANDO; // Inicializar el estado de los filosofos en PENSANDO
    }
    // Crear a los filosofos
    pthread_t filosofos[N];
    int id_filosofos[N];

    for (int i = 0; i < N; i++) {
        id_filosofos[i] = i;
        pthread_create(&filosofos[i], NULL, (void *) filosofo, &id_filosofos[i]);
    }
    
    /* Esperar a los filosofos */
    for(int i = 0; i < N; i++){
        pthread_join(filosofos[i], NULL);
    }

    /* Liberar recursos */
    pthread_mutex_destroy(&mEstados);
    for(int i = 0; i < N; i++){
        sem_destroy(&parLibre[i]);
    }
    free(parLibre);
    free(estados);
    finPantalla();
    return 0;
}


void filosofo(void *ptr){
    // perror("OMG\n");
    int id = *((int *) ptr);
    double angulo = (double)id * A;
    angulo = (angulo*PI)/180;//convertir a radianes
    double Xi = cos(angulo) * RADIO;
    double Yi = sin(angulo) * RADIO;
    
    /* Redondear coordenadas*/
    Xi = round(Xi);
    Yi = round(Yi);

    while (true){
        /* Pensar */

        escribirR_C((MAX_REN + Yi) - 1, MAX_COL + (Xi * 2), " ¿[~_~]?  ", AZUL); 
        escribirR_C(MAX_REN + Yi, MAX_COL + (Xi * 2), "PENSANDO  ", AZUL);
        sleep(MIN_TIEMPO_PENSAR + (rand() % MAX_TIEMPO_PENSAR));
        
        pthread_mutex_lock(&mEstados);// Candado para modificar los estados
        estados[id] = HAMBRIENTO;

        escribirR_C((MAX_REN + Yi) - 1, MAX_COL + (Xi * 2), " ![°O°]!  ", VERDE);
        escribirR_C(MAX_REN + Yi, MAX_COL + (Xi * 2), "HAMBRIENTO  ", VERDE);
        // Preguntar si mis vecinos der., izq. NO estan comiendo    
        if ((estados[(id + 1) % N] != COMIENDO) && (estados[(id + N-1) % N] != COMIENDO)){
            estados[id] = COMIENDO;//Puedo comer
            pthread_mutex_unlock(&mEstados);
        } else { // Los palillos estan ocupados
            pthread_mutex_unlock(&mEstados); // Liberar candado antes de esperar por palillos
            sem_wait(&parLibre[id]); // Sigue hambriento pero no retiene los palillos
        }

        /* COMER */
        // Retener palillos y mostrar comer

        escribirR_C((MAX_REN + Yi)-1, MAX_COL + (Xi * 2), " ~[^-^]~  ", AMARILLO);
        escribirR_C(MAX_REN + Yi, MAX_COL + (Xi * 2), "COMIENDO  ", AMARILLO);
        sleep(MIN_TIEMPO_COMER + (rand() % MAX_TIEMPO_COMER));

       /* Despertar vecinos */ 
        // Hay que usar candado para usar el arreglo de estados y modificarlo
        pthread_mutex_lock(&mEstados);
        estados[id] = PENSANDO;
        // Verificar si el vecidono der. tiene hambre y su vecino no esta comiendo
        if((estados[(id + 1) % N] == HAMBRIENTO) && (estados[(id + 2) % N] != COMIENDO)){
            //Establecer el estado del vecino der. en COMIENDO
            estados[(id + 1) % N] = COMIENDO;
            // Despertar al vecino der. para que pueda comer
            sem_post(&parLibre[(id + 1) % N]);
        } 
        // Verificar si el vecidono izq. tiene hambre y su vecino no esta comiendo
        else if((estados[(id + N-1) % N] == HAMBRIENTO) && (estados[(id + N-2) % N] != COMIENDO)){
            //Establecer el estado del vecino izq. en COMIENDO
            estados[(id + N-1) % N] = COMIENDO;
            // Despertar al vecino der. para que pueda comer
            sem_post(&parLibre[(id + N-1) % N]);
        } 
        pthread_mutex_unlock(&mEstados);
    }
}