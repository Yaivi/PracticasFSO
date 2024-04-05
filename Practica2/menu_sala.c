#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "sala.h"


int main(int argc, char *argv[]){
  int capacidad;
  capacidad = atoi(argv[2]);
  crea_sala(capacidad); // Crea una capacidad con el valor de argv[1]
  
  char input_usuario[100];
  int opcion_usuario;
  
  printf("---Bienvenido a la sucursal en %s---\n", argv[1]);
  while (1){
	  printf("\nEscriba el número de la instrucción: \n");
	  printf("1. Reserva asiento (reserva <id-persona>).\n");
	  printf("2. Liberar asiento (libera <id-persona>).\n");
	  printf("3. Estado del asiento (estado_asiento <id-asiento>).\n");
	  printf("4. Cerrar y eliminar sala (cerrar_sala).\n");
	  
	  fgets(input_usuario, sizeof(input_usuario), stdin); // Pide al usuario por pantalla que escriba la orden deseada
	  sscanf(input_usuario, "%d", &opcion_usuario); // Convierte lo escrito por el usuario a int
	  
	  switch (opcion_usuario){
	  	case 1:
	  		opcion_usuario = 0;
	  		printf("Ha seleccionado reservar asiento.\n");
	  		printf("Escriba el id de la persona que va a reservar el asiento.\n");
	  		fgets(input_usuario, sizeof(input_usuario), stdin);
	  		sscanf(input_usuario, "%d", &opcion_usuario);
	  		int asiento_reservado = reserva_asiento(opcion_usuario);
	  		printf("El asiento reservado es el %d\n", asiento_reservado);
	  		break;
	  	case 2:
	  		opcion_usuario = 0;	  	
	  		printf("Ha seleccionado liberar asiento.\n");
	  		printf("Escriba el id del asiento a liberar.\n");
	  		fgets(input_usuario, sizeof(input_usuario), stdin);
	  		sscanf(input_usuario, "%d", &opcion_usuario);
	  		int asiento_liberado = libera_asiento(opcion_usuario);
	  		printf("%d", asiento_liberado);
	  		break;
	  	case 3:
                        opcion_usuario = 0;
	  		printf("Ha seleccionado mirar estado de un asiento.\n");
	  		printf("Escriba el asiento para mirar su estado.\n");
	  		break;	  	
	  	case 4:
	  		opcion_usuario = 0;	  	
	  		printf("Ha seleccionado cerrar y eliminar sala.\n");
	  		elimina_sala();
	  		sleep(10); // Tiempo en s de espera antes de cerrar.
			exit(0);
	  		break;	  	
	  	default:
	  		printf("Opción inválida. Escriba un número válido.\n");
	  		break;
  	}
  }
  return 0;
}
