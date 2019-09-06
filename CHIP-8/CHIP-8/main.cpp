//#include libreria para graficos ex. SDL, OpenGL etc.
//TODO crear la clase encargada de los opcodes 
#include "cpu.h"
#include <iostream>

Chip8* myChip = new Chip8();

int main()
{
	//Prueba de decodificacion de opcodes, a
	myChip->memory[0] = 0x8B;
	myChip->memory[1] = 0xF0;
	myChip->emulateCycle();

	return 0;
}