#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <semaphore.h>
#include "printRC.h"
#include <stdbool.h>

#define PI 3.1415926536
#define MIN_TIEMPO_COMER 4
#define MAX_TIEMPO_COMER 8
#define MIN_TIEMPO_PENSAR 2
#define MAX_TIEMPO_PENSAR 4
#define RETARDO 3
#define MAX_COL 50
#define MAX_REN 20
#define RADIO 10

sem_t *sPalillos;
void filosofo(void *ptr);
int A;
int N;//Numero de hilos

int main(int argc, char const *argv[]) {
    srand(time(NULL));
    if (argc != 2){
        printf("¡Ingresa un número de hilos!\n");
        exit(0);
    }
    N = atoi(argv[1]); // Obtener el número de hilos

    initPantalla();
    limpiarPantalla();
    sPalillos = malloc(sizeof(sem_t) * N); // Arreglo de semáforos
    for (int i = 0; i < N; i++)
    {
        sem_init(&sPalillos[i], 0, 1);
    }
    
    int id_filosofos[N]; // identificador de cada hilo
    pthread_t filosofos[N]; 

    A = 360 / N;//

    for (int i = 0; i < N; i++) {
        id_filosofos[i] = i;
        pthread_create(&filosofos[i], NULL, (void *) filosofo, &id_filosofos[i]);
    }
    
    for (int i = 0; i < N; i++) {
        pthread_join(filosofos[i], NULL);
    }
    
    
    /* Liberar recursos */
    for (int i = 0; i < N; i++) {
        sem_destroy(&sPalillos[i]);
    }
    free(sPalillos);
    finPantalla();
    
    return 0;
}

/* Rutina de atencion de los filosofos */
void filosofo(void *ptr){
    int id = *((int *) ptr); // Obtener el id del hilo 
    double angulo = (double)id * (A); // Calcular el ángulo correspondiente
    angulo = (angulo*PI)/180;//convertir a radianes
    /* Obtener coordenadas */
    double Xi = cos(angulo) * RADIO; 
    double Yi = sin(angulo) * RADIO;
    /* Redondear coordenadas*/
    Xi = round(Xi);
    Yi = round(Yi);

    while(true){
        // escribirR_C(MAX_REN + Yi, MAX_COL + (Xi * 2), "PENSANDO  ", AZUL);
        // sleep(MIN_TIEMPO_PENSAR + (rand() % MAX_TIEMPO_PENSAR));
        sem_wait(&sPalillos[id]);
            sleep((rand() % RETARDO) + 1);//retardo de 2 a 3 segs
            escribirR_C((MAX_REN + Yi) - 1, MAX_COL + (Xi * 2), " ![°O°]!  ", VERDE);
            escribirR_C(MAX_REN + Yi, MAX_COL + (Xi * 2), "HAMBRIENTO", VERDE);
            sem_wait(&sPalillos[(id + 1) % N]);
                //Comer
                escribirR_C((MAX_REN + Yi)-1, MAX_COL + (Xi * 2), " ~[^-^]~  ", AMARILLO);
                escribirR_C(MAX_REN + Yi, MAX_COL + (Xi * 2), "COMIENDO  ", AMARILLO);
                sleep(MIN_TIEMPO_COMER + (rand() % MAX_TIEMPO_COMER));
            sem_post(&sPalillos[id]);
        sem_post(&sPalillos[(id + 1) % N]);

        escribirR_C((MAX_REN + Yi) - 1, MAX_COL + (Xi * 2), " ¿[~_~]?  ", AZUL); 
        escribirR_C(MAX_REN + Yi, MAX_COL + (Xi * 2), "PENSANDO  ", AZUL);
        sleep(MIN_TIEMPO_PENSAR + (rand() % MAX_TIEMPO_PENSAR));
    }
        
} 