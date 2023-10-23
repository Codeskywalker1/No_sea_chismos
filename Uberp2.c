#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


/*** Declaraciones para el buffer circular ***/

// Declaración del tipo Producto para los viajes.
typedef struct {
   char destino;
   int tiempo;
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

/*** TERMINAR ****/ 
/* Declaración de semáforos y mutex */
pthread_mutex_t mBuffer;
sem_t sProducto;
sem_t sLugar;
/*** Fin TERMINAR */

#define HILOS 6

int main(int argc, char const *argv[]) 
{
   // Inicializar el buffer con una capacidad máxima FIJA.
   creaBuffer(&bufPeticiones, MAX_PETICIONES);

   /*** TERMINAR ****/ 
   // Incialización de semáforos y mutex
   sem_init(&sProducto, 0, 0);// Al inicio no hay productos en el buffer
   sem_init(&sLugar, 0, MAX_PETICIONES);// El número de lugares es tamaño del buffer
   pthread_mutex_init(&mBuffer, NULL);// Candado para acceder al buffer
   /*** Fin TERMINAR */


   /*** TERMINAR ****/ 
   // Declaración de los SEIS hilos consumidores.
   int id[HILOS];
   pthread_t hilos[HILOS];

   // Creación de los hilos consumidores para que ejecuten 
   //   la función "consumidor", que está abajo.
   for (int i = 0; i < HILOS; i++)
   {
      id[i] = i;
      pthread_create(&hilos[i], NULL, (void *) consumidor, &id[i]);
   }
   /*** Fin TERMINAR */


   /** ESTE hilo principal será el PRODUCTOR **/

   /*  Este ciclo es la acción del productor */
   int totalSolicitudes = 25;
   Producto viaje; // Se usará para ir generando peticiones de viajes.
   for (size_t i = 0; i < totalSolicitudes; ++i) {

      /*** TERMINAR ****/ 
      // Asignar a 'viaje' un destino (char entre 'a' y 'e') y 
      viaje.destino =  'A' + (rand() % 5);
      //   un tiempo (2 y 6 segs.).
      viaje.tiempo = aleatorioEn(2, 6);
      /*** Fin TERMINAR ***/

      /*** TERMINAR ****/ 
      // Uso semáforos ANTES de meter el viaje
      sem_wait(&sLugar);// Si no hay lugares en el buffer esperamos
      pthread_mutex_lock(&mBuffer);// Intentar aceeder al buffer
      /*** Fin TERMINAR ***/
      meterProducto(&bufPeticiones, viaje);
      /*** TERMINAR ****/ 
      // Uso semáforos DESPUÉS de meter el viaje
      pthread_mutex_unlock(&mBuffer);// Intentar aceeder al buffer
      sem_post(&sProducto);// Si no hay lugares en el buffer esperamos
      /*** Fin TERMINAR ***/

      sleep( aleatorioEn(2, 5) );
   }

   // Meter el viaje especial para indicar que ya terminó todo.
   viaje.destino = 'X';
   meterProducto(&bufPeticiones, viaje);
   sem_post(&sProducto);
   /*** TERMINAR ****/ 
   // Esperar a TODOS los hilos consumidores
   for (int i = 0; i < HILOS; i++)
   {
      pthread_join(hilos[i], NULL);
   }
   /*** Fin TERMINAR ***/

   destruyeBuffer(&bufPeticiones);
   pthread_mutex_destroy(&mBuffer);
   sem_destroy(&sProducto);
   sem_destroy(&sLugar);
   printf("\n\nFIN.\n\n");

   return 0;
}

/* Definición de la tarea que hará un conductor */
void consumidor(void* a) {
   //Obtener id
   int id = *((int *)a);
   Producto viaje;
   
   /*** TERMINAR ****/ 
   while(true){
      // La tarea para sacar y procesar viajes.
      sem_wait(&sProducto);
      pthread_mutex_lock(&mBuffer);
      sacarProducto(&bufPeticiones, &viaje);
      printf("Conductor %d \t Destino: %c\n", id, viaje.destino);
      pthread_mutex_unlock(&mBuffer);
      sem_post(&sLugar);
      if (viaje.destino == 'X'){
         //Meter al buffer el viaje especial
         meterProducto(&bufPeticiones, viaje); 
         sem_post(&sProducto);        
         pthread_exit(0);
      }
      /*** Fin TERMINAR ***/
      sleep(viaje.tiempo);
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
      p->destino = b->elementos[b->frente].destino;
      p->tiempo = b->elementos[b->frente].tiempo;
      if (b->frente == b->final)
      {  // Era el unico elemento.
         b->frente = -1;
         b->final = -1;
      }
      else
         b->frente = (b->frente + 1) % b->capacidad;
   }
}