#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "sala.h"
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <errno.h>

int *asientos;
int CAPACIDAD_MAXIMA = -1;
int asientos_libres_variable = -1;
int asientos_ocupados_variable = -1;
#define MAX 1000
int num_asientos = 0;
pthread_mutex_t cerrojo = PTHREAD_MUTEX_INITIALIZER;
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


int comprobar_error(){
	if (errno != 0){
		fprintf(stderr, "Se ha producido un error con código %d: %s", errno, strerror(errno));
		fflush(stderr);
		errno = 0;
		return -1;
	}
	return 0;
}


int reserva_asiento(int id_persona){
        pthread_mutex_lock(&cerrojo);
        if (comprueba_sala() == -1){
                pthread_mutex_unlock(&cerrojo);
                return -1; 
        }
	// Si el id_persona es menor que 0 dará error.
	if (comprueba_id_persona(id_persona) == -1){
		pthread_mutex_unlock(&cerrojo);
		return -1;
	}
	// Si no hay ningún asiento libre dará error.
	if (asientos_libres_variable == 0){
	        pthread_mutex_unlock(&cerrojo);
		return -1;
	}
	for (int count = 1; count<=CAPACIDAD_MAXIMA; count++){
		if (*(asientos + count -1) == -1){
			asientos_ocupados_variable++;
			asientos_libres_variable--;
			*(asientos + count -1) = id_persona;
			pthread_mutex_unlock(&cerrojo);
			return count;
		}
	}
}


int libera_asiento(int id_asiento){
        pthread_mutex_lock(&cerrojo);
        if (comprueba_sala() == -1){
                pthread_mutex_unlock(&cerrojo);
                return -1; 
        }
        if (comprueba_id_asiento(id_asiento) == -1){
                pthread_mutex_unlock(&cerrojo);
                return -1;
        }
	int id_persona_anterior = *(asientos + id_asiento -1);
	*(asientos + id_asiento -1) = -1;
        asientos_ocupados_variable--;
        asientos_libres_variable++;
        pthread_mutex_unlock(&cerrojo);
        return id_persona_anterior;
}


int estado_asiento(int id_asiento){
        pthread_mutex_lock(&cerrojo);
        if (comprueba_sala() == -1){
                pthread_mutex_unlock(&cerrojo);
                return -1; 
        }
        if (comprueba_id_asiento(id_asiento)){
                pthread_mutex_unlock(&cerrojo);
                return -1;
        }
	if (*(asientos + id_asiento -1) != -1){
		pthread_mutex_unlock(&cerrojo);
		return *(asientos + id_asiento -1);
	}
	else{
		pthread_mutex_unlock(&cerrojo);
		return 0;
	}
}


int asientos_libres(){
        pthread_mutex_lock(&cerrojo);
        if (comprueba_sala() == -1){
                pthread_mutex_unlock(&cerrojo);
                return -1; 
        }
        pthread_mutex_unlock(&cerrojo);
	return asientos_libres_variable;
}


int asientos_ocupados(){
        pthread_mutex_lock(&cerrojo);
        if (comprueba_sala() == -1){
                pthread_mutex_unlock(&cerrojo);
                return -1; 
        }
        pthread_mutex_unlock(&cerrojo);
        return asientos_ocupados_variable;
} 


int capacidad_sala(){
        pthread_mutex_lock(&cerrojo);
        if (comprueba_sala() == -1){
                pthread_mutex_unlock(&cerrojo);
                return -1; 
        }
	pthread_mutex_unlock(&cerrojo);
	return CAPACIDAD_MAXIMA;
}


int crea_sala(int capacidad){
        pthread_mutex_lock(&cerrojo);
	CAPACIDAD_MAXIMA = capacidad;
	asientos_libres_variable = CAPACIDAD_MAXIMA;
	asientos_ocupados_variable = 0;

  if (CAPACIDAD_MAXIMA <= 0){
	  pthread_mutex_unlock(&cerrojo);
	  return -1;
	}
        
  asientos=(int*)malloc(CAPACIDAD_MAXIMA*sizeof(int));
  
	if (asientos == NULL){
	    pthread_mutex_unlock(&cerrojo);
	    return -1;
	}
	
	for (int count = 1; count <= CAPACIDAD_MAXIMA; count++){
		*(asientos + count - 1) = -1;
	}
	pthread_mutex_unlock(&cerrojo);
	return CAPACIDAD_MAXIMA;
}


int elimina_sala(){
        pthread_mutex_lock(&cerrojo);
	if (asientos == NULL || CAPACIDAD_MAXIMA <= 0){
	  pthread_mutex_unlock(&cerrojo);
	  return -1;
	}
	free(asientos);
	CAPACIDAD_MAXIMA = -1;
	asientos_libres_variable = -1;
	asientos_ocupados_variable = -1;
	pthread_mutex_unlock(&cerrojo);
	return 0;
}


int coger_tamaño_bloque(const char* ruta_fichero) {
    struct stat estado;
    if (stat(ruta_fichero, &estado) == -1) {
        fprintf(stderr, "Error al acceder al estado del archivo\n");
        exit(-1);
    }
    return estado.st_blksize;
}


int guarda_estado_sala(const char* ruta_fichero){
    pthread_mutex_lock(&cerrojo);
    ssize_t bytes_escritos;
    if (comprueba_sala() == -1){
      printf("Error: no se ha creado una sala");
      pthread_mutex_unlock(&cerrojo);
      return -1;
    }
    int fd = open(ruta_fichero, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

    if (fd == -1) {
        comprobar_error();
        pthread_mutex_unlock(&cerrojo);
        return -1;
    }
    int tam_bloque  = coger_tamaño_bloque(ruta_fichero);
    
    // Guardar en el fichero CAPACIDAD_MAXIMA
    write(fd, &CAPACIDAD_MAXIMA, sizeof(int));
    if (bytes_escritos == -1) {
        comprobar_error();
        pthread_mutex_unlock(&cerrojo);
        return -1;
    }
    
    // Guardar en el fichero asientos_ocupados_variable
    write(fd, &asientos_libres_variable,  sizeof(int));
    if (bytes_escritos == -1) {
        printf("es culpa de asientos libres");
        comprobar_error();
        pthread_mutex_unlock(&cerrojo);
        return -1;
    }    

    // Guardar en el fichero asientos_libres_variable
    write(fd, &asientos_ocupados_variable, sizeof(int));
    if (bytes_escritos == -1) {
        comprobar_error();
        pthread_mutex_unlock(&cerrojo);
        return -1;
    }

    // Guardar en el fichero el vector asientos
    for (int i = 0; i<CAPACIDAD_MAXIMA; i++) {
      bytes_escritos = write(fd, &asientos[i], tam_bloque);
      if (bytes_escritos == -1) {
          comprobar_error();
          pthread_mutex_unlock(&cerrojo);
          return -1;
      }
      if (bytes_escritos == 0)
        {
            break;
        }
    }
    close(fd);
    pthread_mutex_unlock(&cerrojo);
    return 0;
}


int recupera_estado_sala(const char* ruta_fichero){
    pthread_mutex_lock(&cerrojo);
    int nueva_CAPACIDAD_MAXIMA;
    int fd = open(ruta_fichero, O_RDONLY);
    if (fd == -1) {
        comprobar_error();
        pthread_mutex_unlock(&cerrojo);
        return -1;
    }
    int contenido;
    
    contenido = read(fd, &nueva_CAPACIDAD_MAXIMA, sizeof(int));
    if (contenido == -1) {
        comprobar_error();
        pthread_mutex_unlock(&cerrojo);
        return -1;
    }
    if (nueva_CAPACIDAD_MAXIMA != CAPACIDAD_MAXIMA){
        comprobar_error();
        pthread_mutex_unlock(&cerrojo);
        return -1;
    }
    contenido = read(fd, &asientos_libres_variable, sizeof(int));
    if (contenido == -1) {
        comprobar_error();
        pthread_mutex_unlock(&cerrojo);
        return -1;
    }
    contenido = read(fd, &asientos_ocupados_variable, sizeof(int));
    if (contenido == -1) {
        comprobar_error();
        pthread_mutex_unlock(&cerrojo);
        return -1;
    }
    int tam_bloque = coger_tamaño_bloque(ruta_fichero);
    for (int i = 0; i < CAPACIDAD_MAXIMA; i++) {
        contenido = read(fd, &asientos[i], tam_bloque);
        if (contenido == -1) {
            comprobar_error();
            pthread_mutex_unlock(&cerrojo);
            return -1;
        }
        if (contenido == 0) {
            break;
        }
    }
    
    close(fd);
    
    // Ajuste de las variables de asientos libres y ocupados
    asientos_ocupados_variable = 0;
    asientos_libres_variable = 0;
    for (int i = 1; i <= CAPACIDAD_MAXIMA; i++){
      if (*(asientos + i -1) == -1){
        asientos_libres_variable++;
      }
    }
    asientos_ocupados_variable = CAPACIDAD_MAXIMA - asientos_libres_variable;
    pthread_mutex_unlock(&cerrojo);
    return 0;
}



int guarda_estadoparcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos) {
    pthread_mutex_lock(&cerrojo);
    int fd = open(ruta_fichero, O_WRONLY | O_TRUNC, 0644);
    if (fd == -1) {
        comprobar_error();
        pthread_mutex_unlock(&cerrojo);
        return -1;
    }
    
    if (write(fd, &CAPACIDAD_MAXIMA, sizeof(int)) != sizeof(int)) {
        comprobar_error();
        pthread_mutex_unlock(&cerrojo);
        return -1;
    }

    for (size_t i = 0; i < num_asientos; i++) {
        int asiento = id_asientos[i];
        if (asiento < 1 || asiento > CAPACIDAD_MAXIMA) {
            comprobar_error();
            pthread_mutex_unlock(&cerrojo);
            return -1;
        }

        off_t offset = sizeof(int) + (asiento+1) * sizeof(int);
        if (lseek(fd, offset, SEEK_SET) == -1) {
            comprobar_error();
            pthread_mutex_unlock(&cerrojo);
            return -1;
        }

        int valor_asiento = asientos[asiento - 1];
        if (write(fd, &valor_asiento, sizeof(int)) != sizeof(int)) {
            comprobar_error();
            pthread_mutex_unlock(&cerrojo);
            return -1;
        }
    }

    if (close(fd) == -1) {
        comprobar_error();
        pthread_mutex_unlock(&cerrojo);
        return -1;
    }

    pthread_mutex_unlock(&cerrojo);
    return 0;
}

int recupera_estadoparcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos){
    pthread_mutex_lock(&cerrojo);
    int fd = open(ruta_fichero, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Error al abrir el archivo\n");
        pthread_mutex_unlock(&cerrojo);
        return -1;
    }
    int contenido;   
    int asiento_a_leer;
    off_t offset = 3 * sizeof(int);
    
    for (size_t i = 0; i < num_asientos; i++){
      int id_asiento_a_recuperar = id_asientos[i]-1;
      if (comprueba_id_asiento(id_asiento_a_recuperar) == -1){
        pthread_mutex_unlock(&cerrojo);
        return -1;
      }

      off_t nuevo_offset = offset + id_asiento_a_recuperar*sizeof(int);
      lseek(fd, nuevo_offset, SEEK_SET);
      contenido = read(fd, &asientos[id_asientos[i]-1], sizeof(int));
      if (contenido == -1){
        perror("Error al leer el estado parcial de la sala");
        pthread_mutex_unlock(&cerrojo);
        return -1;
        } 
    }
    close(fd);
    
    // Ajuste de las variables de asientos libres y ocupados
    asientos_ocupados_variable = 0;
    asientos_libres_variable = 0;
    for (int i = 1; i <= CAPACIDAD_MAXIMA; i++){
      if (*(asientos + i -1) == -1){
        asientos_libres_variable++;
      }
    }
    asientos_ocupados_variable = CAPACIDAD_MAXIMA - asientos_libres_variable;
    pthread_mutex_unlock(&cerrojo);
    return 0;
}
