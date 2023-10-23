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


// NO olvidar IMPLEMENTAR abajo del main esta función.
void consumidor(void* ptr);
bool esPrimo(int n);  // Será más fácil si implementan esta función también.


#define TAREA_ESPECIAL -1

// Número máximo de peticiones y tamaño del buffer.
#define  TAM_BUFF 3
Buffer bufTareas;    // Buffer de tareas para los consumidores

/** IMPORTANTE **/
int S;  // Suma COMPARTIDA de los números primos encontrados.


/*** TERMINAR ****/ 
/* Declaración de semáforos y mutex */

/*** Fin TERMINAR */


int main(int argc, char const *argv[]) 
{
   // Inicializar el buffer con una capacidad máxima FIJA.
   creaBuffer(&bufTareas, TAM_BUFF);

   /*** TERMINAR ****/ 
   // Incialización de semáforos y mutex
   /*** Fin TERMINAR */


   /*** TERMINAR ****/ 
   // Declaración de los SEIS hilos consumidores.

   // Creación de los hilos consumidores para que ejecuten 
   //   la función "consumidor", que está abajo.

   /*** Fin TERMINAR */



   /** El hilo principal será el PRODUCTOR **/
   int L = 16;      // El objetivo será sobrepasar este límite L al sumar primos.
   int nActual = 2; // Este es el primer entero en probar si es primo y sumarlo.

   // Esta variable es compartida (está ya declarada arriba del main)
   S = 0;  // Al inicio la suma de números primos acumulados es 0.

   /*** TERMINAR ****/ 
   // Declarar alguna variable adecuada para poder terminar el ciclo
   /*** Fin TERMINAR ***/
   
   while ( true ) 
   { 
      /*** TERMINAR ****/ 
      // Verificar si L >= S y hacer lo necesario para romper el ciclo o
      // continuar metiendo tareas.
      // RECORDATORIO: L es local, pero S es compartida.
      /*** Fin TERMINAR ***/

      /*** TERMINAR ****/ 
      // Uso semáforos ANTES de meter el posible primo
      /*** Fin TERMINAR ***/
      meterProducto(&bufTareas, nActual);
      /*** TERMINAR ****/ 
      // Uso semáforos DESPUÉS de meter el posible primo
      /*** Fin TERMINAR ***/

      nActual++;
   }

   // Meter la tarea especial para indicar que ya terminó todo.
   meterProducto(&bufTareas, TAREA_ESPECIAL);

   /*** TERMINAR ****/ 
   // Esperar a TODOS los hilos consumidores
   /*** Fin TERMINAR ***/


   destruyeBuffer(&bufTareas);

   printf("\n\nFIN.\n\n");

   return 0;
}

/* Definición de la tarea que hará un consumidor */
void consumidor(void* ptr) {
   /*** TERMINAR ****/ 
   // La tarea para sacar, verificar si el número es perfecto, etc.
   /*** Fin TERMINAR ***/
}

bool esPrimo(int n) {
   /*** TERMINAR ****/ 
   // Determinar si el número n es primo o no lo es.
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
      if (b->frente == -1) // Cuando estaba vacia, el nuevo elemento esta en pos 0.
         b->frente = 0;
   }
}

static inline void sacarProducto(Buffer *b, int *p)
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