#include <stdio.h>
#include "printRC.h"

void escribirR_C(int ren, int col, char *texto, int color)
{
   pthread_mutex_lock(&screen_mutex);
   printf("\033[38;5;%dm", color);
   printf("\033[%d;%dH", ren, col);
   printf("%s", texto);
   fflush(stdout);
   pthread_mutex_unlock(&screen_mutex);
}

void limpiarPantalla() {
   printf("\033[2J");
   fflush(stdout);
}

void initPantalla() {
   pthread_mutex_init(&screen_mutex, NULL);
}

void finPantalla() {
   pthread_mutex_destroy(&screen_mutex);
}