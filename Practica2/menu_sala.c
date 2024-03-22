#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "sala.h"

int main(){
  char input_usuario[100];
  char instrucciones[4][100] = {"reserva", "libera", "estado_asiento", "cerrar_sala"};
  
  printf("\nEscriba la opción que desea entre paréntesis:\n");
  printf("Reserva asiento (reserva <id-persona>).\n");
  printf("Liberar asiento (libera <id-persona>).\n");
  printf("Estado del asiento (estado_asiento <id-asiento>).\n");
  printf("Cerrar y eliminar sala (cerrar_sala).\n");
  
  fgets(input_usuario, 100, stdin); // Pide al usuario por pantalla que escriba la orden deseada
  input_usuario[strcspn(input_usuario, "\n")] = '\0';
  char *espacio_char = strchr(input_usuario, ' ');
  
  if (espacio_char != NULL){
    *espacio_char = '\0';
  }
  
  printf("Orden: %s\n", input_usuario);
  printf("Primera Parte de la orden: %s\n", espacio_char);
  
  return 0;
}
