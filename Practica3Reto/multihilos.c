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
int asientos_reservados[] = {0, 0, 0};

void* ver_estado(void* arg) {
    int capacidad = capacidad_sala();
    for (int i=1; i<capacidad+1; i++){
      printf("Asiento %d %d \n", i, estado_asiento(i));
    }
    return NULL;
}
void* funcion_hito3_reservar(void* arg) {
  int *n_hilo = (int*) arg;
  int asiento1 = reserva_asiento(*n_hilo);
  asientos_reservados[0] = asiento1;
  pausa_aleatoria(3);
  int asiento2 = reserva_asiento(*n_hilo);
  asientos_reservados[1] = asiento2;
  pausa_aleatoria(3);
  int asiento3 = reserva_asiento(*n_hilo);
  asientos_reservados[2] = asiento3;
  pausa_aleatoria(3);
  return NULL;
}


void* funcion_hito3_liberar(void* arg) {
  libera_asiento(asientos_reservados[0]);
  pausa_aleatoria(3);
  libera_asiento(asientos_reservados[1]);
  pausa_aleatoria(3);
  libera_asiento(asientos_reservados[2]);
  pausa_aleatoria(3);
  return NULL;
}


int main(int argc, char *argv[]) {
  if  (strcmp(argv[1], "multihilos") == 0){
      crea_sala(30);
      pthread_t hilos[MAX_HILOS];
      pthread_t hilo_estado;
      int num_hilos = atoi(argv[2]);
      int num_hilos_liberar = atoi(argv[3]);
      int id_hilo[num_hilos];
      for (int i = 0; i<num_hilos; i++) {
          id_hilo[i] = i+1; 
          pthread_create(&hilos[i], NULL, funcion_hito3_reservar, (void*)&id_hilo[i]);
      }
      for (int i = 0; i<num_hilos_liberar; i++) {
          id_hilo[i] = i+1; 
          pthread_create(&hilos[i], NULL, funcion_hito3_liberar, (void*)&id_hilo[i]);
      }
      sleep(20);
      printf("iteracion, para revisar que se queda limpio el archivo\n");
      pthread_create(&hilo_estado, NULL, ver_estado, NULL);
      pthread_join(hilo_estado, NULL);

      for (int i = 0; i<num_hilos; i++) {
          pthread_join(hilos[i], NULL);
          
      }
      elimina_sala();
  }
  else{
    fprintf(stderr, "Orden no válida\n");
  }
  return 0;
}
