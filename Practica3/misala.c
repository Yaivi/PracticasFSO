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

int comprobar_valor_id_persona(int opcion_usuario, int capacidad){
  if (opcion_usuario <= 0){
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

int comprobar_capacidad_en_misala(){
  fprintf(stderr, "Se ha pasado como parámetro una capacidad negativa o 0\n");
  return -1;
}


int main(int argc, char *argv[]){
  int capacidad;
  int fd;
  
  char* orden_opción = argv[1];
  char* f = argv[2];
  char* ruta = argv[3];
  
  if(strcmp(orden_opción, "crea") == 0 && strcmp(f,"-f") == 0){
    if ((fd = open(ruta, O_RDONLY)) == -1) {
      capacidad = atoi(argv[5]);
      crea_sala(capacidad);
      if (crea_sala(capacidad) == -1){
        return -1;
      }
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
    if (crea_sala(capacidad) == -1){
      comprobar_capacidad_en_misala();
      return -1;
    }
    guarda_estado_sala(ruta);
    elimina_sala();
  }

	  
else if (strcmp(orden_opción, "reserva") == 0 && strcmp(f, "-f") == 0) {
    int contador_valor_negativo = 0;
    int fd = open(ruta, O_RDWR);
    int contenido;
    if (fd == -1) {
        comprobar_error_en_misala();
        return -1;    
    }
    contenido = read(fd, &capacidad, sizeof(int));
    if (contenido == -1) {
        comprobar_error_en_misala();
        close(fd);
        return -1;  
    }
    close(fd);
    
    if (crea_sala(capacidad) == -1) {
        comprobar_capacidad_en_misala();
        return -1;
    }
    recupera_estado_sala(ruta);
    int num_asientos = argc-4;
    
    for (int i = 4; i < argc; i++) {
        int id_persona = atoi(argv[i]);
        if (comprobar_valor_id_persona(id_persona, capacidad) == -1) {
          num_asientos -= 1;
        }
    }

    if (num_asientos > asientos_libres()) {
        fprintf(stderr, "No hay suficientes asientos libres, faltan %d asientos\n", argc - 4 - asientos_libres());
        fflush(stderr);
        errno = 0;
        return -1;
    }
    
    int *lista_asientos = malloc((argc - 4) * sizeof(int));
    if (lista_asientos == NULL) {
        comprobar_error_en_misala();
        return -1;
    }
    
    int index = 0;
    for (int i = 4; i < argc; i++) {
        int id_persona = atoi(argv[i]);
        if (comprobar_valor_id_persona(id_persona, capacidad) == -1) {
            contador_valor_negativo++;
        } else {
            lista_asientos[index] = reserva_asiento(id_persona);
            index++;
        }
    }
    
    int num_asientos_a_reservar = argc - 4 - contador_valor_negativo;
    int *lista_asientos_a_reservar = malloc(num_asientos_a_reservar * sizeof(int));
    if (lista_asientos_a_reservar == NULL) {
        comprobar_error_en_misala();
        free(lista_asientos);
        return -1;
    }
    
    for (int i = 0; i < num_asientos_a_reservar; i++) {
        lista_asientos_a_reservar[i] = lista_asientos[i];
    }
    
    guarda_estadoparcial_sala(ruta, num_asientos_a_reservar * sizeof(int), lista_asientos_a_reservar);
    elimina_sala();
    
    free(lista_asientos);
    free(lista_asientos_a_reservar);
    
    if (contador_valor_negativo > 0 && contador_valor_negativo < argc - 4) {
        printf("Reserva completa pero con fallos\n");
    } else if (contador_valor_negativo == argc - 4) {
        printf("Reserva no completada\n");
    } else {
        printf("Reserva completada\n");
    }
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
    int asientos_realmente_a_guardar = 0;
    
    int *lista_asientos = malloc((argc-4)*sizeof(int));
    
    for (int i = 5; i < num_asientos_pasados; i++){
          int asiento = atoi(argv[i]);
          if (comprobar_valor_id_asiento(asiento, capacidad) == -1){
            printf("Asiento no válido\n");
          }
          libera_asiento(asiento);
          *(lista_asientos + asientos_realmente_a_guardar) = asiento;
          asientos_realmente_a_guardar++;
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
        comprobar_error_en_misala();
        return -1;  
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
