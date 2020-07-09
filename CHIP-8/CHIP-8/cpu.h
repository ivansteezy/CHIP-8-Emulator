#ifndef CPU_H_
#define CPU_H_

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

#include <SDL.h>


struct Chip8
{
	Chip8() {}
	//CHIP-8 tiene 35 opcodes, todo son de 2 bytes
	unsigned short opcode;

	//Tiene 4K de memoria, puede ser emulada tal que
	unsigned char memory[4096];

	//Existen 15 registros de 8 bits para uso general llamados Vx (x = {0x00, 0x01, 0x02... 0x0E})
	//Y un regitro para "carry flag" 
	unsigned char V[16];

	//Hay un registro indice <<I>> y program counter <<PC>> de 12bits que va de 0x000 a 0xFFF
	unsigned short i;
	unsigned short pc = 0;

	//El chip-8 contiene hard-codeada un font
	unsigned char font[80]
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0,	//0
		0x20, 0x60, 0x20, 0x20, 0x70,	//1
		0xF0, 0x10, 0xF0, 0x80, 0xF0,	//2
		0xF0, 0x10, 0xF0, 0x10, 0xF0,	//3
		0x90, 0x90, 0xF0, 0x10, 0x10,	//4
		0xF0, 0x80, 0xF0, 0x10, 0xF0,	//5
		0xF0, 0x80, 0xF0, 0x90, 0xF0,	//6
		0xF0, 0x10, 0x20, 0x40, 0x40,	//7
		0xF0, 0x90, 0xF0, 0x90, 0xF0,	//8
		0xF0, 0x90, 0xF0, 0x10, 0xF0,	//9
		0xF0, 0x90, 0xF0, 0x90, 0x90,	//A
		0xE0, 0x90, 0xE0, 0x90, 0xE0,	//B
		0xF0, 0x80, 0x80, 0x80, 0xF0,	//C
		0xE0, 0x90, 0x90, 0x90, 0xE0,	//D
		0xF0, 0x80, 0xF0, 0x80, 0xF0,	//E
		0xF0, 0x80, 0xF0, 0x80, 0x80,	//F
	};

	 char keys[16] = {
		SDL_SCANCODE_X, //0
		SDL_SCANCODE_1, //1
		SDL_SCANCODE_2, //2
		SDL_SCANCODE_3, //3
		SDL_SCANCODE_Q, //4
		SDL_SCANCODE_W, //5
		SDL_SCANCODE_E, //6
		SDL_SCANCODE_A, //7
		SDL_SCANCODE_S, //8
		SDL_SCANCODE_D, //9
		SDL_SCANCODE_Z, //A
		SDL_SCANCODE_C, //B
		SDL_SCANCODE_4, //C
		SDL_SCANCODE_R, //D
		SDL_SCANCODE_F, //E
		SDL_SCANCODE_V //F
	};
	
	
	/*Memory map
	+---------------+= 0xFFF (4095) End of Chip-8 RAM
	|               |
	|               |
	|               |
	|               |
	|               |
	| 0x200 to 0xFFF|
	|     Chip-8    |
	| Program / Data|
	|     Space     |
	|               |
	|               |
	|               |
	+- - - - - - - -+= 0x600 (1536) Start of ETI 660 Chip-8 programs
	|               |
	|               |
	|               |
	+---------------+= 0x200 (512) Start of most Chip-8 programs
	| 0x000 to 0x1FF|
	| Reserved for  |
	|  interpreter  |
	+---------------+= 0x000 (0) Start of Chip-8 RAM */

	//El sistema de graficos del CHIP-8 se realiza mediante una
	//instrucción, se realiza en modo XOR y si un pixel esta
	//apagado como resultado el registro VF se setteara, esto 
	//Es utilizado para la deteccion de colisiones

	//Los graficos del CHIP-8 son unicamente blanco o negro
	//con un total de 2048 pixeles(64px x 32px)
	char gfx[64 * 32];

	//El CHIP-8 no cuenta con ningun tipo de interrupcion 
	//Pero existen 2 registros que cuentan a 60hz hasta 0
	unsigned char delay_timer;
	unsigned char sound_timer;

	//Las especificacion de CHIP-8 no hacen mención a 
	//un stack pointer pero se debe implementar uno 
	//puesto que se debe guardar el program counter
	//en el caso de que se haga un llamado a una subrutina

	unsigned short stack[16];
	unsigned short sp;

	//Finalmente el CHIP-8 tiene un teclado HEX based (0x0, 0xF)
	bool drawFlag;

	void initialize();
	void emulateCycle();
	void decodeOpcode();
	void load(const std::string& romPath);
	int IsKeyDown(char key);
};

static void Expansion(char* from, uint32_t* to)
{
	for (int i = 0; i < 2048; i++)
		to[i] = (from[i]) ? -1 : 0;
}
#endif // !CPU_H_
