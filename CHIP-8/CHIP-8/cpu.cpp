#include "cpu.h"
#include "opcodes.h"
#include <iostream>

void _cpu::initialize()
{
	//Inicializamos los registros y memoria una vez

	pc = 0x200;		//El contador del programa comienza en la direccion 0x200
	opcode = 0;		//Resetea el opcode actual
	i = 0;			//Restea el index registrer
	sp = 0;			//Restea el stack pointer

	//Se limpia el display (duda con estos bucles)
	for (int i = 0; i < (64 * 32); ++i)
	{
		gfx[i] = 0;
	}

	//Se limpia el stack
	for (int j = 0; j < 16; ++j) 
	{
		stack[j] = 0;
	}

	//Se limpian los registros de V0-VF
	for (int k = 0; k < 16; ++k)
	{
		V[k] = 0;
	}

	//Se limpia la memoria
	for (int l = 0; l < 4096; ++l)
	{
		memory[l] = 0;
	}

	//TODO cargar el fonset predeterminado y resettear los timers
}

void _cpu::emulateCycle()
{
	//Fetch Opcode
	//Se obtendran los opcodes almacenados en el array en el indice el pc, 
	//un opcodo es 2 bytes long así que obtendremos 2 bytes y los mezclaremos
	opcode = memory[0] << 8 | memory[1];
	//Decode Opcode
	//Dependiendo de "opcode", verificaremos en la tabla de opcodes la instruccion correspondiente (la tabla es opcodes.h)
	decodeOpcode();

	//Actualizamos los timers
	/*
	if (delay_timer > 0)
	{
		--delay_timer;
	}
	if (sound_timer > 0)
	{
		if (sound_timer == 1)
		{
			std::cout << "Beep" << std::endl;
			--sound_timer;
		}
	}*/
}

void _cpu::decodeOpcode()
{
	//TODO escribir todos los opcodes tal como se hizo aquí.
	switch (opcode & 0xF000)
	{
	case 0xA000: //Aquellos que comiencen con A
		//Unicamente existe una instruccion "ANNN"
		//Execute opcode
		i = opcode & 0x0FFF; //Sets I to the adress NNN
		pc += 2;
		std::cout << "La instruccion fue ANNN e I vale: " << i <<
			"y PC vale: " << pc << std::endl;
		break;

	case 0x0000:
		if ((opcode & 0x00FF) == 0x00E0) /*00E0 | Clears the screen.*/
		{
			std::cout << "LA instruccion fue CLS" << std::endl;
			for (int i = 0; i < 2048; ++i) { gfx[i] = 0; }
			pc += 2;
		}
		if ((opcode & 0x00FF) == 0x00EE) /*00EE | Returns from a sub routine.*/
		{ /*Ojo*/
			std::cout << "La instruccion fue RET" << std::endl;
			pc = stack[sp--];
			pc += 2;
		}
		break;

	case 0x1000: /* 1NNN | Jumps to address NNN */
		pc = opcode & 0x0FFF;
		std::cout << "La instrucion es ir a la direccion: " << std::hex << pc << std::endl;
		break;
		/*from here...*/
	case 0x2000: /* 2NNN | Calls subroutine at NNN */
		stack[++sp] = pc;
		pc = opcode & 0x0FFF;
		break;

	case 0x3000: /* 3XNN | Skip the next instruction if Vx equals NN */
		if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
		{
			pc += 4;
		}
		else
		{
			pc += 2;
		}
		break;

	case 0x4000: /* 4XNN | Skips the next instruction if VX doesnt equal NN */
		if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
		{
			pc += 4;
		}
		else
		{
			pc += 2;
		}
		break;

	case 0x5000: /* 5XY0 | Skips the next instruction if VX equals VY. */
		if (V[(opcode & 0x0F00) >> 8] == V[opcode & 0x00F0] >> 4)
		{
			pc += 4;
		}
		else
		{
			pc += 2;
		}
		break;

	case 0x6000: /* 6XNN | Sets VX to NN */
		V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
		pc += 2;
		break;

	case 0x7000: /* 7XNN | Adds NN to VX (Carry flag is not changed) */
		/*TODO continuar con las instrucciones*/
		break;
	}
}