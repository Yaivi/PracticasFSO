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


void test_CapacidadMaxSobrepasada(){
	#define CAPACIDAD_MAX 1000
	#define id_persona_de_mas 7
	INICIO_TEST("Capacidad máxima sobrepasada");
	for (int i = 1; i<=CAPACIDAD_MAX; i++){
	reserva_asiento(i+10);
	}
	DebeSerCierto(reserva_asiento(id_persona_de_mas)==-1);
	FIN_TEST("Capacidad máxima sobrepasada");
}


void test_RerservaSinSala() {
	int mi_asiento;
	#define ID_2 100
	INICIO_TEST("Reserva Sin Sala");
	DebeSerCierto((mi_asiento=reserva_asiento(ID_2))<=-1);
	FIN_TEST("Reserva Sin Sala");
}


void test_reservas_multiples() {
	int mi_asiento;
	#define CAPACIDAD_TEATRO 10
	#define ID_3 777
	int num_asientos = 7;
	INICIO_TEST("Reservas multiples");
	crea_sala(CAPACIDAD_TEATRO);
	DebeSerCierto(capacidad_sala()==CAPACIDAD_TEATRO);
	for (int i=0;i<num_asientos; i++) {
		DebeSerCierto((mi_asiento=reserva_asiento(ID_3))>=0);
	}
	DebeSerCierto(asientos_libres() == 3);
	FIN_TEST("Reservas multiples");    
}


void test_ReservaPrimerAsiento(){
	#define capacidad_test_reservaprimer_asiento 10
	int mi_primer_asiento;
	INICIO_TEST("Reserva primer asiento");
	crea_sala(capacidad_test_reservaprimer_asiento);
	int id_persona_primera = 765432;
	DebeSerCierto(mi_primer_asiento=reserva_asiento(ID_1)==1);

	FIN_TEST("Reserva primer asiento");
}


void test_ReservarAsientoAntesOcupado(){ 
	#define CAPACIDAD_MAX_ASIENTOS_OCUPADOS 100
	int id_asiento_nuevo = 77;

	INICIO_TEST("Reservar asiento anteriormente ocupado");
	crea_sala(CAPACIDAD_MAX_ASIENTOS_OCUPADOS);
	for (int i = 1; i<=CAPACIDAD_MAX; i++){
	reserva_asiento(i*10);
	}
	libera_asiento(2);
	DebeSerCierto(reserva_asiento(id_asiento_nuevo)==2);
	FIN_TEST("Capacidad asiento anteriormente ocupado");
}


void ejecuta_tests ()
{
	test_ReservaBasica();
	// Añadir nuevos tests
	test_CapacidadNegativa();
	test_CapacidadCero();
	test_CapacidadMaxSobrepasada();
	test_ReservaSinSala();
	test_reservas_multiples();
	test_ReservaPrimerAsiento();
	test_ReservarAsientoAntesOcupado();
}

int main()
{
	puts("Iniciando tests...");
	
	ejecuta_tests();
	
	puts("Batería de test completa.");
}
