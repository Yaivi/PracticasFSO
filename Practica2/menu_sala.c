#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "sala.h"

int comprobar_valor(int opcion_usuario){
  if (opcion_usuario <= 0){
    return -1;
  }else{
    return 0;
  }
}

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
	  printf("4. Mirar el número de asientos libres.\n");
	  printf("5. Mirar el número de asientos ocupados.\n");
	  printf("6. Mirar la capacidad de la sala.\n");
	  printf("7. Cerrar y eliminar sala (cerrar_sala).\n");
	  
	  fgets(input_usuario, sizeof(input_usuario), stdin); // Pide al usuario por pantalla que escriba la orden deseada
	  sscanf(input_usuario, "%d", &opcion_usuario); // Convierte lo escrito por el usuario a int
	  
	  switch (opcion_usuario){
	  	case 1:
	  		opcion_usuario = 0;
	  		printf("Ha seleccionado reservar asiento.\n");
	  		printf("Escriba el id de la persona que va a reservar el asiento.\n");
	  		fgets(input_usuario, sizeof(input_usuario), stdin);
	  		opcion_usuario = atoi(input_usuario);
	  		if (comprobar_valor(opcion_usuario) == -1){
	  		  printf("Valor introducido no válido.\n");
	  		  break;
	  		}
	  		int asiento_reservado = reserva_asiento(opcion_usuario);
	  		printf("El asiento reservado es el %d\n", asiento_reservado);
	  		break;
	  	case 2:
	  		opcion_usuario = 0;	  	
	  		printf("Ha seleccionado liberar asiento.\n");
	  		printf("Escriba el id del asiento a liberar.\n");
	  		fgets(input_usuario, sizeof(input_usuario), stdin);
	  		opcion_usuario = atoi(input_usuario);
	  		if (comprobar_valor(opcion_usuario) == -1){
	  		  printf("Valor introducido no válido.\n");
	  		  break;
	  		}
	  		int asiento_liberado = libera_asiento(opcion_usuario);
	  		printf("Se ha liberado el asiento %d", asiento_liberado);
	  		break;
	  	case 3:
                        opcion_usuario = 0;
	  		printf("Ha seleccionado mirar estado de un asiento.\n");
	  		printf("Escriba el asiento para mirar su estado.\n");
			fgets(input_usuario, sizeof(input_usuario), stdin);
	  		opcion_usuario = atoi(input_usuario);
	  		if (comprobar_valor(opcion_usuario) == -1){
	  		  printf("Valor introducido no válido.\n");
	  		  break;
	  		}
			printf("El estado del asiento %d es %d\n", opcion_usuario, estado_asiento(opcion_usuario));
	  		break;	  	
		case 4:
                        opcion_usuario = 0;
	  		printf("Ha seleccionado mirar el número de asientos libres.\n");
			printf("Hay %d asientos libres\n", asientos_libres());
	  		break;		  
		case 5:
                        opcion_usuario = 0;
	  		printf("Ha seleccionado mirar el número de asientos ocupados.\n");
			printf("Hay %d asientos ocupados.\n", asientos_ocupados());
	  		break;		
		case 6:
                        opcion_usuario = 0;
	  		printf("Ha seleccionado mirar la capacidad de la sala.\n");
			printf("Hay %d asientos en la sala.\n", capacidad_sala());
	  		break;		  
		case 7:
	  		opcion_usuario = 0;	  	
	  		printf("Ha seleccionado cerrar y eliminar sala.\n");
	  		elimina_sala();
	  		sleep(5); // Tiempo en s de espera antes de cerrar.
			exit(0);
	  		break;	  	
	  	default:
	  		printf("Opción inválida. Escriba un número válido.\n");
	  		break;
  	}
  }
  return 0;
}
