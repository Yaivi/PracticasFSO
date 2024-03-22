#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "sala.h"

void crea_sucursal (const char* ciudad, int capacidad);

void crea_sucursal (const char* ciudad, int capacidad) {
  // Crear un proceso hijo que lance una terminal donde se
  // ejecute el mini-shell añadido a la práctica 1
  // Deberás emplear las llamadas al sistema fork() y exec()
  pid_t pid = fork();
  
  if (pid == -1){
    exit(-1);
  }
  if (pid == 0){
    // Proceso Hijo
    //execl();
    
    //Si falla, hará exit(-1), este código no debería
    //ejecutarse si todo va bien.
    exit(-1);
  }
  
  

}


int main() {
    char nombresala[100];
    int capacidad;
    while (1) {
        fgets(nombresala, 100, stdin);
        nombresala[strcspn(nombresala, "\n")] = '\0';
        
        // Pedir nombre de nueva sala y capacidad
        if (!strcmp("salir",nombresala)){
            break;
        }
        crea_sucursal (nombresala, capacidad);
        } // termina el bucle principal
}
