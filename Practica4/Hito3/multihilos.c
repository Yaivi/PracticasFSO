#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>
#include "retardo.h"
#include "sala.h"

#define MAX_HILOS 10

int n_hilos[MAX_HILOS];
int asientos_reservados[3];

pthread_mutex_t cerrojo_condiciones = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condicion_reservar = PTHREAD_COND_INITIALIZER;
pthread_cond_t condicion_liberar = PTHREAD_COND_INITIALIZER;

void* ver_estado(void* arg) {
    while (1) {
        printf("Inicio de la revision \n");
        int capacidad = capacidad_sala();
        for (int i = 1; i < capacidad + 1; i++) {
            printf("Asiento %d %d \n", i, estado_asiento(i));
        }
        printf("Fin de la revision \n");
        sleep(5);
    }
    return NULL;
}

void* funcion_hito3_reservar(void* arg) {
    int index = *(int*)arg;
    pthread_mutex_lock(&cerrojo_condiciones);
    while (asientos_libres == 0){
        pthread_mutex_unlock(&cerrojo_condiciones);
        pthread_cond_wait(&condicion_reservar, &cerrojo_condiciones);
        pthread_mutex_lock(&cerrojo_condiciones);
    }
    asientos_reservados[0] = reserva_asiento(n_hilos[index]);
    pausa_aleatoria(3);
    asientos_reservados[1] = reserva_asiento(n_hilos[index]);
    pausa_aleatoria(3);
    asientos_reservados[2] = reserva_asiento(n_hilos[index]);
    pausa_aleatoria(3);
    
    pthread_mutex_unlock(&cerrojo_condiciones);
    pthread_cond_broadcast(&condicion_reservar);
    pthread_cond_broadcast(&condicion_liberar);
    return NULL;
}

void* funcion_hito3_liberar(void* arg) {
    int index = *(int*)arg;
    pthread_mutex_lock(&cerrojo_condiciones);
    while (asientos_ocupados == 0){
        pthread_mutex_unlock(&cerrojo_condiciones);
        pthread_cond_wait(&condicion_liberar, &cerrojo_condiciones);
        pthread_mutex_lock(&cerrojo_condiciones);
    }
    for (int i = 0; i < 3; i++){
      for (int i = 1; i < capacidad_sala(); i++){
        if (estado_asiento(i) > 0){
          libera_asiento(i);
          pausa_aleatoria(3);
        }
      }
    }
    /**
    libera_asiento(asientos_reservados[0]);
    pausa_aleatoria(3);
    libera_asiento(asientos_reservados[1]);
    pausa_aleatoria(3);
    libera_asiento(asientos_reservados[2]);
    pausa_aleatoria(3);
    **/
    pthread_mutex_unlock(&cerrojo_condiciones);
    pthread_cond_broadcast(&condicion_reservar);
    pthread_cond_broadcast(&condicion_liberar);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s multihilos <num_hilos> <num_hilos_liberar>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "multihilos") == 0) {
        crea_sala(10);
        pthread_t hilos[MAX_HILOS];
        pthread_t hilos_liberar[MAX_HILOS];
        pthread_t hilo_estado;
        int num_hilos = atoi(argv[2]);
        int num_hilos_liberar = atoi(argv[3]);
        int indices[MAX_HILOS];

        pthread_create(&hilo_estado, NULL, ver_estado, NULL);
        for (int i = 0; i < num_hilos; i++) {
            n_hilos[i] = i + 1;
            indices[i] = i;
            pthread_create(&hilos[i], NULL, funcion_hito3_reservar, (void*)&indices[i]);
        }

        for (int i = 0; i < num_hilos; i++) {
            pthread_join(hilos[i], NULL);
        }

        for (int i = 0; i < num_hilos_liberar; i++) {
            pthread_create(&hilos_liberar[i], NULL, funcion_hito3_liberar, (void*)&indices[i]);
        }

        for (int i = 0; i < num_hilos_liberar; i++) {
            pthread_join(hilos_liberar[i], NULL);
        }
        
        pthread_join(hilo_estado, NULL);

        elimina_sala();
    } else {
        fprintf(stderr, "Orden no válida\n");
    }

    return 0;
}
