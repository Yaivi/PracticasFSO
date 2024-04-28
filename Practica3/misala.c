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


int comprobar_valor_id_asiento(int opcion_usuario, int capacidad){
  if (opcion_usuario <= 0 || opcion_usuario > capacidad){
    return -1;
  }
    return 0;
}

int comprobar_error_en_misala(){
	if (errno != 0){
		fprintf(stderr, "Se ha producido un error con código %d: %s", errno, strerror(errno));
		fflush(stderr);
		errno = 0;
		return -1;
	}
	return 0;
}



int main(int argc, char **argv){
  int opt;
  char* orden_opción = argv[1];
  char* ruta;
  int capacidad;
  int asiento;
  int fd;
  
  int flag_f = 0;
  int flag_o = 0;
  int flag_c = 0;
  int flag_a = 0;

  while ((opt = getopt (argc, argv, "f:c:o:a")) != -1) {
      switch(opt) {
      case 'f':
          flag_f = 1;
          ruta = malloc(strlen(optarg) + 1);
          strcpy( ruta, optarg);
          break;
      case 'c':
          flag_c = 1;
          capacidad = atoi(optarg);
          break;
      case 'o':
          flag_o = 1;
          ruta = malloc(strlen(optarg) + 1); // Asignar memoria para la ruta        
          strcpy( ruta, optarg);
          break;  
      case 'a':
          flag_a = 1;
          asiento = atoi(optarg);
          break;     
      } 
  } 
  
  
  if(strcmp(orden_opción, "crea") == 0 && flag_f == 1 && flag_c == 1){
    if ((fd = open(ruta, O_RDONLY)) == -1) {
      crea_sala(capacidad);
      guarda_estado_sala(ruta);
      elimina_sala();
      printf("Se ha creado el archivo con los datos pasados\n");
    } else {
      fprintf(stderr, "Este archivo ya existe, para sobreescribirlo ponga -o en vez de -f\n");
      fflush(stderr);
      errno = 0;
      return -1; 
    }
  }
  
  else if(strcmp(orden_opción, "crea") == 0 && flag_o == 1){
    crea_sala(capacidad);
    guarda_estado_sala(ruta);
    elimina_sala();
  }

	  
  else if (strcmp(orden_opción, "reserva") == 0 && flag_f == 1){
    int fd = open(ruta, O_RDWR);
    int contenido;
    if (fd == -1) {
        comprobar_error_en_misala();
        return -1;    
    }
    contenido = read(fd, &capacidad, sizeof(int));
    if (contenido == -1) {
        comprobar_error_en_misala();
        return -1;  
    }
    crea_sala(capacidad);
    recupera_estado_sala(ruta);
    if (argc-3 > asientos_libres()) {
        fprintf(stderr, "No hay suficientes asientos libres, faltan %d asientos", argc-3-asientos_libres());
        fflush(stderr);
        errno = 0;
        return -1;
    }
    for (int i = 4; i<argc; i++) {
        reserva_asiento(atoi(argv[i]));
    }
    guarda_estado_sala(ruta);
    elimina_sala();
    printf("Reserva completa\n");
  }   

	  
  else if(strcmp(orden_opción, "anula") == 0 && flag_f == 1 && flag_a == 1){
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

	  
  else if(strcmp(orden_opción, "estado") == 0 && flag_f == 1){
    fd = open(ruta, O_RDONLY);
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
  
  else if(strcmp(orden_opción, "compara") == 0){
    char* ruta1 = argv[2];
    char* ruta2 = argv[3];
    
    fd = open(ruta1, O_RDONLY);
    int contenido;
    
    contenido = read(fd, &capacidad, sizeof(int));
    if (contenido == -1) {
      fprintf(stderr, "Error %d al leer el archivo: \n", errno);
      exit(-1);
    }
    close(fd);
    crea_sala(capacidad);
    size_t capacidad_sala1 = capacidad_sala();
    int* asientos_sala1;
    recupera_estadoparcial_sala(ruta, capacidad_sala1, asientos_sala1);
    
    
    
    
    int fd = open(ruta1, O_RDONLY);
    contenido = read(fd, &capacidad, sizeof(int));
    if (contenido == -1) {
      fprintf(stderr, "Error %d al leer el archivo: \n", errno);
      exit(-1);
    }
    close(fd);
    crea_sala(capacidad);
    
    recupera_estado_sala(ruta);
    int capacidad_sala2 = capacidad_sala();
    int asientos_libres_sala2 = asientos_libres();

  
  }

  else if(strcmp(orden_opción, "compara") == 0){
    char* ruta1 = argv[3];
    char* ruta2  = argv[4];
    int fd1 = open(ruta1, O_RDONLY);
    int fd2 = open(ruta2, O_RDONLY);
    int contenido1;
    int contenido2;
    int capacidad1;
    int capacidad2;
    int asientos_libres1;
    int asientos_libres2;
    int asientos_ocupados1;
    int asientos_ocupados2;

    contenido1 = read(fd1, &capacidad1, sizeof(int));
    if (contenido1 == -1) {
        comprobar_error_en_misala();
        return -1;
    }
    contenido1 = read(fd1, &asientos_libres1, sizeof(int));
    if (contenido1 == -1) {
        comprobar_error_en_misala();
        return -1;
    }
    contenido1 = read(fd1, &asientos_ocupados1, sizeof(int));
    if (contenido1 == -1) {
        comprobar_error_en_misala();
        return -1;
    }
    close(fd1);

    contenido2 = read(fd1, &capacidad2, sizeof(int));
    if (contenido2 == -1) {
        comprobar_error_en_misala();
        return -1;
    }
    contenido2 = read(fd1, &asientos_libres2, sizeof(int));
    if (contenido2 == -1) {
        comprobar_error_en_misala();
        return -1;
    }
    contenido2 = read(fd1, &asientos_ocupados2, sizeof(int));
    if (contenido2 == -1) {
        comprobar_error_en_misala();
        return -1;
    }
    close(fd2);


    if (capacidad1 != capacidad2){
      fprintf(stderr, "Las salas pasadas no tienen la misma capacidad");
    }
    if (asientos_libres1 != asientos_libres2){
      fprintf(stderr, "Las salas pasadas no tienen el mismo número de asientos libres");
    }

    if (asientos_ocupados1 != asientos_ocupados2){
      fprintf(stderr, "Las salas pasadas no tienen el mismo número de asientos ocupados");
    }
  }


  else{
    fprintf(stderr, "Orden no válida\n");
  }
  free(ruta);
  return 0;
}
