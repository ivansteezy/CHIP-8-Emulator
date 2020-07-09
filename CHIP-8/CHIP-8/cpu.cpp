/*Ninji 2019*/
#include "cpu.h"

#include <random>	//For std::random_device
#include <vector>	//For std::vector
#include <iostream>	//For std::cout
#include <cstddef>	//For std::byte
#include <filesystem>
#include <iterator>

void Chip8::initialize()
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

	for (int i = 0; i < 80; ++i) 
	{
		memory[i] = font[i];
	}

	delay_timer = 0;
	sound_timer = 0;

	srand(time(NULL));
}

void Chip8::emulateCycle()
{
	opcode = memory[pc] << 8 | memory[pc + 1];
	decodeOpcode();
}
void Chip8::decodeOpcode()
{
	switch (opcode & 0xF000)
	{
	case 0x0000:
		switch (opcode & 0x000F)
		{
		case 0x0000:
			for (int i = 0; i < 2048; ++i)
			{
				gfx[i] = 0;
			}
			drawFlag = true;
			pc += 2;
			break;

		case 0x000E:
			--sp;
			pc = stack[sp];
			pc += 2;
			break;
		}
		break;

	case 0x1000: /* 1NNN | Jumps to address NNN */
		pc = opcode & 0x0FFF;
		break;
		/*from here...*/
	case 0x2000: /* 2NNN | Calls subroutine at NNN */
		//if (sp < 16)
		stack[sp] = pc;
		++sp;
		pc = opcode & 0x0FFF;
		break;

	case 0x3000: /* 3XNN | Skip the next instruction if Vx equals NN */
		if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
			pc += 4;
		else
			pc += 2;
		break;

	case 0x4000: /* 4XNN | Skips the next instruction if VX doesnt equal NN */
		if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))	
			pc += 4;
		else 
			pc += 2;
		break;

	case 0x5000: /* 5XY0 | Skips the next instruction if VX equals VY. */
		if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
            pc += 4;
        else
            pc += 2;
        break;

	case 0x6000: /* 6XNN | Sets VX to NN */
		V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
		pc += 2;
		break;

	case 0x7000: /* 7XNN | Adds NN to VX (Carry flag is not changed) */
		V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
		pc += 2;
		break;

	case 0x8000:
		switch (opcode & 0x000F)
		{
		case 0x0000: /*LD Vx, Vy | Sets Vx to the value of Vy */
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
            pc += 2;
			break;

		case 0x0001: /*OR Vx, Vy | Sets Vx to Vx or Vy (Bitwise or) */
			V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		case 0x0002: /*AND Vx, Vy | Sets Vx to Vx and Vy (Bitwise and) */
			V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		case 0x0003: /*XOR Vx, Vy | Sets Vx to Vx xor Vy */
			V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		case 0x0004: /*ADD Vx, Vy | Adds Vy to Vx. VG is set to 1 when there's a carry, and to 0 when there isn't */
			V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
			if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
				V[0xF] = 1;
			else
				V[0xF] = 0;
			pc += 2;
			break;

		case 0x0005: /*SUB Vx, Vy | Vy is subtracted from Vx. Vf is set to 0 when there's a borrow, and 1 when there isnt'n */
			if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
				V[0xF] = 0;
			else
				V[0xF] = 1;

			V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		case 0x0006: /*SHR Vx {, Vy} | Stores the least significant bit of Vx in Vf and then shifts Vx to the right by 1 */
			V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
			V[(opcode & 0x0F00) >> 8] >>= 1;
			pc += 2;
			break;

		case 0x0007: /*SUBN Vx, Vy | Sets vx to Vy minus Vx. Vf is set to 0 when there's a borrow, and 1 when isn't */
			if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])
				V[0xF] = 0;
			else
				V[0xF] = 1;
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00 >> 8)];
			pc += 2;
			break;
		case 0x000E: /*SHL Vx {, Vy} | Stores the most significant bit of Vx in Vf and then shifts Vx to left by 1 */
			V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
			V[(opcode & 0x0F00)] <<= 1;
			pc += 2;
			break;
		default:
			std::cout << "No se conoce el opcode: " << opcode << std::endl;
		}
		break;
	
	case 0x9000: /*SNE Vx, Vy | Skips the next instruction if Vx doesn't equal Vy */
		if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
			pc += 4;
		else 
			pc += 2;
		break;

	case 0xA000:
		//Unicamente existe una instruccion "ANNN"
		//Execute opcode
		i = opcode & 0x0FFF; //Sets I to the adress NNN
		pc += 2;
		break;
	case 0xB000:	// BNNN - Jumps to the address NNN plus V0.
		pc = (opcode & 0x0FFF) + V[0];
		break;

	case 0xC000:	/* RND Vx, Byte | Set Vx = random byte AND kk  check this!!*/
	{			
		/*std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_real_distribution<double> dist(0x00, 0xFF);
		V[(opcode & 0x0F00) >> 8] = dist(mt);*/
		V[(opcode & 0x0F00) >> 8] = (rand() % (0xFF + 1)) & (opcode & 0x00FF);
		pc += 2;
		break;
	}

	case 0xD000: /* DRW Vx, Vy, nibble | Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision*/
	{
		unsigned short x = V[(opcode & 0x0F00) >> 8];
		unsigned short y = V[(opcode & 0x00F0) >> 4];
		unsigned short height = opcode & 0x000F;
		unsigned short pixel;

		V[0xF] = 0;
		for (int yline = 0; yline < height; yline++)
		{
			pixel = memory[i + yline];
			for (int xline = 0; xline < 8; xline++)
			{
				if ((pixel & (0x80 >> xline)) != 0)
				{
					if (gfx[(x + xline + ((y + yline) * 64))] == 1)
					{
						V[0xF] = 1;
					}
					gfx[x + xline + ((y + yline) * 64)] ^= 1;
				}
			}
		}

		drawFlag = true;
		pc += 2;
	}
	break;

	case 0xE000:
		switch (opcode & 0x000F)
		{
		case 0x000E:	/* SKP Vx | Skip next instruction if key with the value of Vx is pressed */
			if (IsKeyDown(V[(opcode & 0x0F00) >> 8]))
				//pc = (pc += 2) & 0xFFF;
				pc += 4;
			else
				pc += 2;
			break;
		case 0x0001:	/* SKNP Vx | Skip next instruction if key with the value of Vx is not */
			if (!IsKeyDown(V[(opcode & 0x0F00) >> 8]))
				//pc = (pc += 2) & 0xFFF;
				pc += 4;
			else
				pc += 2;
			break;
		}
		break;

	case 0xF000:
		switch (opcode & 0x00FF)
		{
		case 0x0007:	/* LD Vx, Dt | Set Vx = delay timer value */
			V[(opcode & 0x0F00) >> 8] = delay_timer;
			pc += 2;
			break;

		case 0x000A:	/* LD Vx, K | Wait for a key press, store the value of the key in Vx. */
		{
			bool key_pressed = false;

			for (int i = 0; i < 16; i++)
			{
				if (keys[i] != 0)
				{
					V[(opcode & 0x0F00) >> 8] = i;
					key_pressed = true;
				}
			}

			if (!key_pressed)
				return;
			pc += 2;
		}	
		break;	

		case 0x0015:	/* LD DT, Vx | Set delay timer = Vx */
			delay_timer = V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;

		case 0x0018:	/* LD ST, Vx | Set sound timer = Vx  */
			sound_timer = V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;

		case 0x001E:	/* ADD I, Vx | Set I = I + Vx */
			if (i + V[(opcode & 0x0F00) >> 8] > 0xFFF)
				V[0xF] = 1;
			else
				V[0xF] = 0;
			i += V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;

		case 0x0029:	/* LD F, Vx | Set I = location of sprite for digit Vx */ 
			i = V[(opcode & 0x0F00) >> 8] * 0x5;
			pc += 2;
			break;

		case 0x0033:	/* LD B, Vx | Store BCD representation of Vx in memory location I, I+1 and I+2 */
			memory[i]     = V[(opcode & 0x0F00) >> 8] / 100;
			memory[i + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
			memory[i + 2] = V[(opcode & 0x0F00) >> 8] % 10;
			pc += 2;
			break;

		case 0x0055:	/* LD[I], Vx | Store registers V0 trough Vx in memory starting at location I */
			for (int j = 0; j <= ((opcode & 0x0F00) >> 8); j++)
				memory[i + j] = V[j];

			//i += ((opcode & 0x0F00) >> 8) + 1;
			pc += 2;
			break;

		case 0x0065:	/*LD Vx, [I] | Read registers V0 through Vx from memory starting at location I*/
			for (int j = 0; j <= ((opcode & 0x0F00) >> 8); j++)
				V[j] = memory[i + j];

			//i += ((opcode & 0x0F00) >> 8) + 1;
			pc += 2; 
			break;
		}
		break;
	}
}

void Chip8::load(const std::string& romPath)
{
	initialize();
	std::cout << "Loading ROM: " << romPath << "..." << std::endl;

	std::ifstream file(romPath, std::ios::binary);
	file.unsetf(std::ios::skipws);
	std::streampos fileSize;

	file.seekg(0, std::ios::end);
	fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<unsigned char> buffer;
	buffer.reserve(fileSize);
	buffer.insert(buffer.begin(), std::istream_iterator<unsigned char>(file), std::istream_iterator<unsigned char>());

	if ((4096 - 512) > fileSize)
	{
		for (auto i = 0; i < fileSize; i++)
		{
			memory[i + 512] = buffer.at(i);
		}
	}
}

int Chip8::IsKeyDown(char key)		//in line this
{
	const Uint8* sdlKeys = SDL_GetKeyboardState(NULL);
	Uint8 realKey = keys[(int)key];
	return sdlKeys[realKey];
}
