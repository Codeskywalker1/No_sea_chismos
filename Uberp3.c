#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


/*** Declaraciones para el buffer circular ***/

#define UBERBLACK 1
#define UBERX 2

// Declaración del tipo Producto para los viajes.
typedef struct {
   char destino;  // Destino del viaje en UBER
   int tiempo;    // Duración del viaje en UBER
   int tipoUber;  // Tipo de Uber: UBERBLACK o UBERX
} Producto;

// Declaración del nuevo tipo de datos Buffer
typedef struct {
   Producto* elementos;  // almacenamiento interno del buffer.
   int capacidad;   // capacidad máxima del buffer.
   int frente;      // índice para apuntar al elemento que está al frente.
   int final;       // índice para apuntar al elemento que está al final.
} Buffer;


/** Funciones para modificar el buffer. **/

// buf: dirección de variable tipo Buffer ya declarada.
// tam: tamaño máximo fijo del buffer que se creará.
void creaBuffer(Buffer* buf, int tam);

// buf: dirección de un buffer ya declarado e INICIALIZADO.
// p: el producto que se va insertar.
static inline void meterProducto(Buffer *b, Producto p);

// buf: dirección de un buffer ya declarado e INICIALIZADO.
// p: DIRECCIÓN de una variable de tipo producto donde se DEJARÁ el resultado.
static inline void sacarProducto(Buffer *b, Producto* p);

// buf: dirección de un buffer ya declarado e INICIALIZADO.
void destruyeBuffer(Buffer* buf);

// Estas funciones son internas al buffer, NO tienen que usarlas ustedes.
static inline bool bufLleno(Buffer* buf);  
static inline bool bufVacio(Buffer* buf);

int aleatorioEn(int minVal, int maxVal);

/*** FIN Declaraciones para el buffer circular ***/



// Número máximo de peticiones y tamaño del buffer.
#define  MAX_PETICIONES 3
Buffer bufPeticiones;    // Buffer de peticiones de viajes.


// NO olvidar IMPLEMENTAR abajo del main esta función.
void consumidor(void* ptr);
void productorUberBlack(void* ptr);
void productorUberX(void* ptr);

/*** TERMINAR ****/ 
/* Declaración de semáforos y/o mutex */
sem_t sProductos;
sem_t sLugares;
sem_t sHayViajes;
pthread_mutex_t mBuffer;
pthread_mutex_t mTerminal;
pthread_mutex_t mGanancia;
/*** Fin TERMINAR */

#define HILOS 5
#define PRODUCTORES 2
#define TARIFA_UBER_X 20
#define TARIFA_UBER_BLACK 40
#define GANANCIA_ESTABLECIDA 5000

/*** TERMINAR ****/
// Declarar las variables compartidas que sean necesarias.
int GT = 0;// Ganancia total 

/*** Fin TERMINAR */



int main(int argc, char const *argv[]) 
{
   // Inicializar el buffer con una capacidad máxima FIJA.
   creaBuffer(&bufPeticiones, MAX_PETICIONES);
   srand(time(NULL));
   /*** TERMINAR ****/ 
   // Incialización de semáforos y mutex
   pthread_mutex_init(&mBuffer, NULL);
   pthread_mutex_init(&mTerminal, NULL);
   pthread_mutex_init(&mGanancia, NULL);
   sem_init(&sLugares, 0, MAX_PETICIONES);
   sem_init(&sProductos, 0, 0);
   sem_init(&sHayViajes, 0, 0);
   /*** Fin TERMINAR */


   /*** TERMINAR ****/ 
   // Declaración de los hilos consumidores.
   pthread_t hilos[HILOS];
   // Creación de los hilos consumidores para que ejecuten 
   //   la función "consumidor", que está abajo.
   for (int i = 0; i < HILOS; i++)
   {
      pthread_create(&hilos[i], NULL, (void *) consumidor, NULL);
   }
   /*** Fin TERMINAR */

   // Creación e inicio de los DOS hilos productores
   pthread_t hiloBlack;
   pthread_create(&hiloBlack, NULL, (void*) productorUberBlack,  NULL);

   pthread_t hiloX;
   pthread_create(&hiloX, NULL, (void*) productorUberX,  NULL);



   /** Lo que sigue será el hilo OBSERVADOR **/
   /*** TERMINAR ****/ 
   // Poner el ciclo del hilo que muestra la Ganacia Total solamente cuando realmente
   // se hace un viaje.
   while(GT < GANANCIA_ESTABLECIDA)
   {
      sem_wait(&sHayViajes);
      pthread_mutex_lock(&mTerminal);
      printf("Ganancia total: %d\n", GT);    
      pthread_mutex_unlock(&mTerminal);
   }
   /*** Fin TERMINAR ***/


   /*** TERMINAR ****/ 
   // Esperar a TODOS los hilos productores y consumidores
   pthread_join(hiloBlack,NULL);
   pthread_join(hiloX,NULL);
   for (int i = 0; i < HILOS; i++)
   {
      pthread_join(hilos[i], NULL);
   }
   /*** Fin TERMINAR ***/


   destruyeBuffer(&bufPeticiones);
   pthread_mutex_destroy(&mBuffer);
   pthread_mutex_destroy(&mTerminal);
   pthread_mutex_destroy(&mGanancia);
   sem_destroy(&sProductos);
   sem_destroy(&sLugares);
   sem_destroy(&sHayViajes);
   printf("\n\nFIN.\n\n");

   return 0;
}

/* Definición de la tarea que hará un conductor */
void consumidor(void* ptr) {
   int gL = 0; // Ganancia total local
   Producto viaje;
   /*** TERMINAR ****/ 
   // La tarea para sacar y procesar viajes de ambos tipos de Uber.
   while(true){
      sem_wait(&sProductos);
      pthread_mutex_lock(&mBuffer);
      sacarProducto(&bufPeticiones, &viaje);
      pthread_mutex_unlock(&mBuffer);
      sem_post(&sLugares);
      if (viaje.destino == 'X'){
         meterProducto(&bufPeticiones, viaje);
         sem_post(&sProductos);
         pthread_exit(0);
      }
      pthread_mutex_lock(&mTerminal);
      printf("Destino: %c\n", viaje.destino);
      pthread_mutex_unlock(&mTerminal);
      sleep(viaje.tiempo);
      // Calcular la ganacia total del viaje
      if(viaje.tipoUber == UBERX) {
         gL = viaje.tiempo * TARIFA_UBER_X;
      } 
      else {
         gL = viaje.tiempo * TARIFA_UBER_BLACK;
      }

      pthread_mutex_lock(&mGanancia);
      GT += gL;
      sem_post(&sHayViajes);
      pthread_mutex_unlock(&mGanancia);
      
   }
   /*** Fin TERMINAR ***/
}

void productorUberX(void* ptr) {
   Producto viaje;

   /*** TERMINAR ****/ 
   // Todo lo que falta para el ciclo principal.
   while (GT < GANANCIA_ESTABLECIDA){
      viaje.destino = 'A' + (rand() % 5);
      viaje.tiempo = aleatorioEn(2, 4);
      viaje.tipoUber = UBERX;
      sem_wait(&sLugares);
      pthread_mutex_lock(&mBuffer);
      meterProducto(&bufPeticiones, viaje);
      pthread_mutex_unlock(&mBuffer);
      sem_post(&sProductos);
      sleep(aleatorioEn(1,2));
   }
   // No olvidar que al final, este productor mete al búfer el 
   // viaje especial con destino= 'x'.
   viaje.destino = 'X';
   meterProducto(&bufPeticiones, viaje);
   sem_post(&sProductos);
   /*** Fin TERMINAR ***/
}

void productorUberBlack(void* ptr) {
   Producto viaje;

   /*** TERMINAR ****/ 
   // Usar la variable de Ganancia Total para la condición
   // para terminar el ciclo de abajo.
   /*** Fin TERMINAR ***/

   while (GT < GANANCIA_ESTABLECIDA) {
      /*** TERMINAR ****/ 
      // Asignar a 'viaje' un destino (char entre 'a' y 'e'),  
      viaje.destino = 'A' + (rand() % 5);
      // un tiempo (entre 2 y 4 segs.) y ...
      viaje.tiempo = aleatorioEn(2, 4);
      /*** Fin TERMINAR ***/
      viaje.tipoUber = UBERBLACK; // ... el tipo de Uber.

      /*** TERMINAR ****/ 
      // Uso de semáforos ANTES de meter el viaje
      sem_wait(&sLugares);
      pthread_mutex_lock(&mBuffer);
      /*** Fin TERMINAR ***/
      meterProducto(&bufPeticiones, viaje);
      /*** TERMINAR ****/ 
      // Uso de semáforos DESPUÉS de meter el viaje
      pthread_mutex_unlock(&mBuffer);
      sem_post(&sProductos);
      /*** Fin TERMINAR ***/
      
      sleep( aleatorioEn(1, 2) );
   }
}





// NO ES NECESARIO VER ESTE CÓDIGO, SIMPLEMENTE HACER LAS LLAMADAS PARA USAR EL BUFFER.

int aleatorioEn(int minVal, int maxVal) {
   return (minVal + rand() % (maxVal-minVal + 1) );
}

void creaBuffer(Buffer *buf, int tam)
{
   buf->capacidad = tam;
   buf->elementos = (Producto *)malloc(tam * sizeof(Producto));
   buf->frente = -1; // Para indicar que está vacía la cola
   buf->final = -1;  // Para indicar que está vacía la cola
}

void destruyeBuffer(Buffer *buf)
{
   free(buf->elementos);
}

static inline bool bufLleno(Buffer *b)
{
   int next = (b->final + 1) % b->capacidad;

   if (b->frente == next)
      return true;
   else
      return false;
}

static inline bool bufVacio(Buffer *b)
{
   if (b->frente == -1)
      return true;
   else
      return false;
}

static inline void meterProducto(Buffer *b, Producto p)
{
   if (!bufLleno(b))
   {
      b->final = (b->final + 1) % b->capacidad;
      b->elementos[b->final] = p;
      if (b->frente == -1) // Cuando estaba vacia, el nuevo elemento esta en pos 0.
         b->frente = 0;
   }
}

static inline void sacarProducto(Buffer *b, Producto *p)
{
   if (!bufVacio(b))
   {
      *p = b->elementos[b->frente];

      if (b->frente == b->final)
      {  // Era el unico elemento.
         b->frente = -1;
         b->final = -1;
      }
      else
         b->frente = (b->frente + 1) % b->capacidad;
   }
}