#include <stdio.h>
#include <stdlib.h>

int *asientos;
int CAPACIDAD_MAXIMA;
int asientos_libres_variable;
int asientos_ocupados_variable;


int reserva_asiento(int id_persona){
}

int libera_asiento(int id_asiento){
  *(asientos + id_asiento) = -1;
  asientos_ocupados_variable--;
}

int estado_asiento(int id_asiento){
  
}

int asientos_libres(){
  return asientos_libres_variable;
}

int asientos_ocupados(){
  return asientos_ocupados_variable;
} 


int capacidad_sala(){
  return CAPACIDAD_MAXIMA;
}

int crea_sala(int capacidad){
  CAPACIDAD_MAXIMA = capacidad;
  asientos_libres_variable = CAPACIDAD_MAXIMA;
  asientos=(int*)malloc(CAPACIDAD_MAXIMA*sizeof(int));
  if (asientos == NULL){
    exit(-1);
  }
  for (int count = 1; count <= CAPACIDAD_MAXIMA; count++){
    *(asientos + count) = -1;
  }
  return 0;
}

int elimina_sala(){
  if (asientos == NULL){
    return -1;
  }
  free(asientos);
  return 0;
}
