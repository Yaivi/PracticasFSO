// Método para reservar un asiento de la sala
// Devuelve:
//   El número del asiento
//   -1 si no hay asientos libres
int reserva_asiento (int id_persona);


// Método para liberar un asiento reservado.
// Devuelve:
//   El identificador de la persona que ocupada el asiento
//   0 si el asiento está libre
//   -1 si ha habido algún error
int libera_asiento (int id_asiento); 


// Método que devuelve:
//   el identificador de la persona que lo ocupa
//   0 si no está ocupado
//   -1 si ha habido un error a la hora de ejecutar el método
int estado_asiento(int id_asiento);


// Método que devuelve el número de asientos libres.
int asientos_libres ();


// Método que devuelve el número de asientos ocupados.
int asientos_ocupados (); 


// Método que devuelve la capacidad de la sala.
int capacidad_sala ();


// Método para crear y reservar espacio para una sala.
// Se le pasa como argumento el número de asientos máximos.
//Devuelve:
//   La capacidad de la sala
//   -1 si no se ha podido crear la sala.
int crea_sala (int capacidad);


// Método para eliminar una sala y liberar la memoria.
// Devuelve:
//   0 si ha funcionado correctamente
//   -1 si ha habido algún error
int elimina_sala();


//
int guarda_estado_sala(const	char*	ruta_fichero);


//
int recupera_estado_sala(const	char*	ruta_fichero);


//
int	guarda_estadoparcial_sala(const char*	ruta_fichero,	size_t num_asientos,	int*	id_asientos);


//
int	recupera_estadoparcial_sala(const	char*	ruta_fichero, size_t num_asientos, int*	id_asientos);
