#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "sala.h"
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

int *asientos;
int CAPACIDAD_MAXIMA = -1;
int asientos_libres_variable = -1;
int asientos_ocupados_variable = -1;
int num_asientos = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int comprueba_id_persona(int id_persona){
	if (id_persona <= 0){
		return -1;
	}
	return 0;
}

int comprueba_sala(){
	if (asientos_libres_variable < 0){
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

int comprobar_capacidad_salafichero_misma_salaactual(const char* ruta_fichero){
    int nueva_CAPACIDAD_MAXIMA;
    int fd = open(ruta_fichero, O_RDONLY);
    if (fd == -1) {
        comprobar_error();
        return -1;
    }
    
    int contenido;
    contenido = read(fd, &nueva_CAPACIDAD_MAXIMA, sizeof(int));
    if (contenido == -1) {
        comprobar_error();
        return -1;
    }
    if (nueva_CAPACIDAD_MAXIMA != CAPACIDAD_MAXIMA){
        comprobar_error();
        return -1;
    }
    return 0;
}

int reserva_asiento(int id_persona){
    pthread_mutex_lock(&lock);
    if (comprueba_sala() == -1){
        pthread_mutex_unlock(&lock);
        return -1; 
    }
    
    if (comprueba_id_persona(id_persona) == -1){
        pthread_mutex_unlock(&lock);
        return -1;
    }
    
    if (asientos_libres_variable == 0){
        pthread_mutex_unlock(&lock);
        return -1;
    }
    
    for (int count = 1; count <= CAPACIDAD_MAXIMA; count++){
        if (*(asientos + count - 1) == -1){
            asientos_ocupados_variable++;
            asientos_libres_variable--;
            *(asientos + count - 1) = id_persona;
            pthread_mutex_unlock(&lock);
            return count;
        }
    }
    pthread_mutex_unlock(&lock);
    return -1;
}

int libera_asiento(int id_asiento){
    pthread_mutex_lock(&lock);
    if (comprueba_sala() == -1 || comprueba_id_asiento(id_asiento) == -1){
        pthread_mutex_unlock(&lock);
        return -1; 
    }

	int id_persona_anterior = *(asientos + id_asiento - 1);
	*(asientos + id_asiento - 1) = -1;
    asientos_ocupados_variable--;
    asientos_libres_variable++;
    pthread_mutex_unlock(&lock);
    return id_persona_anterior;
}

int estado_asiento(int id_asiento){
    pthread_mutex_lock(&lock);
    if (comprueba_sala() == -1 || comprueba_id_asiento(id_asiento) == -1){
        pthread_mutex_unlock(&lock);
        return -1; 
    }

    int estado = *(asientos + id_asiento - 1);
    pthread_mutex_unlock(&lock);
    return estado == -1 ? 0 : estado;
}

int asientos_libres(){
    pthread_mutex_lock(&lock);
    if (comprueba_sala() == -1){
        pthread_mutex_unlock(&lock);
        return -1; 
    }
    int libres = asientos_libres_variable;
    pthread_mutex_unlock(&lock);
    return libres;
}

int asientos_ocupados(){
    pthread_mutex_lock(&lock);
    if (comprueba_sala() == -1){
        pthread_mutex_unlock(&lock);
        return -1; 
    }
    int ocupados = asientos_ocupados_variable;
    pthread_mutex_unlock(&lock);
    return ocupados;
}

int capacidad_sala(){
    pthread_mutex_lock(&lock);
    if (comprueba_sala() == -1){
        pthread_mutex_unlock(&lock);
        return -1; 
    }
    int capacidad = CAPACIDAD_MAXIMA;
    pthread_mutex_unlock(&lock);
    return capacidad;
}

int crea_sala(int capacidad){
    pthread_mutex_lock(&lock);
	CAPACIDAD_MAXIMA = capacidad;
	asientos_libres_variable = CAPACIDAD_MAXIMA;
	asientos_ocupados_variable = 0;

    if (CAPACIDAD_MAXIMA <= 0){
        pthread_mutex_unlock(&lock);
	    return -1;
	}
        
    asientos = (int*)malloc(CAPACIDAD_MAXIMA * sizeof(int));
    if (asientos == NULL){
        pthread_mutex_unlock(&lock);
        return -1;
    }

    for (int count = 1; count <= CAPACIDAD_MAXIMA; count++){
        *(asientos + count - 1) = -1;
    }
    pthread_mutex_unlock(&lock);
    return CAPACIDAD_MAXIMA;
}

int elimina_sala(){
    pthread_mutex_lock(&lock);
    if (asientos == NULL || CAPACIDAD_MAXIMA <= 0){
        pthread_mutex_unlock(&lock);
        return -1;
    }
    free(asientos);
    CAPACIDAD_MAXIMA = -1;
    asientos_libres_variable = -1;
    asientos_ocupados_variable = -1;
    pthread_mutex_unlock(&lock);
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
    ssize_t bytes_escritos;
    pthread_mutex_lock(&lock);
    if (comprueba_sala() == -1){
        printf("Error: no se ha creado una sala");
        pthread_mutex_unlock(&lock);
        return -1;
    }
    
    int fd = open(ruta_fichero, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        comprobar_error();
        pthread_mutex_unlock(&lock);
        return -1;
    }
    int tam_bloque  = coger_tamaño_bloque(ruta_fichero);
    
    bytes_escritos = write(fd, &CAPACIDAD_MAXIMA, sizeof(int));
    if (bytes_escritos == -1) {
        comprobar_error();
        pthread_mutex_unlock(&lock);
        return -1;
    }
    
    bytes_escritos = write(fd, &asientos_libres_variable,  sizeof(int));
    if (bytes_escritos == -1) {
        comprobar_error();
        pthread_mutex_unlock(&lock);
        return -1;
    }    

    bytes_escritos = write(fd, &asientos_ocupados_variable, sizeof(int));
    if (bytes_escritos == -1) {
        comprobar_error();
        pthread_mutex_unlock(&lock);
        return -1;
    }

    int num_bloques = CAPACIDAD_MAXIMA / tam_bloque;
    int resto = CAPACIDAD_MAXIMA % tam_bloque;
    for (int i = 0; i < num_bloques; i++) {
        bytes_escritos = write(fd, &asientos[i * tam_bloque], tam_bloque * sizeof(int));
        if (bytes_escritos == -1) {
            comprobar_error();
            pthread_mutex_unlock(&lock);
            return -1;
        }
    }
    if (resto > 0){
        bytes_escritos = write(fd, &asientos[num_bloques * tam_bloque], resto * sizeof(int));
        if (bytes_escritos == -1) {
            comprobar_error();
            pthread_mutex_unlock(&lock);
            return -1;
        }
    }
    close(fd);
    pthread_mutex_unlock(&lock);
    return 0;
}

int recupera_estado_sala(const char* ruta_fichero){
    pthread_mutex_lock(&lock);
    if (comprobar_capacidad_salafichero_misma_salaactual(ruta_fichero) == -1){
        pthread_mutex_unlock(&lock);
        return -1;
    }

    ssize_t bytes_leidos;
    int fd = open(ruta_fichero, O_RDONLY);
    if (fd == -1) {
        comprobar_error();
        pthread_mutex_unlock(&lock);
        return -1;
    }

    bytes_leidos = read(fd, &CAPACIDAD_MAXIMA, sizeof(int));
    if (bytes_leidos == -1) {
        comprobar_error();
        pthread_mutex_unlock(&lock);
        return -1;
    }
    bytes_leidos = read(fd, &asientos_libres_variable,  sizeof(int));
    if (bytes_leidos == -1) {
        comprobar_error();
        pthread_mutex_unlock(&lock);
        return -1;
    }
    bytes_leidos = read(fd, &asientos_ocupados_variable, sizeof(int));
    if (bytes_leidos == -1) {
        comprobar_error();
        pthread_mutex_unlock(&lock);
        return -1;
    }

    asientos = (int*)malloc(CAPACIDAD_MAXIMA * sizeof(int));
    if (asientos == NULL){
        comprobar_error();
        pthread_mutex_unlock(&lock);
        return -1;
    }
    bytes_leidos = read(fd, asientos, CAPACIDAD_MAXIMA * sizeof(int));
    if (bytes_leidos == -1) {
        comprobar_error();
        pthread_mutex_unlock(&lock);
        return -1;
    }
    close(fd);
    pthread_mutex_unlock(&lock);
    return 0;
}
