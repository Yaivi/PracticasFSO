#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "sala.h"


int leer_valor_usuario(char *input_usuario){
  int opcion_usuario;
  fgets(input_usuario, sizeof(input_usuario), stdin);
  opcion_usuario = atoi(input_usuario);
  return opcion_usuario;
}

int comprobar_valor(int opcion_usuario){
  if (opcion_usuario <= 0){
    return -1;
  }
    return 0;
}


int main(int argc, char *argv[]){
  int capacidad;
  //crea_sala(capacidad); // Crea una capacidad con el valor de argv[2]
  
  char* orden_opción = argv[1];
  char* f = argv[2];
  char* ruta = argv[3];

  for (int i = 0; i<argc; i++){
    printf("PARAM %d %s\n", i, argv[i]);
  }
  
  if(strcmp(orden_opción, "crea") == 0){
    capacidad = atoi(argv[5]);
  }

	  
  else if (strcmp(orden_opción, "reserva") == 0 && strcmp(f,"-f") == 0){
    
  }  

	  
  else if(strcmp(orden_opción, "anula") == 0 && strcmp(f,"-f") == 0 && strcmp(argv[4], "-asientos") == 0){
    int fd = open(ruta, O_RDONLY);
    int contenido;
    
    contenido = read(fd, &capacidad, sizeof(int));
    if (contenido == -1) {
      fprintf(stderr, "Error %d al leer el archivo: \n", errno);
      exit(-1);
    }
    close(fd);
    crea_sala(capacidad);
    
    int num_asientos_pasados = argc - 5;
    int asientos_fuera_de_rango = 0;
    int asientos_resultantes_a_anular = 0;
    int *asientos_a_anular = (int*)malloc(num_asientos_pasados * sizeof(int));
    if (asientos_a_anular == NULL){
      return -1;
    }
    int *asientos_a_fuera_rango_vector = (int*)malloc(num_asientos_pasados * sizeof(int));
    int index = 0;
    
    for (int i = 0; i < num_asientos_pasados; i++){
      if (comprobar_valor_id_asiento(atoi(argv[5+i]), capacidad) == 0){
        *(asientos_a_anular + index) = atoi(argv[5+i]);
        index++;
      } else {
        printf("ATOI %d\n", atoi(argv[5+i]));
        printf("LUGAR: %d\n", asientos_fuera_de_rango);
        asientos_a_fuera_rango_vector[asientos_fuera_de_rango] = atoi(argv[5+i]);
        asientos_fuera_de_rango++;
      }
    }
    
    asientos_resultantes_a_anular = num_asientos_pasados - asientos_fuera_de_rango;     
    guarda_estadoparcial_sala(ruta, asientos_resultantes_a_anular, asientos_a_anular);

    if (asientos_fuera_de_rango > 0){
      fprintf(stderr, "No se han podido guardar estos asientos: ");
      for (int i = 0; i < asientos_fuera_de_rango; i++){
        printf("%d ", asientos_a_fuera_rango_vector[i]);
      }
      printf(" \n");
    }
    //Libera la memoria del vector auxiliar para los asientos a anular
    free(asientos_a_anular);
  }

	  
  else if(strcmp(orden_opción, "estado") == 0 && strcmp(f,"-f") == 0){
    int fd = open(ruta, O_RDONLY);
    int contenido;
    
    contenido = read(fd, &capacidad, sizeof(int));
    if (contenido == -1) {
      fprintf(stderr, "Error %d al leer el archivo: \n", errno);
      exit(-1);
    }
    close(fd);
    crea_sala(capacidad);
    
    recupera_estado_sala(ruta);
    printf("Capacidad de la sala: %d\n", capacidad_sala());
    printf("Asientos Ocupados: %d\n", asientos_ocupados());
    printf("Asientos libres: %d\n", asientos_libres());
    for (int i=1; i<capacidad+1; i++){
      printf("Asiento %d %d \n", i, estado_asiento(i));
    }
  }

	  
  else{
    fprintf(stderr, "Orden no válida\n");
  }
  return 0;
}  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  /**
  printf("---Bienvenido a la sala en %s---\n", argv[1]);
  while (1){
	  printf("\nEscriba el número de la instrucción: \n");
	  printf("1. Reserva asiento (reserva <id-persona>).\n");
	  printf("2. Liberar asiento (libera <id-asiento>).\n");
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
                        opcion_usuario = leer_valor_usuario(input_usuario);
	  		if (comprobar_valor(opcion_usuario) == -1){
	  		  printf("Valor introducido no válido.\n");
	  		  break;
	  		}
	  		int asiento_reservado = reserva_asiento(opcion_usuario);
	  		printf("El asiento reservado es el número %d.\n", asiento_reservado);
	  		break;
	  	case 2:
	  		opcion_usuario = 0;	  	
	  		printf("Ha seleccionado liberar asiento.\n");
	  		printf("Escriba el id del asiento a liberar.\n");
                        opcion_usuario = leer_valor_usuario(input_usuario);
	  		if (comprobar_valor(opcion_usuario) == -1){
	  		  printf("Valor introducido no válido.\n");
	  		  break;
	  		}
	  		int asiento_liberado = libera_asiento(opcion_usuario);
	  		printf("Se ha liberado el asiento número %d, que pertenecía a la persona %d.", opcion_usuario, asiento_liberado);
	  		break;
	  	case 3:
                        opcion_usuario = 0;
	  		printf("Ha seleccionado mirar estado de un asiento.\n");
	  		printf("Escriba el asiento para mirar su estado.\n");
                        opcion_usuario = leer_valor_usuario(input_usuario);
	  		if (comprobar_valor(opcion_usuario) == -1){
	  		  printf("Valor introducido no válido.\n");
	  		  break;
	  		}
			printf("El estado del asiento número %d es %d.\n", opcion_usuario, estado_asiento(opcion_usuario));
	  		break;	  	
		case 4:
                        opcion_usuario = 0;
	  		printf("Ha seleccionado mirar el número de asientos libres.\n");
			printf("Hay %d asientos libres actualmente. \n", asientos_libres());
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
**/
