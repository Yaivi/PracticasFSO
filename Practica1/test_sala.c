// test_sala.c
// ===============
// Batería de pruebas de la biblioteca "sala.h/sala.c"
//

#include "sala.h"
#include <assert.h>
#include <stdio.h>

#define DebeSerCierto(x)	assert(x)
#define DebeSerFalso(x)		assert(!(x))

void INICIO_TEST (const char* titulo_test)
{
  printf("********** batería de pruebas para %s: ", titulo_test); 
 	// fflush fuerza que se imprima el mensaje anterior
	// sin necesidad de utilizar un salto de línea
	fflush(stdout);
}

void FIN_TEST (const char* titulo_test)
{
  printf ("********** hecho\n");
}

	
int reserva_multiple(int npersonas, int* lista_id){
    if (asientos_libres()<npersonas) {
	    return -1;
    }
    else {
	for (int i = 0; i<npersonas; i++) {
	    int id_actual =*(lista_id+i);
	    reserva_asiento(id_actual);
	}
	return 0;
    }
}
void test_ReservaBasica()
{
	int mi_asiento;
	#define CAPACIDAD_CUYAS 500
	#define ID_1 1500

	INICIO_TEST("Reserva básica");
	crea_sala(CAPACIDAD_CUYAS);
	DebeSerCierto(capacidad_sala()==CAPACIDAD_CUYAS);
	DebeSerCierto((mi_asiento=reserva_asiento(ID_1))>=0);
	DebeSerCierto((asientos_libres()+asientos_ocupados())==CAPACIDAD_CUYAS);
	DebeSerCierto(estado_asiento(mi_asiento)>0);
	DebeSerCierto(libera_asiento(mi_asiento)==ID_1);
	DebeSerCierto((asientos_libres()+asientos_ocupados())==CAPACIDAD_CUYAS);
	elimina_sala();
	FIN_TEST("Reserva básica");
}



void test_CapacidadNegativa(){
	#define CAPACIDAD_MENOR_CERO -1000

	INICIO_TEST("Capacidad Negativa");
	DebeSerCierto(crea_sala(CAPACIDAD_MENOR_CERO)==-1);
	DebeSerCierto(elimina_sala()==-1);
	FIN_TEST("Capacidad Negativa");
}


void test_CapacidadCero(){
	#define CAPACIDAD_CERO 0

	INICIO_TEST("Capacidad Cero");
	DebeSerCierto(crea_sala(CAPACIDAD_CERO)==-1);
	DebeSerCierto(elimina_sala()==0);
	FIN_TEST("Capacidad Cero");
}

void test_IdAsientoMayorQueCapacidad(){
	#define CAPACIDAD_ID_ASIENTO_MAYOR_QUE_CAPACIDAD 1000
	#define id_asiento_de_más 1001

	INICIO_TEST("Asiento mayor que Capacidad");
	crea_sala(CAPACIDAD_ID_ASIENTO_MAYOR_QUE_CAPACIDAD);
	DebeSerCierto(libera_asiento(id_asiento_de_más)==-1);
	DebeSerCierto(estado_asiento(id_asiento_de_más)==-1);
	elimina_sala();
	FIN_TEST("Asiento mayor que Capacidad");
}

void test_CapacidadMaxSobrepasada(){
	#define CAPACIDAD_MAX 1000
	#define id_persona_de_mas 7
	INICIO_TEST("Capacidad máxima sobrepasada");
	crea_sala(CAPACIDAD_MAX);
	for (int i = 1; i<=CAPACIDAD_MAX; i++){
	reserva_asiento(i+10);
	}
	DebeSerCierto(reserva_asiento(id_persona_de_mas)==-1);
	elimina_sala();
	FIN_TEST("Capacidad máxima sobrepasada");
}


void test_ReservaSinSala() {
	int mi_asiento;
	#define ID_2 100
	INICIO_TEST("Reserva Sin Sala");
	DebeSerCierto((mi_asiento=reserva_asiento(ID_2))<=-1);
	FIN_TEST("Reserva Sin Sala");
}


void test_ReservaVariosAsientos() {
	int mi_asiento;
	#define CAPACIDAD_TEATRO 10
	#define ID_3 777
	int num_asientos = 7;
	INICIO_TEST("Reservas varios asientos");
	crea_sala(CAPACIDAD_TEATRO);
	DebeSerCierto(capacidad_sala()==CAPACIDAD_TEATRO);
	for (int i=0;i<num_asientos; i++) {
		DebeSerCierto((mi_asiento=reserva_asiento(ID_3))>=0);
	}
	DebeSerCierto(asientos_libres() == 3);
	elimina_sala();
	FIN_TEST("Reservas varios asientos");
}


void test_ReservaPrimerAsiento(){
	#define capacidad_ReservaPrimerAsiento 10
	int mi_primer_asiento;
	int id_persona_primera = 765432;
	
	INICIO_TEST("Reserva primer asiento");
	crea_sala(capacidad_ReservaPrimerAsiento);
	DebeSerCierto((mi_primer_asiento=reserva_asiento(id_persona_primera))>=0);
	DebeSerCierto(elimina_sala()==0);
	FIN_TEST("Reserva primer asiento");
}

void test_ReservaAsientoLiberado(){
	#define capacidad_ReservaAsientoLiberado 10
	int id_persona_en_asiento_liberado = 98765432;
	int mi_asiento_liberado;
	
	INICIO_TEST("Reserva asiento liberado");
	crea_sala(capacidad_ReservaAsientoLiberado);
	for (int i = 1; i<=capacidad_ReservaAsientoLiberado; i++){
		reserva_asiento(i*100);
	}
	libera_asiento(2);
	DebeSerCierto(mi_asiento_liberado=reserva_asiento(id_persona_en_asiento_liberado)>=2);
	DebeSerCierto(elimina_sala()==0);
	FIN_TEST("Reserva asiento liberado");
}

void test_ReservasMultiples() {
	#define capacidad_Sala_Multiple 5
	int* listaIdGrupo1;
	int ids1[6] = {22, 43, 67, 49, 62, 89};
	int tamañoGrupo1 = sizeof(ids1)/sizeof(ids1[0]);
	listaIdGrupo1 = ids1;
	
	int* listaIdGrupo2;
	int ids2[3] = {22, 43, 67};
	int tamañoGrupo2 = sizeof(ids2)/sizeof(ids2[0]);
	listaIdGrupo2 = ids2;


	INICIO_TEST("Reservas Múltiples");
	crea_sala(capacidad_Sala_Multiple);
	DebeSerCierto(reserva_multiple(tamañoGrupo1,listaIdGrupo1)==-1);
	DebeSerCierto(reserva_multiple(tamañoGrupo2,listaIdGrupo2)==0);
	DebeSerCierto(estado_asiento(1) == 22);
	DebeSerCierto(estado_asiento(2) == 43);
  	DebeSerCierto(estado_asiento(3) == 67);
  	DebeSerCierto(estado_asiento(4) == 0);
  	FIN_TEST("Reservas Múltiples");
}

void test_Id_Negativa() {
    #define Capacidad_Salon 10
    #define ID_Negativo -100
    INICIO_TEST("ID Negativa");
    crear_sala(Capacidad_Salon);
    DebeSerCierto(reserva_asiento(ID_Negativo)==-1);
    FIN_TEST("ID Negativa");
    
}

void ejecuta_tests ()
{
	test_ReservaBasica();
	// Añadir nuevos tests
	test_CapacidadNegativa();
	test_CapacidadCero();
	test_IdAsientoMayorQueCapacidad();
	test_CapacidadMaxSobrepasada();
	test_ReservaSinSala();
	test_ReservaVariosAsientos();
	test_ReservaPrimerAsiento();
	test_ReservaAsientoLiberado();
	test_ReservasMultiples();
	test_Id_Negativa();
}

int main()
{
	puts("Iniciando tests...");
	
	ejecuta_tests();
	
	puts("Batería de test completa.");
}
