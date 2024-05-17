#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>
#include "retardo.h"
#include "sala.h"


#define MAX_HILOS 10

void* ver_estado(void* arg) {
    int capacidad = capacidad_sala();
    for (int i=1; i<capacidad+1; i++){
      printf("Asiento %d %d \n", i, estado_asiento(i));
    }
}
void* funcion_hito1(void* arg) {
  int *n_hilo = (int*) arg;
  int asiento1 = reserva_asiento(*n_hilo);
  pausa_aleatoria(3);
  int asiento2 = reserva_asiento(*n_hilo);
  pausa_aleatoria(3);
  int asiento3 = reserva_asiento(*n_hilo);
  pausa_aleatoria(3);
  libera_asiento(asiento1);
  pausa_aleatoria(3);
  libera_asiento(asiento2);
  pausa_aleatoria(3);
  libera_asiento(asiento3);
  pausa_aleatoria(3);

}

int main(int argc, char *argv[]) {
  if  (strcmp(argv[1], "multihilos") == 0){
      crea_sala(50);
      pthread_t hilos[MAX_HILOS];
      pthread_t hilo_estado;
      int num_hilos = atoi(argv[2]);
      int id_hilo[num_hilos];
      for (int i = 0; i<num_hilos; i++) {
          id_hilo[i] = i+1; 
          pthread_create(&hilos[i], NULL, funcion_hito1, (void*)&id_hilo[i]);
          pthread_create(&hilo_estado, NULL, ver_estado, NULL);
      }
      sleep(6);
      printf("iteracion, para revisar que no se queda limpio el archivo\n");
      ver_estado(NULL);

      for (int i = 0; i<num_hilos; i++) {
          pthread_join(hilos[i], NULL);
      }
      
  }
  else{
    fprintf(stderr, "Orden no válida\n");
  }
  return 0;
}
