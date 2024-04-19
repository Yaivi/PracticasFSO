#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "sala.h"
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

int *asientos;
int CAPACIDAD_MAXIMA = -1;
int asientos_libres_variable = -1;
int asientos_ocupados_variable = -1;
#define MAX 1000
int num_asientos = 0;

int comprueba_id_persona(int id_persona){
	if (id_persona <= 0){
		return -1;
	}
	return 0;
}

int comprueba_sala(){
	if (asientos_libres < 0){
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
        if (comprueba_sala() == -1){
                return -1; 
        }
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
        if (comprueba_sala() == -1){
                return -1; 
        }
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
        if (comprueba_sala() == -1){
                return -1; 
        }
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
        if (comprueba_sala() == -1){
                return -1; 
        }
	return asientos_libres_variable;
}


int asientos_ocupados(){
        if (comprueba_sala() == -1){
                return -1; 
        }
        return asientos_ocupados_variable;
} 


int capacidad_sala(){
        if (comprueba_sala() == -1){
                return -1; 
        }
	return CAPACIDAD_MAXIMA;
}


int crea_sala(int capacidad){
	CAPACIDAD_MAXIMA = capacidad;
	asientos_libres_variable = CAPACIDAD_MAXIMA;
	asientos_ocupados_variable = 0;

  if (CAPACIDAD_MAXIMA <= 0){
	  return -1;
	}
        
  asientos=(int*)malloc(CAPACIDAD_MAXIMA*sizeof(int));
  
	if (asientos == NULL){
	    return -1;
	}
	
	for (int count = 1; count <= CAPACIDAD_MAXIMA; count++){
		*(asientos + count - 1) = -1;
	}
	return CAPACIDAD_MAXIMA;
}


int elimina_sala(){
	if (asientos == NULL || CAPACIDAD_MAXIMA <= 0){
	  return -1;
	}
	free(asientos);
	CAPACIDAD_MAXIMA = -1;
	asientos_libres_variable = -1;
	asientos_ocupados_variable = -1;
	return 0;
}


int coger_tamaño_bloque(const char* ruta_fichero) {
    struct stat estado;
    if (fstat(ruta_fichero, &estado) == -1) {
        fprintf(stderr, "Error al acceder al estado del archivo\n");
        exit(-1);
    }
    return estado.st_blksize;
}

int guarda_estado_sala(const char* ruta_fichero){
    if (comprueba_sala() == -1){
      printf("Error: no se ha creado una sala");
      return -1;
    }
    int fd = open(ruta_fichero, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    ssize_t bytes_escritos;
    if (fd == -1) {
        fprintf(stderr, "Error al crear el archivo\n");
        exit(EXIT_FAILURE);
    }
    int tam_bloque  = coger_tamaño_bloque(ruta_fichero);
    
    // Guardar en el fichero CAPACIDAD_MAXIMA
    bytes_escritos = write(fd, &CAPACIDAD_MAXIMA, tam_bloque);
    if (bytes_escritos == -1) {
        fprintf(stderr, "Error %d al escribir en el archivo\n", errno);
        exit(-1);
    }
    
    // Guardar en el fichero asientos_ocupados_variable
    bytes_escritos = write(fd, &asientos_libres_variable, tam_bloque);
    if (bytes_escritos == -1) {
        fprintf(stderr, "Error %d al escribir en el archivo\n", errno);
        exit(-1);
    }    

    // Guardar en el fichero asientos_libres_variable
    bytes_escritos = write(fd, &asientos_ocupados_variable, tam_bloque);
    if (bytes_escritos == -1) {
        fprintf(stderr, "Error %d al escribir en el archivo\n", errno);
        exit(-1);
    }

    // Guardar en el fichero el vector asientos
    bytes_escritos = write(fd, asientos, sizeof(int)*CAPACIDAD_MAXIMA);
    if (bytes_escritos == -1) {
        fprintf(stderr, "Error %d al escribir en el archivo\n", errno);
        exit(-1);
    }

    close(fd);
    return 0;
}


int recupera_estado_sala(const char* ruta_fichero){
    int nueva_CAPACIDAD_MAXIMA;
    int fd = open(ruta_fichero, O_RDONLY);
    int contenido;
    
    contenido = read(fd, &nueva_CAPACIDAD_MAXIMA, sizeof(int));
    if (contenido == -1) {
      fprintf(stderr, "Error %d al leer el archivo: \n", errno);
      exit(-1);
    }
    if (nueva_CAPACIDAD_MAXIMA != CAPACIDAD_MAXIMA){
      exit(-1);
    }
    contenido = read(fd, &asientos_libres_variable, sizeof(int));
    if (contenido == -1) {
      fprintf(stderr, "Error %d al leer el archivo: \n", errno);
      exit(-1);
    }
    contenido = read(fd, &asientos_ocupados_variable, sizeof(int));
    if (contenido == -1) {
      fprintf(stderr, "Error %d al leer el archivo: \n", errno);
      exit(-1);
    }

    close(fd);
    return 0;
}


int guarda_estadoparcial_sala(const char* ruta_fichero,	size_t num_asientos, int* id_asientos){
    int fd;
    ssize_t bytes_escritos;
    
    fd = open(ruta_fichero, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    
    bytes_escritos = write(fd, asientos, sizeof(int)*CAPACIDAD_MAXIMA);
    if (bytes_escritos == -1) {
        fprintf(stderr, "Error %d al escribir en el archivo\n", errno);
        exit(EXIT_FAILURE);
    return 0;
  }
}

int recupera_estadoparcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos){
    return 0;
}


int main(){
  int espacio = 10;
  crea_sala(espacio);
  guarda_estado_sala("c/Practica03/prueba.txt");
  elimina_sala();
  
  espacio = 10;
  crea_sala(espacio);
  reserva_asiento(10);
  reserva_asiento(20);
  reserva_asiento(30);
  reserva_asiento(40);
  guarda_estado_sala("c/Practica03/prueba.txt");
  elimina_sala();

  printf("Capacidad Máxima: %d \n",capacidad_sala()); 
  printf("Asientos Libres: %d \n",asientos_libres());
  printf("Asientos Ocupados: %d \n",asientos_ocupados());
  
  espacio = 10;
  crea_sala(espacio);  
  recupera_estado_sala("c/Practica03/prueba.txt");
  printf("Capacidad Máxima: %d \n",capacidad_sala()); 
  printf("Asientos Libres: %d \n",asientos_libres());
  printf("Asientos Ocupados: %d \n",asientos_ocupados()); 
  return 0; 
}
