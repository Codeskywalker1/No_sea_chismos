#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/*** Declaraciones para el buffer circular ***/

// Declaración del nuevo tipo de datos Buffer
typedef struct {
   int* elementos;  // almacenamiento interno del buffer.
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
static inline void meterProducto(Buffer *b, int p);

// buf: dirección de un buffer ya declarado e INICIALIZADO.
// p: DIRECCIÓN de una variable de tipo producto donde se DEJARÁ el resultado.
static inline void sacarProducto(Buffer *b, int* p);

// buf: dirección de un buffer ya declarado e INICIALIZADO.
void destruyeBuffer(Buffer* buf);

// Estas funciones son internas al buffer, NO tienen que usarlas ustedes.
static inline bool bufLleno(Buffer* buf);
static inline bool bufVacio(Buffer* buf);

/*** FIN Declaraciones para el buffer circular ***/

// NO olvidar IMPLEMENTAR abajo del main la función para los consumidores.
void consumidor(void* ptr);
bool esPerfecto(int n);  // Será más fácil si implementan esta función también.

#define TAREA_ESPECIAL -1

// Número máximo de peticiones y tamaño del buffer.
#define  TAM_BUFF 3
Buffer bufTareas;    // Buffer de tareas para los consumidores

/***** IMPORTANTE *****/
/**********************/
int numP;  // Contador COMPARTIDO del número de enteros perfectos encontrados.

/*** TERMINAR ****/
/* Declaración de semáforos y mutex */
pthread_mutex_t mBuffer;
sem_t sProducto;
sem_t sNumero;
/*** Fin TERMINAR */

#define HILOS 6

int main(int argc, char const *argv[])
{
   // Inicializar el buffer con una capacidad máxima FIJA.
   creaBuffer(&bufTareas, TAM_BUFF);

   /*** TERMINAR ****/
   // Incialización de semáforos y mutex
   sem_init(&sProducto, 0, 0); // Al inicio no hay productos en el buffer
   sem_init(&sNumero, 0, TAM_BUFF); // Es el tamaño del buffer
   pthread_mutex_init(&mBuffer, NULL); // Candado para acceder al buffer
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

   /** El hilo principal será el PRODUCTOR **/
   int P = 10;      // El objetivo será encontrar 10 números perfectos a partir de 2
   int nActual = 2; // Este es el primer entero en probar si es perfecto.

   // Esta variable es compartida (está ya declarada arriba del main)
   numP = 0; // Al inicio no tenemos ningún número perfecto encontrado.

   /*** TERMINAR ****/
   // Declarar alguna variable adecuada para poder terminar el ciclo
   bool tareasT = true;
   /*** Fin TERMINAR ***/

   while (tareasT)
   {
      /*** TERMINAR ****/
      // Verificar si numP >= P y hacer lo necesario para romper el ciclo o
      // continuar metiendo tareas.
      // RECORDATORIO: P es local, pero numP compartida.
      if (numP >= P)
      {
         tareasT = false;
      }
      else
      {
         /*** TERMINAR ****/
         // Uso semáforos ANTES de meter el entero actual.
         sem_wait(&sNumero); // Si no hay lugares en el buffer esperamos
         pthread_mutex_lock(&mBuffer); // Intentar aceeder al buffer
         /*** Fin TERMINAR ***/
         meterProducto(&bufTareas, nActual);
         printf("Productor: Colocando número en el buffer: %d\n", nActual); // Agregar impresión
         /*** TERMINAR ****/
         // Uso semáforos DESPUÉS de meter el entero actual.
         pthread_mutex_unlock(&mBuffer); // Intentar aceeder al buffer
         sem_post(&sProducto); // Si no hay lugares en el buffer esperamos
         /*** Fin TERMINAR ***/
      }

      nActual++;
   }

   // Meter la tarea especial para indicar que ya terminó todo.
   meterProducto(&bufTareas, TAREA_ESPECIAL);

   /*** TERMINAR ****/
   // Esperar a TODOS los hilos consumidores
   for (int i = 0; i < HILOS; i++)
   {
      pthread_join(hilos[i], NULL);
   }
   /*** Fin TERMINAR ***/

   destruyeBuffer(&bufTareas);

   printf("Proceso completo. Se encontraron %d números perfectos.\n", numP); // Agregar impresión al final

   return 0;
}

/* Definición de la tarea que hará un consumidor */
void consumidor(void *ptr)
{
   /*** TERMINAR ****/
   int id = *((int *)ptr);
   int num;
   // La tarea para sacar, verificar si el número es perfecto, etc.
   sem_wait(&sProducto);
   pthread_mutex_lock(&mBuffer);
   sacarProducto(&bufTareas, &num);
   printf("Hilo %d: Consumiendo número del buffer: %d\n", id, num); // Agregar impresión
   if (num == TAREA_ESPECIAL)
   {
      pthread_mutex_unlock(&mBuffer);
      sem_post(&sNumero);
      return;
   }
   printf("Hilo %d: Verificando si %d es un número perfecto...\n", id, num); // Agregar impresión
   if (esPerfecto(num))
   {
      numP++;
      printf("Hilo %d: ¡Número perfecto encontrado! %d es perfecto.\n", id, num); // Agregar impresión
   }
   pthread_mutex_unlock(&mBuffer);
   sem_post(&sNumero);
   /*** Fin TERMINAR ***/
}

bool esPerfecto(int n)
{
   /*** TERMINAR ****/
   // Determinar si n es perfecto o no.
   bool perfecto = false;
   int sum = 0, mul = 0;
   for (int i = 1; i < n; i++)
   {
      mul = n % i;
      if (mul == 0)
      {
         sum += i;
      }
   }
   if (sum == n)
   {
      perfecto = true;
   }
   return perfecto;
   /*** Fin TERMINAR ***/
}

// NO ES NECESARIO VER ESTE CÓDIGO, SIMPLEMENTE HACER LAS LLAMADAS PARA USAR EL BUFFER.

void creaBuffer(Buffer *buf, int tam)
{
   buf->capacidad = tam;
   buf->elementos = (int *)malloc(tam * sizeof(int));
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

static inline void meterProducto(Buffer *b, int p)
{
   if (!bufLleno(b))
   {
      b->final = (b->final + 1) % b->capacidad;
      b->elementos[b->final] = p;
      if (b->frente == -1) // Cuando estaba vacía, el nuevo elemento está en pos 0.
         b->frente = 0;
   }
}

static inline void sacarProducto(Buffer *b, int *p)
{
   if (!bufVacio(b))
   {
      *p = b->elementos[b->frente];

      if (b->frente == b->final)
      {  // Era el único elemento.
         b->frente = -1;
         b->final = -1;
      }
      else
         b->frente = (b->frente + 1) % b->capacidad;
   }
}
