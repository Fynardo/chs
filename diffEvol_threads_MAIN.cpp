/*
 * Main module (top)
 *
 * Universidade Da Coru�a. 2015
 *
 * Marcos Horro
 * Christian Ponte
 */

#include "systemc.h"

#include "fifo.h"
#include "FPplano.h"
#include "controller.h"
#include "producerConsumer.h"
#include "producerConsumerMultiple.h"

// Nombres de ficheros de entrada
const char *fINTRO = "benchMover_fileEntradaDE.dat";
const char *Frand1 = "benchMover_fileRandom8bits_1.dat";
const char *Frand2 = "benchMover_fileRandom8bits_2.dat";
const char *Frand3 = "benchMover_fileRandom8bits_3.dat";
const char *Fhibridar = "benchMover_fileHibridar.dat";
const char *Flisto = "benchMover_fileListo.dat";
const char *Ffa = "benchMover_fileFA.dat";
const char *Ffb = "benchMover_fileFB.dat";

// Nombres de ficheros de salida
const char *FaddrA = "benchMover_fileAddrA.dat";
const char *FaddrB = "benchMover_fileAddrB.dat";
const char *FiniVal = "benchMover_fileIniVal.dat";
const char *FiniRes = "benchMover_fileIniRes.dat";
const char *Fr = "benchMover_fileR.dat";
const char *Fs = "benchMover_fileS.dat";
const char *Fo = "benchMover_fileO.dat";
const char *fOUTRO = "benchMover_fileSalidaDE.dat";

char* nombreIdx(char *cad, int idx)
{
	static char nombreYnumero[128];
	sprintf(nombreYnumero, "%s_%d", cad, idx);
	return nombreYnumero;
}

class top : public sc_module
{
	public:
		// Entradas a Controlador
		fifo_T<sc_uint<64>> *qINTRO;
		fifo_T<sc_uint<8>> *Qrand1, *Qrand2, *Qrand3;
		fifo_T<bool> *Qlisto;
		fifo_T<sc_uint<16>> *Qhibridar;
		fifo_T<sc_uint<64>> *Qfa[11], *Qfb[10];

		// Salidas de controlador
		fifo_T<sc_uint<8>> *QaddrA, *QaddrB;
		fifo_T<sc_uint<64>> *QiniVal, *QiniRes;
		fifo_T<sc_uint<64>> *Qr[10], *Qs[10], *Qo[10];
		fifo_T<sc_uint<64>> *qOUTRO;

		// Productores de cada cola de entrada
		productor<sc_uint<64>, double> *pINTRO;
		productor<sc_uint<8>, unsigned char> *Prand1, *Prand2, *Prand3;
		productor<sc_uint<16>, unsigned int> *Phibridar;
		productor<bool, bool> *Plisto;
		productorMultiple<sc_uint<64>, double, 11> *Pfa;
		productorMultiple<sc_uint<64>, double, 10> *Pfb;

		// Consumidores de las colas de salida
		consumidor<sc_uint<8>, unsigned char> *CaddrA, *CaddrB;
		consumidor<sc_uint<64>, double> *CiniVal, *CiniRes;
		consumidorMultiple<sc_uint<64>, double, 10> *Cr, *Cs, *Co;
		consumidor<sc_uint<64>, double> *cOUTRO;

		controller *instController; // Declaraci�n del controlador

		SC_CTOR(top) // the module constructor
		{
			// Instanciaci�n de las colas de entrada
			qINTRO = new fifo_T<sc_uint<64>>("INTRO", 1);
			Qrand1 = new fifo_T<sc_uint<8>>("rand1", 1);
			Qrand2 = new fifo_T<sc_uint<8>>("rand2", 1);
			Qrand3 = new fifo_T<sc_uint<8>>("rand3", 1);
			Qhibridar = new fifo_T<sc_uint<16>>("hibridar", 1);
			Qlisto = new fifo_T<bool>("listo", 1);
			for (int i=0; i<10; i++)
				Qfa[i] = new fifo_T<sc_uint<64>>(nombreIdx("Qfa", i), 1);
			for (int i=0; i<11; i++)
				Qfb[i] = new fifo_T<sc_uint<64>>(nombreIdx("Qfb", i), 1);

			// Instanciaci�n de las colas de salida
			QaddrA = new fifo_T<sc_uint<8>>("addrA", 1);
			QaddrB = new fifo_T<sc_uint<8>>("addrB", 1);
			QiniVal = new fifo_T<sc_uint<64>>("iniVal", 1);
			QiniRes = new fifo_T<sc_uint<64>>("iniRes", 1);
			for(int i=0; i<10; ++i){
				Qr[i] = new fifo_T<sc_uint<64>>(nombreIdx("Qr", i), 1);
				Qs[i] = new fifo_T<sc_uint<64>>(nombreIdx("Qs", i), 1);
				Qo[i] = new fifo_T<sc_uint<64>>(nombreIdx("Qo", i), 1);
			}
			qOUTRO = new fifo_T<sc_uint<64>>("OUTRO", 1);

			// Instanciaci�n de los productores
			pINTRO = new productor<sc_uint<64>, double>("INTRO", fINTRO);
			Prand1 = new productor<sc_uint<8>, unsigned char>("rand1", Frand1);
			Prand2 = new productor<sc_uint<8>, unsigned char>("rand2", Frand2);
			Prand3 = new productor<sc_uint<8>, unsigned char>("rand3", Frand3);
			Phibridar = new productor<sc_uint<16>, unsigned int>("hibridar", Fhibridar);
			Plisto = new productor<bool, bool>("listo", Flisto);
			Pfa = new productorMultiple<sc_uint<64>, double, 11>("fa", Ffa);
			Pfb = new productorMultiple<sc_uint<64>, double, 10>("fb", Ffb);

			// Instanciaci�n de los consumidores
			CaddrA = new consumidor<sc_uint<8>, unsigned char>("addrA", FaddrA);
			CaddrB = new consumidor<sc_uint<8>, unsigned char>("addrB", FaddrB);
			CiniVal = new consumidor<sc_uint<64>, double>("iniVal", FiniVal);
			CiniRes = new consumidor<sc_uint<64>, double>("iniRes", FiniRes);
			Cr = new consumidorMultiple<sc_uint<64>, double, 10>("r", Fr);
			Cs = new consumidorMultiple<sc_uint<64>, double, 10>("s", Fs);
			Co = new consumidorMultiple<sc_uint<64>, double, 10>("o", Fo);
			cOUTRO = new consumidor<sc_uint<64>, double>("OUTRO", fOUTRO);

			// Instanciaci�n del controlador
			instController = new controller("microcontroller");

			// Conexi�n productores
			pINTRO->entrada(*qINTRO);
			Prand1->entrada(*Qrand1);
			Prand2->entrada(*Qrand2);
			Prand3->entrada(*Qrand3);
			Phibridar->entrada(*Qhibridar);
			Plisto->entrada(*Qlisto);
			for (int i=0; i<11; i++)
				Pfa->entrada[i](*Qfa[i]);
			for (int i=0; i<10; i++)
				Pfb->entrada[i](*Qfb[i]);

			// Conexi�n consumidores
			CaddrA->resultado(*QaddrA);
			CaddrB->resultado(*QaddrB);
			CiniVal->resultado(*QiniVal);
			CiniRes->resultado(*QiniRes);
			for (int i=0; i<10; i++){
				Cr->resultado[i](*Qr[i]);
				Cs->resultado[i](*Qs[i]);
				Co->resultado[i](*Qo[i]);
			}
			cOUTRO->resultado(*qOUTRO);

			// Conexi�n controlador
			instController->INTRO(*qINTRO);
			instController->rand1(*Qrand1);
			instController->rand2(*Qrand2);
			instController->rand3(*Qrand3);
			instController->hibridar(*Qhibridar);
			instController->listo(*Qlisto);
			for (int i=0; i<11; i++)
				instController->fa[i](*Qfa[i]);
			for (int i=0; i<10; i++)
				instController->fb[i](*Qfb[i]);
			instController->addrA(*QaddrA);
			instController->addrB(*QaddrB);
			instController->iniVal(*QiniVal);
			instController->iniRes(*QiniRes);
			for (int i=0; i<10; i++){
				instController->r[i](*Qr[i]);
				instController->s[i](*Qs[i]);
				instController->o[i](*Qo[i]);
			}
			instController->OUTRO(*qOUTRO);
		}
};


int sc_main(int nargs, char* vargs[])
{
	top principalDE("top");
	sc_start();

	return 0;
}
