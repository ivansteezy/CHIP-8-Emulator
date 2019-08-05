//#include libreria para graficos ex. SDL, OpenGL etc.
//TODO crear la clase encargada de los opcodes 
#include "cpu.h"
#include <iostream>

_cpu* myChip = new _cpu();

int main()
{
	//Prueba de decodificacion de opcodes, a
	myChip->memory[0] = 0x1B;
	myChip->memory[1] = 0xCE;
	myChip->emulateCycle();

	return 0;
}