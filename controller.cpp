/*
 * Implementaci�n funciones controller
 *
 * Universidade Da Coru�a. 2015
 *
 * Marcos Horro
 * Christian Ponte
 */

/*
 * NOTA: se han a�adido peri�dicamente, en todos los bucles,
 *       la funci�n wait(SC_ZERO_TIME) para intentar mejorar
 *       el funcionamiento de SystemC
 */

#include "controller.h"

/*
 * Funciona a modo de main, dado que delega la inicializaci�n en
 * un m�todo init, realiza las G generaciones y finaliza enviando
 * los datos por salida.
 */
void controller::process()
{
	int i,j;
	
	init(); // inicializaci�n de datos

	for(i=0;i<G;i++) {
		for(j=0;j<256;i++) {
		}
	}

	memToOut(); // sacamos por salida OUTRO tras G generaciones
}

/*
 * Esta funci�n realiza el proceso de iniciar el conjunto
 */
void controller::init()
{
	int i,j;
	sc_uint<64> iniFCoste;

	INTRO->read(G); // leemos n�mero de generaciones
	for(i=0;i<256;i++) {
		for(j=0;j<10;j++) {
			INTRO->read(x[j]); // leemos valores iniciales
			iniVal->write(x[j]); // los mandamos a memoria
			wait(SC_ZERO_TIME);
		}
		INTRO->read(iniFCoste);
		iniRes->write(iniFCoste);
		wait(SC_ZERO_TIME);
	}
}


/*
 * Esta funci�n realiza el proceso de enviar a la salida
 */
void controller::memToOut()
{
	int i,j;
	sc_uint<64> tmp;

	for(i=0; i<256; ++i){
		addrA->write( (sc_uint<8>) i );        addrB->write( (sc_uint<8>) 0 );
		wait(SC_ZERO_TIME);
		for(j=0; j<11; ++j){
			fa[j]->read( tmp );
			OUTRO->write( tmp );
			if(j<10){
				fb[j]->read( tmp );    // para vaciar esta cola...
			}
			wait(SC_ZERO_TIME);
		}
	}
}