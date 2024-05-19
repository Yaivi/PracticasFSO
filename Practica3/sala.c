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
    if (stat(ruta_fichero, &estado) == -1) {
        fprintf(stderr, "Error al acceder al estado del archivo\n");
        exit(-1);
    }
    return estado.st_blksize;
}


int guarda_estado_sala(const char* ruta_fichero){
    ssize_t bytes_escritos;
    if (comprueba_sala() == -1){
      printf("Error: no se ha creado una sala");
      return -1;
    }
    
    int fd = open(ruta_fichero, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

    if (fd == -1) {
        comprobar_error();
        return -1;
    }
    int tam_bloque  = coger_tamaño_bloque(ruta_fichero);
    int fd = open(ruta_fichero, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

    if (fd == -1) {
        comprobar_error();
        return -1;
    }
    int tam_bloque  = coger_tamaño_bloque(ruta_fichero);
    
    // Guardar en el fichero CAPACIDAD_MAXIMA
    write(fd, &CAPACIDAD_MAXIMA, sizeof(int));
    if (bytes_escritos == -1) {
        comprobar_error();
        return -1;
    }
    
    // Guardar en el fichero asientos_ocupados_variable
    write(fd, &asientos_libres_variable,  sizeof(int));
    if (bytes_escritos == -1) {
        printf("es culpa de asientos libres");
        comprobar_error();
        return -1;
    }    

    // Guardar en el fichero asientos_libres_variable
    write(fd, &asientos_ocupados_variable, sizeof(int));
    if (bytes_escritos == -1) {
        comprobar_error();
        return -1;
    }

    // Guardar en el fichero el vector asientos
    int num_bloques = CAPACIDAD_MAXIMA / tam_bloque;
    int resto = CAPACIDAD_MAXIMA % tam_bloque;
    for (int i = 0; i < num_bloques; i++) {
      bytes_escritos = write(fd, &asientos[i*tam_bloque], tam_bloque*sizeof(int));
      if (bytes_escritos == -1) {
          comprobar_error();
          return -1;
      }
    }
    // Si hay bloques que no estén llenos
    if (resto > 0){
      bytes_escritos = write(fd, &asientos[num_bloques * tam_bloque], resto*sizeof(int));
      if (bytes_escritos == -1) {
      comprobar_error();
      return -1;
      }
    }
      
    close(fd);
    return 0;
}


int recupera_estado_sala(const char* ruta_fichero){
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
    contenido = read(fd, &asientos_libres_variable, sizeof(int));
    if (contenido == -1) {
        comprobar_error();
        return -1;
    }
    contenido = read(fd, &asientos_ocupados_variable, sizeof(int));
    if (contenido == -1) {
        comprobar_error();
        return -1;
    }

    int tam_bloque  = coger_tamaño_bloque(ruta_fichero);
    int num_bloques = CAPACIDAD_MAXIMA / tam_bloque;
    int resto = CAPACIDAD_MAXIMA % tam_bloque;
    for (int i = 0; i < num_bloques; i++) {
        contenido = read(fd, &asientos[i*tam_bloque], tam_bloque*sizeof(int));
        if (contenido == -1) {
            comprobar_error();
            return -1;
        }
    }
    if (resto > 0){
        contenido = read(fd, &asientos[num_bloques * tam_bloque], resto*sizeof(int));      
        if (contenido == -1) {
          comprobar_error();
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
    
    return 0;
}

int guarda_estadoparcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos){
    if (comprobar_capacidad_salafichero_misma_salaactual(ruta_fichero) == -1){
      return -1;
    }
    
    for (size_t i = 0; i < num_asientos/sizeof(int); i++) {
        int asiento = id_asientos[i];
        printf("Asiento %d\n", asiento);
        if (asiento < 1 || asiento > CAPACIDAD_MAXIMA) {
            fprintf(stderr, "Hay un asiento o más que no se encuentran en el rango de la sala.\n");
            comprobar_error();
            return -1;
        }
    }
    
    int fd = open(ruta_fichero, O_WRONLY);
    if (fd == -1) {
        comprobar_error();
        return -1;
    }
    
    off_t offset = 3 * sizeof(int);
    for (size_t i = 0; i < num_asientos/sizeof(int); i++){
        int id_asiento_a_guardar = id_asientos[i]-1;
        if (comprueba_id_asiento(id_asiento_a_guardar) == -1){
            return -1;
        }

        off_t nuevo_offset = offset + id_asiento_a_guardar*sizeof(int);
        lseek(fd, nuevo_offset, SEEK_SET);
        int id_persona_a_guardar = asientos[id_asiento_a_guardar];
        if (write(fd, &id_persona_a_guardar, sizeof(int)) == -1){
            comprobar_error();
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
    
    return 0;
}


int recupera_estadoparcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos){
    if (comprobar_capacidad_salafichero_misma_salaactual(ruta_fichero) == -1){
      return -1;
    }

    for (size_t i = 0; i < num_asientos/sizeof(int); i++) {
      int asiento = id_asientos[i];
      printf("Asiento %d\n", asiento);
      if (asiento < 1 || asiento > CAPACIDAD_MAXIMA) {
        fprintf(stderr, "Hay un asiento o más que no se encuentran en el rango de la sala.\n");
        comprobar_error();
        return -1;
      }
  }
    
    int fd = open(ruta_fichero, O_RDONLY);
    if (fd == -1) {
        comprobar_error();
        return -1;
    }
    int contenido;   
    int asiento_a_leer;
    off_t offset = 3 * sizeof(int);
    
    for (size_t i = 0; i < num_asientos; i++){
      int id_asiento_a_recuperar = id_asientos[i]-1;
      if (comprueba_id_asiento(id_asiento_a_recuperar) == -1){
        return -1;
      }

      off_t nuevo_offset = offset + id_asiento_a_recuperar*sizeof(int);
      lseek(fd, nuevo_offset, SEEK_SET);
      contenido = read(fd, &asientos[id_asientos[i]-1], sizeof(int));
      if (contenido == -1){
        comprobar_error();
        return -1;
        } 
    }
    close(fd);
    
    
    int asientos_libres_fichero = 0;
    int asientos_ocupados_fichero = 0;
    fd = open(ruta_fichero, O_RDONLY);
    if (fd == -1) {
        comprobar_error();
        return -1;
    }

    lseek(fd, sizeof(int), SEEK_SET);
    contenido = read(fd, &asientos_libres_fichero, sizeof(int));
    if (contenido == -1) {
        comprobar_error();
        return -1;
    }
    contenido = read(fd, &asientos_ocupados_fichero, sizeof(int));
    if (contenido == -1) {
        comprobar_error();
        return -1;
    }
    close(fd);
  
    if (asientos_libres() != asientos_libres_fichero || asientos_ocupados() != asientos_ocupados_fichero){
      // Ajuste de las variables de asientos libres y ocupados
      asientos_ocupados_variable = 0;
      asientos_libres_variable = 0;
      for (int i = 1; i <= CAPACIDAD_MAXIMA; i++){
        if (*(asientos + i -1) == -1){
          asientos_libres_variable++;
        }
      }
      asientos_ocupados_variable = CAPACIDAD_MAXIMA - asientos_libres_variable;
    }
    return 0;
}

/**
int main() {
  printf("---------------------------------\n");
  crea_sala(10);
  guarda_estado_sala("Documentos/Practica3/pruebaGP.txt");
  for (int i = 1; i < 9; i++){
     reserva_asiento(i*101);
  }  
  for (int i = 1; i < CAPACIDAD_MAXIMA+1; i++){
    printf("ASIENTO : %d estado: %d\n", i, estado_asiento(i));
  }    
  int ids_asientos[4] = {1, 4, 6, 5}; // Ejemplo de IDs de asientos
  guarda_estadoparcial_sala("Documentos/Practica3/pruebaGP.txt", sizeof(ids_asientos), ids_asientos);
  elimina_sala();
  printf("---------------------------------\n");
  crea_sala(10);
  //recupera_estadoparcial_sala("Documentos/Practica3/pruebaGP.txt", sizeof(ids_asientos), ids_asientos);
  recupera_estado_sala("Documentos/Practica3/pruebaGP.txt");
  reserva_asiento(222);
  reserva_asiento(333);
  reserva_asiento(444);
  reserva_asiento(555);
  reserva_asiento(666);
  printf("ASIENTOS LIBRES %d\n", asientos_libres());
  printf("ASIENTOS OCUPADOS %d\n", asientos_ocupados());
  for (int i = 1; i < CAPACIDAD_MAXIMA+1; i++){
    printf("ASIENTO : %d estado: %d\n", i, estado_asiento(i));
  }    
  elimina_sala();
  
  printf("TERMINADO\n");
  return 0;
}

**/ 
