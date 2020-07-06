//#include libreria para graficos ex. SDL, OpenGL etc.
//TODO crear la clase encargada de los opcodes 
#include <iostream>

#include "cpu.h"
Chip8* myChip = new Chip8();

int main()
{
	//Prueba de decodificacion de opcodes, a
	myChip->memory[0] = 0x8B;
	myChip->memory[1] = 0xF0;

	myChip->memory[2] = 0x8B;
	myChip->memory[3] = 0xF0;
	myChip->emulateCycle();

	// this now load rom file into memory
	myChip->load("C:\\Users\\Iván\\Downloads\\PONG");
	return 0;
}