#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_PETICIONES 3

typedef struct {
    char tipoTrans;
    int monto;
} Producto;

typedef struct {
    Producto* elementos;
    int capacidad;
    int frente;
    int final;
} Buffer;

Buffer bufPeticiones;
pthread_mutex_t mutexBuffer;
sem_t semEspacioLibre;
sem_t semElementos;
int cuentaGlobal = 0;
int objetivoAlcanzado = 0; // Variable para verificar si se alcanzó el objetivo de $5000

void creaBuffer(Buffer* buf, int tam);
void destruyeBuffer(Buffer* buf);
static inline bool bufLleno(Buffer* buf);
static inline bool bufVacio(Buffer* buf);
static inline void meterProducto(Buffer* buf, Producto p);
static inline void sacarProducto(Buffer* buf, Producto* p);
int aleatorioEn(int minVal, int maxVal);

void consumidor(void* a);
void productorDepositos(void* ptr);
void productorRetiros(void* ptr);
void hiloObservador(void* ptr);

int main(int argc, char const* argv[]) {
    creaBuffer(&bufPeticiones, MAX_PETICIONES);
    pthread_mutex_init(&mutexBuffer, NULL);
    sem_init(&semEspacioLibre, 0, MAX_PETICIONES);
    sem_init(&semElementos, 0, 0);

    pthread_t consumidores[4];
    for (int i = 0; i < 4; i++) {
        pthread_create(&consumidores[i], NULL, (void*)consumidor, NULL);
    }

    pthread_t hiloDepositos;
    pthread_create(&hiloDepositos, NULL, (void*)productorDepositos, NULL);

    pthread_t hiloRetiros;
    pthread_create(&hiloRetiros, NULL, (void*)productorRetiros, NULL);

    pthread_t observador;
    pthread_create(&observador, NULL, (void*)hiloObservador, NULL);

    pthread_join(observador, NULL);

    destruyeBuffer(&bufPeticiones);

    printf("\n\nFIN.\n\n");

    return 0;
}

void consumidor(void* a) {
    while (1) {
        Producto trans;
        sacarProducto(&bufPeticiones, &trans);

        if (trans.tipoTrans == 't') {
            break;
        }

        if (trans.tipoTrans == 'd') {
            cuentaGlobal += trans.monto;
            printf("Realizado depósito de $%d\n", trans.monto);
            printf("Balance de la cuenta: %d\n", cuentaGlobal);
        } else if (trans.tipoTrans == 'r') {
            cuentaGlobal -= trans.monto;
            printf("Realizado retiro de $%d\n", trans.monto);
            printf("Balance de la cuenta: %d\n", cuentaGlobal);
        }

        sem_post(&semEspacioLibre);

        if (cuentaGlobal >= 5000) {
            objetivoAlcanzado = 1;
            break;
        }
    }
}

void productorDepositos(void* ptr) {
    while (1) {
        Producto trans;
        trans.monto = aleatorioEn(100, 500);
        trans.tipoTrans = 'd';

        sem_wait(&semEspacioLibre);
        pthread_mutex_lock(&mutexBuffer);

        meterProducto(&bufPeticiones, trans);
        printf("Generado depósito de $%d\n", trans.monto);
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semElementos);

        if (objetivoAlcanzado) {
            break;
        }

        usleep(aleatorioEn(1, 3) * 100000);
    }
}

void productorRetiros(void* ptr) {
    while (1) {
        Producto trans;
        trans.monto = aleatorioEn(20, 100);
        trans.tipoTrans = 'r';

        sem_wait(&semEspacioLibre);
        pthread_mutex_lock(&mutexBuffer);

        meterProducto(&bufPeticiones, trans);
        printf("Generado retiro de $%d\n", trans.monto);
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semElementos);

        if (objetivoAlcanzado) {
            break;
        }

        usleep(aleatorioEn(1, 2) * 100000);
    }
}

void hiloObservador(void* ptr) {
    while (1) {
        sem_wait(&semElementos);
        printf("Balance de la cuenta: %d\n", cuentaGlobal);

        if (cuentaGlobal >= 5000) {
            printf("Objetivo de $5000 alcanzado. Terminando el programa.\n");
            objetivoAlcanzado = 1;

            // Terminar productores
            Producto transFin;
            transFin.tipoTrans = 't';

            pthread_mutex_lock(&mutexBuffer);
            meterProducto(&bufPeticiones, transFin);
            pthread_mutex_unlock(&mutexBuffer);
            sem_post(&semElementos);

            break;
        }
    }
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

static inline bool bufLleno(Buffer* buf) {
    int next = (buf->final + 1) % buf->capacidad;
    return (buf->frente == next);
}

static inline bool bufVacio(Buffer* buf) {
    return (buf->frente == -1);
}

static inline void meterProducto(Buffer* buf, Producto p) {
    if (!bufLleno(buf)) {
        buf->final = (buf->final + 1) % buf->capacidad;
        buf->elementos[buf->final] = p;
        if (buf->frente == -1)
            buf->frente = 0;
    }
}

static inline void sacarProducto(Buffer* buf, Producto* p) {
    if (!bufVacio(buf)) {
        *p = buf->elementos[buf->frente];

        if (buf->frente == buf->final) {
            buf->frente = -1;
            buf->final = -1;
        } else {
            buf->frente = (buf->frente + 1) % buf->capacidad;
        }
    }
}

int aleatorioEn(int minVal, int maxVal) {
    return (minVal + rand() % (maxVal - minVal + 1));
}