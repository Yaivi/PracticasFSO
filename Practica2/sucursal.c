#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "sala.h"

void crea_sucursal (const char* ciudad, int capacidad);
int lista_pid[100]; //Para ir guardando los pid en esta lista, y borrarlos luego al ir cerrándolos, 
                    //principal uso, para copiar el pid si más de un proceso se cierra al mismo tiempo
                    //y poder imprimirlo en por pantalla.
char nombres_salas[100][100]; // Lista para almacenar nombres de salas

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
    execlp("xterm", "xterm", "-e", "./menu_sala", ciudad, capacidadChar, NULL);
    
    //Si falla, hará exit(-1), este código no debería ejecutarse si todo va bien.
    printf("Algo ha fallado.\n");
    exit(-1);
  } else {
      for (int i = 0; i<100;i++) {
        if (lista_pid[i] == 0) {
            lista_pid[i] = pid;
            strncpy(nombres_salas[i], ciudad, 100);
            break;
        }
      }//añadimos el pid del hijo y el nombre en la misma pos de sus respectivas listas
  }
}  

int ComprobarSala(int capacidad){
  if (capacidad <= 0){
    return -1;
  } else{
    return 0;
  }
}


int main() {
    #define MAX_LENGTH_NAME 100
    #define MAX_LENGTH_CAPACITY 100
    char nombresala[MAX_LENGTH_NAME];
    char capacidadString[MAX_LENGTH_CAPACITY];
    int capacidad;
    char sala_acabada[MAX_LENGTH_NAME];
    pid_t pid_terminado;
    while (1) {
        pid_terminado = waitpid(-1, NULL, WNOHANG);
        
        if (pid_terminado > 0) {
          for (int i = 0; i < 100; i++) {
                  if (lista_pid[i] == pid_terminado) {
                      strcpy(sala_acabada, nombres_salas[i]);
                      nombres_salas[i][0] = '\0'; 
                      lista_pid[i] = 0;
                      break;
                  }
              }
          printf("Proceso hijo con PID-%d, y nombre de sala: %s. Ha terminado.\n", pid_terminado, sala_acabada);
        } 
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

        if (ComprobarSala(capacidad) == -1){
          printf("Capacidad introducida no válida\n");
          continue;
        }
        
        crea_sucursal (nombresala, capacidad);
        } // termina el bucle principal
        
}
