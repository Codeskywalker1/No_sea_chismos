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

int aleatorioEn(int minVal, int maxVal);

/*** FIN Declaraciones para el buffer circular ***/


// NO olvidar IMPLEMENTAR abajo del main estas DOS funciones.
void consumidor(void* ptr);
void productor(void* ptr);


// Número máximo de peticiones y tamaño del buffer.
#define  TAM_BUF_T 3
Buffer bufTareas;    // Buffer de tareas para los consumidores

#define  TAM_BUF_R 6
Buffer bufResultados;  // Buffer de resultados hilo maestro



/*** TERMINAR ****/ 
/* Declaración de semáforos y mutex */

/*** Fin TERMINAR */


int main(int argc, char const *argv[]) 
{
   // Inicializar el buffer con una capacidad máxima FIJA.
   creaBuffer(&bufTareas, TAM_BUF_T);
   creaBuffer(&bufResultados, TAM_BUF_R);

   /*** TERMINAR ****/ 
   // Incialización de semáforos y mutex
   /*** Fin TERMINAR */


   /*** TERMINAR ****/ 
   // Declaración de los SEIS hilos consumidores.

   // Creación de los hilos consumidores para que ejecuten 
   //   la función "consumidor", que está abajo.

   // Declaración del único hilo productor para que ejecute
   //   la función "productor" que está abajo.

   /*** Fin TERMINAR */


   /** El hilo principal será el que hace la suma y termina TODO */
   int L = 45;      // El objetivo será sobrepasar este límite L al sumar cuadrados.
   int sumaCuadrados = 0; // Al inicio la suma es 0.
   int resActual;   // Acá irá obteniendo cada valor ya elevado al cuadrado.

   // RECORDATORIO: La condición de terminación solamente usa variables locales:
   // L y sumaCuadrados
   while ( /* poner condición de terminación */ ) 
   { 

      /*** TERMINAR ****/ 
      // Uso semáforos ANTES de sacar el resultado
      /*** Fin TERMINAR ***/
     sacarProducto(&bufResultados, &resActual);
      /*** TERMINAR ****/ 
      // Uso semáforos DESPUÉS de sacar el resultado
      /*** Fin TERMINAR ***/

      sumaCuadrados = sumaCuadrados + resActual;
   }

   // Este hilo principal sí puede dejar corriendo a los otros hilos.
   // Acá sale él.

   destruyeBuffer(&bufTareas);
   destruyeBuffer(&bufResultados);


   printf("\n\nFIN.\n\n");

   return 0;
}

/* Definición de la tarea que hará el único productor */
void productor(void* ptr) {
   int nActual = 1;

   // Este productor sí lo podemos tener en un ciclo "infinito"
   // El hilo principal sí podrá terminar sin problema.
   while (true) {
      /*** TERMINAR ****/ 
      // Uso semáforos ANTES de meter el nuevo entero
      /*** Fin TERMINAR ***/
     meterProducto(&bufTareas, nActual);
      /*** TERMINAR ****/ 
      // Uso semáforos DESPUÉS de meter el nuevo entero
      /*** Fin TERMINAR ***/
      nActual++;
   }
}

/* Definición de la tarea que hará cada consumidor */
void consumidor(void* ptr) {
   // A cada consumidor sí lo podemos tener en un ciclo "infinito"
   // El hilo principal sí podrá terminar sin problema.

   /*** TERMINAR ****/ 
   // Terminar el ciclo principal donde en cada iteración:
   //  1) Toma la tarea del búfer de tareas.
   //  2) Calcula el cuadrado.
   //  3) Meter el resultado al búfer de RESULTADOS

   // CADA acceso a los búfers anteriores deben 
   // tener el uso de los semáforos necesarios.
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