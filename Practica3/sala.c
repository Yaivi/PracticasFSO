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

int AbrirArchivo(char *nombre) {
    int archivo_abierto;

    archivo_abierto = open(nombre, O_RDONLY);
    if (archivo_abierto == -1) {
        fprintf(stderr, "Error %d al abrir el archivo: \n", errno);
        exit(-1);
    }
    
    return archivo_abierto;
}

void CerrarArchivo(int fd) {
    if (fd == -1) {
        fprintf(stderr, "El archivo no se encuentra abierto.\n");
    } else {
        close(fd);
    }

}

int CrearArchivo(char *nombre) {
    int fd;

    fd = open(nombre, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        fprintf(stderr, "Error al crear el archivo\n");
        exit(EXIT_FAILURE);
    }
        
    return fd;    
}


void LeerEstado(int fd) {
    int asientos[MAX];
    int contenido;
    contenido = read(fd, asientos, sizeof(int)*MAX);
    if (contenido == -1) {
        fprintf(stderr, "Error %d al leer el archivo: \n", errno);
        exit(-1);
    }
    int num_asientos_leidos = 1 / sizeof(int); //bytes_leidos / sizeof(int);
    
    printf("Asientos leídos desde el archivo:\n");
    for (int i = 0; i < num_asientos_leidos; i++) {
        printf("%d ", asientos[i]);
    }
    printf("\n");
    
}

void GuardarEstado(int fd) {
    ssize_t bytes_escritos;

    bytes_escritos = write(fd, asientos, sizeof(int) * num_asientos);
    if (bytes_escritos == -1) {
        fprintf(stderr, "Error %d al escribir en el archivo\n", errno);
        exit(EXIT_FAILURE);
    }

    printf("Estado guardado en el archivo correctamente.\n");      
}

void MostrarAtributos(int fd) {
    struct stat estado;
	
    if (fstat(  fd, &estado) == -1) {
        fprintf(stderr, "Error al acceder al estado del archivo\n");
        exit(-1);
    }
    if (S_ISREG(estado.st_mode)) {
        printf("Fichero regular\n");
    }
    printf("El identificador del propietario %d \n", estado.st_uid);
    printf("El identificador del grupo %d \n", estado.st_gid);
    printf("El archivo tiene un tamaño de %ld bytes \n", estado.st_size);
    printf("El archivo tiene un tamaño de bloque de %ld \n",estado.st_blocks);
    CerrarArchivo(fd);
}

int guarda_estado_sala(const char* ruta_fichero){
    int fd;
    ssize_t bytes_escritos;
    off_t offset = 0;
    fd = open(ruta_fichero, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

    // Guardar en el fichero CAPACIDAD_MAXIMA
    offset = sizeof(int)*CAPACIDAD_MAXIMA;
    off_t nuevo_offset = lseek(fd, offset, SEEK_SET);
    if (nuevo_offset == -1){
      perror("Ha sucedido un error al guardar el estado de la sala");
      exit(-1);
    }
    bytes_escritos = write(fd, &CAPACIDAD_MAXIMA, sizeof(int));
    if (bytes_escritos == -1) {
        fprintf(stderr, "Error %d al escribir en el archivo\n", errno);
        exit(-1);
    }
    
    // Guardar en el fichero asientos_libres_variable
    offset += sizeof(int);
    nuevo_offset = lseek(fd, offset, SEEK_SET);
    if (nuevo_offset == -1){
      perror("Ha sucedido un error al guardar el estado de la sala");
      exit(-1);
    }
    bytes_escritos = write(fd, &asientos_libres_variable, sizeof(int));
    if (bytes_escritos == -1) {
        fprintf(stderr, "Error %d al escribir en el archivo\n", errno);
        exit(-1);
    }    
    
    return 0;
}


int recupera_estado_sala(const char* ruta_fichero){
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
