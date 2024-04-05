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
   int pid = fork();
  
  if (pid == -1){
    exit(-1);
  }
  if (pid == 0){
    // Proceso Hijo
    char capacidadChar[20];
    snprintf(capacidadChar, sizeof(capacidadChar), "%d", capacidad);
    execlp("xterm", "xterm", "-e", "c/Practica02/sala", ciudad, capacidadChar, NULL);
    
    //Si falla, hará exit(-1), este código no debería ejecutarse si todo va bien.
    printf("Algo ha fallado.\n");
    exit(-1);
  }
}  



int main() {
    #define MAX_LENGTH_NAME 100
    #define MAX_LENGTH_CAPACITY 100
    char nombresala[MAX_LENGTH_NAME];
    char capacidadString[MAX_LENGTH_CAPACITY];
    int capacidad;
    while (1) {
        printf("Introduzca el nombre de la sucursal:\n");
        fgets(nombresala, MAX_LENGTH_NAME, stdin);
        nombresala[strcspn(nombresala, "\n")] = '\0';
                
        // Pedir nombre de nueva sala y capacidad
        if (!strcmp("salir",nombresala)){
            break;
        }
        
        printf("Introduzca la capacidad de la sucursal:\n");
        fgets(capacidadString, MAX_LENGTH_CAPACITY, stdin);
        capacidad = atoi(capacidadString);

        if (capacidad == 0){
          printf("Capacidad introducida no válida");
        }
        
        crea_sucursal (nombresala, capacidad);
        } // termina el bucle principal
}
