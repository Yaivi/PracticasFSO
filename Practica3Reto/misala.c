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



int main(int argc, char **argv){
  int opt;
  char* orden_opción = argv[1];
  char* ruta;
  int capacidad;
  int num_asientos = argc - 5;
  int fd;
  int *asientos;
  
  int flag_f = 0;
  int flag_o = 0;
  int flag_c = 0;
  int flag_p = 0;

  while ((opt = getopt (argc, argv, "f:c:o:p")) != -1) {
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
          ruta = malloc(strlen(optarg) + 1);       
          strcpy( ruta, optarg);
          break;  
      case 'p':
          flag_p = 1;
          int optind = 5;
          int i = 0;
          asientos=(int*)malloc(num_asientos*sizeof(int));
          while (optind < argc && argv[optind][0] != '-') {
            *(asientos + i) = atoi(argv[optind]);
            optind++;
            i++;
          }
          break;
      }
  } 
  
  
  if(strcmp(orden_opción, "crea") == 0 && flag_f == 1 && flag_c == 1){
    if ((fd = open(ruta, O_RDONLY)) == -1) {
      int resultado = crea_sala(capacidad);
      if (resultado == -1){
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
  
  else if(strcmp(orden_opción, "crea") == 0 && flag_o == 1){
    int resultado = crea_sala(capacidad);
    if (resultado == -1){
       return -1;
    }
    guarda_estado_sala(ruta);
    elimina_sala();
  }

	  
  else if (strcmp(orden_opción, "reserva") == 0 && flag_f == 1){
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
    if (crea_sala(capacidad) == -1) {
        comprobar_capacidad_en_misala();
        return -1;
    }
    recupera_estado_sala(ruta);
    int num_asientos = argc - 4;
    int num_asientos_validos = 0;
    int *lista_asientos = malloc(num_asientos * sizeof(int));
    if (num_asientos_validos > asientos_libres()) {
        fprintf(stderr, "No hay suficientes asientos libres, faltan %d asientos\n", num_asientos_validos - asientos_libres());
        fflush(stderr);
        errno = 0;
        free(lista_asientos);
        return -1;
    }
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

	  
  else if(strcmp(orden_opción, "anula") == 0 && flag_f == 1 && flag_p == 1){
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
    
    int num_personas_pasados = argc-5;
    int num_asientos_validos = 0;
    int *lista_asientos = malloc(num_asientos * sizeof(int));
    for (int j = 0;j<num_personas_pasados;j++) {
      int id_persona_actual = *(asientos + j);
      printf("persona %d\n", id_persona_actual);
      for (int i = 1; i < capacidad+1; i++){
          if (estado_asiento(i) == id_persona_actual) {
              lista_asientos[num_asientos_validos++] = i;
          }
      }
    }
    for (int i = 0; i<capacidad;i++) {
        printf("%d\n", lista_asientos[i]);
    }
    
    for (int i = 0; i < num_asientos_validos; i++) {
        libera_asiento(lista_asientos[i]);
    }

    if (guarda_estadoparcial_sala(ruta, num_asientos_validos * sizeof(int), lista_asientos) == -1) {
        comprobar_error_en_misala();
        free(lista_asientos);
        return -1;
    }
    free(lista_asientos);
    elimina_sala();
    return 0;
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

	  
  else{
    fprintf(stderr, "Orden no válida\n");
  }
  free(ruta);

  return 0;
}


