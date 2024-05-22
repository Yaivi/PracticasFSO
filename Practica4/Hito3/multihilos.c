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

typedef struct {
    int n_hilo;
    int asientos_reservados[3];
} hilo_data;

void* ver_estado(void* arg) {
    int capacidad = capacidad_sala();
    for (int i = 1; i < capacidad + 1; i++) {
        printf("Asiento %d %d \n", i, estado_asiento(i));
    }
    return NULL;
}

void* funcion_hito3_reservar(void* arg) {
    hilo_data* data = (hilo_data*)arg;
    data->asientos_reservados[0] = reserva_asiento(data->n_hilo);
    pausa_aleatoria(3);
    data->asientos_reservados[1] = reserva_asiento(data->n_hilo);
    pausa_aleatoria(3);
    data->asientos_reservados[2] = reserva_asiento(data->n_hilo);
    pausa_aleatoria(3);
    return NULL;
}

void* funcion_hito3_liberar(void* arg) {
    hilo_data* data = (hilo_data*)arg;
    libera_asiento(data->asientos_reservados[0]);
    pausa_aleatoria(3);
    libera_asiento(data->asientos_reservados[1]);
    pausa_aleatoria(3);
    libera_asiento(data->asientos_reservados[2]);
    pausa_aleatoria(3);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s multihilos <num_hilos> <num_hilos_liberar>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "multihilos") == 0) {
        crea_sala(30);
        pthread_t hilos[MAX_HILOS];
        pthread_t hilos_liberar[MAX_HILOS];
        pthread_t hilo_estado;
        int num_hilos = atoi(argv[2]);
        int num_hilos_liberar = atoi(argv[3]);
        hilo_data hilos_data[MAX_HILOS];

        for (int i = 0; i < num_hilos; i++) {
            hilos_data[i].n_hilo = i + 1;
            pthread_create(&hilos[i], NULL, funcion_hito3_reservar, (void*)&hilos_data[i]);
        }

        for (int i = 0; i < num_hilos; i++) {
            pthread_join(hilos[i], NULL);
        }

        for (int i = 0; i < num_hilos_liberar; i++) {
            pthread_create(&hilos_liberar[i], NULL, funcion_hito3_liberar, (void*)&hilos_data[i]);
        }

        for (int i = 0; i < num_hilos_liberar; i++) {
            pthread_join(hilos_liberar[i], NULL);
        }

        printf("Iteracion, para revisar que se queda limpio el archivo\n");
        pthread_create(&hilo_estado, NULL, ver_estado, NULL);
        pthread_join(hilo_estado, NULL);

        elimina_sala();
    } else {
        fprintf(stderr, "Orden no válida\n");
    }

    return 0;
}
