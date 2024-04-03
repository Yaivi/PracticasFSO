#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "sala.h"

int *asientos;
int CAPACIDAD_MAXIMA = -1;
int asientos_libres_variable = -1;
int asientos_ocupados_variable = -1;


int comprueba_id_persona(int id_persona){
	if (id_persona <= 0){
		return -1;
	}
	return 0;
}


int comprueba_id_asiento(int id_asiento){
	if (id_asiento < 0){
		return -1;
	}

        if (id_asiento > CAPACIDAD_MAXIMA){
		return -1;
	}
	return 0;
}


int reserva_asiento(int id_persona){
	// Si el id_persona es menor que 0 dará error.
	if (comprueba_id_persona(id_persona) == -1){
		return -1;
	}
	// Si no hay ningún asiento libre dará error.
	if (asientos_libres_variable == 0){
		return -1;
	}
	for (int count = 1; count<=CAPACIDAD_MAXIMA; count++){
		if (*(asientos + count -1) == -1){
			asientos_ocupados_variable++;
			asientos_libres_variable--;
			*(asientos + count -1) = id_persona;
			return count;
		}
	}
}


int libera_asiento(int id_asiento){
        if (comprueba_id_asiento(id_asiento) == -1){
                return -1;
        }
	int id_persona_anterior = *(asientos + id_asiento -1);
	*(asientos + id_asiento -1) = -1;
        asientos_ocupados_variable--;
        asientos_libres_variable++;
        return id_persona_anterior;
}


int estado_asiento(int id_asiento){
        if (comprueba_id_asiento(id_asiento)){
                return -1;
        }
	if (*(asientos + id_asiento -1) != -1){
		return *(asientos + id_asiento -1);
	}
	else{
		return 0;
	}
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
	asientos_ocupados_variable = 0;
	asientos=(int*)malloc(CAPACIDAD_MAXIMA*sizeof(int));
	
	if (asientos == NULL){
	    return -1;
	}
	
	if (CAPACIDAD_MAXIMA <= 0){
	  return -1;
	}
	
	for (int count = 1; count <= CAPACIDAD_MAXIMA; count++){
		*(asientos + count - 1) = -1;
	}
	return CAPACIDAD_MAXIMA;
}


int elimina_sala(){
	if (asientos == NULL){
	        free(asientos);
		return -1;
	}
	free(asientos);
	CAPACIDAD_MAXIMA = -1;
	asientos_libres_variable = -1;
	asientos_ocupados_variable = -1;
	return 0;
}

int main(){
  
  char input_usuario[100];
  
  printf("\nEscriba el número de la instrucción: \n");
  printf("1. Reserva asiento (reserva <id-persona>).\n");
  printf("2. Liberar asiento (libera <id-persona>).\n");
  printf("3. Estado del asiento (estado_asiento <id-asiento>).\n");
  printf("4. Cerrar y eliminar sala (cerrar_sala).\n");
  
  fgets(input_usuario, 100, stdin); // Pide al usuario por pantalla que escriba la orden deseada
  input_usuario[strcspn(input_usuario, "\n")] = '\0';

  //int input_usuario_int = input_usuario - '0'; 
  
  printf("Orden: %s\n", input_usuario);
  if (input_usuario == "1"){
    printf("Indice el número del asiento");
  }
  if (input_usuario == "2"){
  }
  if (input_usuario == "3"){
  }
  if (input_usuario == "4"){
  }
  
  return 0;
}
