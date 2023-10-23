#include <pthread.h>

// Más adelante veremos para qué sirve esto.
pthread_mutex_t screen_mutex;  

// Una enumeración asigna un alias a los códigos enteros de los colores.
enum COLORES {NEGRO=0, ROJO, VERDE, AMARILLO, AZUL, MAGENTA, CYAN, BLANCO};

void initPantalla();
void finPantalla();
void escribirR_C(int ren, int col, char* texto, int color);
void limpiarPantalla();