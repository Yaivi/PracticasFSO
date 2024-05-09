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
#include "sala.h"


#define MAX_HILOS 10

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



int main(int argc, char *argv[]){
  int capacidad;
  int fd;
  //crea_sala(capacidad); // Crea una capacidad con el valor de argv[2]
  
  char* orden_opción = argv[1];
  char* f = argv[2];
  char* ruta = argv[3];
  
  if(strcmp(orden_opción, "crea") == 0 && strcmp(f,"-f") == 0){
    if ((fd = open(ruta, O_RDONLY)) == -1) {
      capacidad = atoi(argv[5]);
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
  
  else if(strcmp(orden_opción, "crea") == 0 && strcmp(f,"-o") == 0){
    capacidad = atoi(argv[5]);
    crea_sala(capacidad);
    guarda_estado_sala(ruta);
    elimina_sala();
  }

	  
  else if (strcmp(orden_opción, "reserva") == 0 && strcmp(f,"-f") == 0){
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

	  
  else if(strcmp(orden_opción, "anula") == 0 && strcmp(f,"-f") == 0 && strcmp(argv[4], "-asientos") == 0){
    int fd = open(ruta, O_RDONLY);
    int contenido;
    
    contenido = read(fd, &capacidad, sizeof(int));
    if (contenido == -1) {
      fprintf(stderr, "Error %d al leer el archivo: \n", errno);
      exit(-1);
    }
    
    crea_sala(capacidad);
    recupera_estado_sala(ruta);
    
    int num_asientos_pasados = argc;
      
    for (int i = 5; i < num_asientos_pasados; i++){
          int asiento = atoi(argv[i]);
          libera_asiento(asiento);
    }

    guarda_estado_sala(ruta);
    elimina_sala();
    
    return 0;
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


