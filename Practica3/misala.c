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
    if (fd == -1) {
        comprobar_error_en_misala();
        return -1;    
    }
    
    int capacidad;
    if (read(fd, &capacidad, sizeof(int)) == -1) {
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
    
    int num_asientos = argc - 4;
    int num_asientos_validos = 0;
    int *lista_asientos = malloc(num_asientos * sizeof(int));
    if (lista_asientos == NULL) {
        comprobar_error_en_misala();
        return -1;
    }
    
    for (int i = 4; i < argc; i++) {
        int id_persona = atoi(argv[i]);
        if (comprobar_valor_id_persona(id_persona, capacidad) == -1) {
            contador_valor_negativo++;
        } else {
            lista_asientos[num_asientos_validos++] = reserva_asiento(id_persona);
        }
    }
    
    if (num_asientos_validos > asientos_libres()) {
        fprintf(stderr, "No hay suficientes asientos libres, faltan %d asientos\n", num_asientos_validos - asientos_libres());
        fflush(stderr);
        errno = 0;
        free(lista_asientos);
        return -1;
    }
    
    guarda_estadoparcial_sala(ruta, num_asientos_validos * sizeof(int), lista_asientos);
    elimina_sala();
    free(lista_asientos);
    
    if (contador_valor_negativo > 0) {
        printf("Reserva completa pero con fallos\n");
    } else if (contador_valor_negativo == num_asientos) {
        printf("Reserva no completada\n");
    } else {
        printf("Reserva completada\n");
    }
}

  
  else if (strcmp(orden_opción, "anula") == 0 && strcmp(f, "-f") == 0 && strcmp(argv[4], "-asientos") == 0) {
    int fd = open(ruta, O_RDWR);
    if (fd == -1) {
        comprobar_error_en_misala();
        return -1;
    }
    
    int capacidad;
    if (read(fd, &capacidad, sizeof(int)) == -1) {
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
    
    int num_asientos = argc - 5;
    int contador_valor_negativo = 0;
    int *lista_asientos = malloc(num_asientos * sizeof(int));
    if (lista_asientos == NULL) {
        comprobar_error_en_misala();
        return -1;
    }
    
    int num_asientos_validos = 0;
    for (int i = 5; i < argc; i++) {
        int num_asiento = atoi(argv[i]);
        if (num_asiento < 1 || num_asiento > capacidad) {
            contador_valor_negativo++;
        } else {
            lista_asientos[num_asientos_validos++] = num_asiento;
        }
    }
    
    for (int i = 0; i < num_asientos_validos; i++) {
        libera_asiento(lista_asientos[i]);
    }
    
    if (guarda_estadoparcial_sala(ruta, num_asientos_validos * sizeof(int), lista_asientos) == -1) {
        comprobar_error_en_misala();
        free(lista_asientos);
        return -1;
    }
    
    elimina_sala();
    free(lista_asientos);
    
    if (contador_valor_negativo > 0 && contador_valor_negativo < num_asientos) {
        printf("Anulación completa pero con fallos\n");
    } else if (contador_valor_negativo == num_asientos) {
        printf("Anulación no completada\n");
    } else {
        printf("Anulación completada\n");
    }
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
