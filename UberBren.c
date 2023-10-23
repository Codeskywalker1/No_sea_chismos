#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define UBERBLACK 1
#define UBERX 2

typedef struct {
   char destino;
   int tiempo;
   int tipoUber;
} Producto;

typedef struct {
   Producto* elementos;
   int capacidad;
   int frente;
   int final;
} Buffer;

#define MAX_PETICIONES 3
Buffer bufPeticiones;

sem_t lleno;
sem_t vacio;
pthread_mutex_t mutex;
sem_t observadorSem;

int gananciaTotal = 0;

int aleatorioEn(int minVal, int maxVal) {
   return (minVal + rand() % (maxVal - minVal + 1));
}

void creaBuffer(Buffer* buf, int tam) {
   buf->capacidad = tam;
   buf->elementos = (Producto*)malloc(tam * sizeof(Producto));
   buf->frente = -1;
   buf->final = -1;
}

void destruyeBuffer(Buffer* buf) {
   free(buf->elementos);
}

static inline bool bufLleno(Buffer* b) {
   int next = (b->final + 1) % b->capacidad;
   if (b->frente == next)
      return true;
   else
      return false;
}

static inline bool bufVacio(Buffer* b) {
   if (b->frente == -1)
      return true;
   else
      return false;
}

static inline void meterProducto(Buffer* b, Producto p) {
   if (!bufLleno(b)) {
      b->final = (b->final + 1) % b->capacidad;
      b->elementos[b->final] = p;
      if (b->frente == -1)
         b->frente = 0;
   }
}

static inline void sacarProducto(Buffer* b, Producto* p) {
   if (!bufVacio(b)) {
      *p = b->elementos[b->frente];
      if (b->frente == b->final) {
         b->frente = -1;
         b->final = -1;
      }
      else
         b->frente = (b->frente + 1) % b->capacidad;
   }
}

void observador(void* ptr) {
   while (gananciaTotal < 5000) {
      // Esperar hasta que un consumidor termine un viaje
      sem_wait(&observadorSem);
      pthread_mutex_lock(&mutex);
      printf("Ganancia total actual: $%d\n", gananciaTotal);
      pthread_mutex_unlock(&mutex);
   }
}

void consumidor(void* ptr) {
   Producto viaje;
   int gananciaLocal;
   while (1) {
      // Uso de semáforos para esperar hasta que haya elementos en el buffer
      sem_wait(&lleno);
      pthread_mutex_lock(&mutex);
      sacarProducto(&bufPeticiones, &viaje);
      pthread_mutex_unlock(&mutex);
      sem_post(&vacio);

      // Procesar el viaje
      printf("Conductor procesando viaje al destino %c\n", viaje.destino);
      sleep(viaje.tiempo);

      // Calcular ganancia local y acumularla a la ganancia total
      if (viaje.tipoUber == UBERX) {
         gananciaLocal = viaje.tiempo * 20;
      }
      else {
         gananciaLocal = viaje.tiempo * 40;
      }
      pthread_mutex_lock(&mutex);
      gananciaTotal += gananciaLocal;
      pthread_mutex_unlock(&mutex);

      // Notificar al observador que se ha completado un viaje
      sem_post(&observadorSem);

      if (gananciaTotal >= 5000) {
         // Terminar cuando la ganancia total alcanza $5000
         pthread_exit(NULL);
      }
   }
}

void productorUberX(void* ptr) {
   Producto viaje;
   while (1) {
      // Generar un viaje UberX
      viaje.destino = (char)('a' + aleatorioEn(0, 4));
      viaje.tiempo = aleatorioEn(2, 4);
      viaje.tipoUber = UBERX;

      // Uso de semáforos antes de meter el viaje
      sem_wait(&vacio);
      pthread_mutex_lock(&mutex);
      meterProducto(&bufPeticiones, viaje);
      pthread_mutex_unlock(&mutex);
      sem_post(&lleno);

      pthread_mutex_lock(&mutex);
      if (gananciaTotal >= 5000) {
         pthread_mutex_unlock(&mutex);
         break; // Terminar el hilo si se alcanza la ganancia total de $5000
      }
      pthread_mutex_unlock(&mutex);
   }
   // Insertar un viaje especial con destino 'x'
   viaje.destino = 'x';
   viaje.tiempo = aleatorioEn(2, 4);
   viaje.tipoUber = UBERX;
   sem_wait(&vacio);
   pthread_mutex_lock(&mutex);
   meterProducto(&bufPeticiones, viaje);
   pthread_mutex_unlock(&mutex);
   sem_post(&lleno);
   // Notificar al observador que se ha completado un viaje
   sem_post(&observadorSem);
}

void productorUberBlack(void* ptr) {
   Producto viaje;
   while (1) {
      // Generar un viaje UberBlack
      viaje.destino = (char)('a' + aleatorioEn(0, 4));
      viaje.tiempo = aleatorioEn(2, 4);
      viaje.tipoUber = UBERBLACK;

      // Uso de semáforos antes de meter el viaje
      sem_wait(&vacio);
      pthread_mutex_lock(&mutex);
      meterProducto(&bufPeticiones, viaje);
      pthread_mutex_unlock(&mutex);
      sem_post(&lleno);

      pthread_mutex_lock(&mutex);
      if (gananciaTotal >= 5000) {
         pthread_mutex_unlock(&mutex);
         break; // Terminar el hilo si se alcanza la ganancia total de $5000
      }
      pthread_mutex_unlock(&mutex);
   }
}

int main(int argc, char const* argv[]) {
   creaBuffer(&bufPeticiones, MAX_PETICIONES);

   sem_init(&lleno, 0, 0);
   sem_init(&vacio, 0, MAX_PETICIONES);
   sem_init(&observadorSem, 0, 0);
   pthread_mutex_init(&mutex, NULL);

   pthread_t hiloObservador;
   pthread_create(&hiloObservador, NULL, (void*)observador, NULL);

   pthread_t hiloBlack;
   pthread_create(&hiloBlack, NULL, (void*)productorUberBlack, NULL);

   pthread_t hiloX;
   pthread_create(&hiloX, NULL, (void*)productorUberX, NULL);

   pthread_t hilosConsumidores[5];
   for (int i = 0; i < 5; i++) {
      pthread_create(&hilosConsumidores[i], NULL, (void*)consumidor, NULL);
   }

   pthread_join(hiloBlack, NULL);
   pthread_join(hiloX, NULL);
   pthread_join(hiloObservador, NULL);
   for (int i = 0; i < 5; i++) {
      pthread_join(hilosConsumidores[i], NULL);
   }

   destruyeBuffer(&bufPeticiones);

   sem_destroy(&lleno);
   sem_destroy(&vacio);
   sem_destroy(&observadorSem);
   pthread_mutex_destroy(&mutex);

   printf("\n\nFIN.\n\n");

   return 0;
}
