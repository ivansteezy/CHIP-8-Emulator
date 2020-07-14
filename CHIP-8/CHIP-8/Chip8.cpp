#include "Chip8.hpp"
#include "Renderer.hpp"

#include <random>	//For std::random_device
#include <vector>	//For std::vector
#include <iostream>	//For std::cout
#include <cstddef>	//For std::byte
#include <filesystem>
#include <iterator>

Chip8::Chip8() : pc(0x200), opcode(0), i(0), sp(0), delayTimer(0), 
				 soundTimer(0), screen{0}, stack{0}, V{0}, memory{0}
{
	keys = {
		SDL_SCANCODE_X,
		SDL_SCANCODE_1,
		SDL_SCANCODE_2,
		SDL_SCANCODE_3,
		SDL_SCANCODE_Q,
		SDL_SCANCODE_W,
		SDL_SCANCODE_E,
		SDL_SCANCODE_A,
		SDL_SCANCODE_S,
		SDL_SCANCODE_D,
		SDL_SCANCODE_Z,
		SDL_SCANCODE_C,
		SDL_SCANCODE_4,
		SDL_SCANCODE_R,
		SDL_SCANCODE_F,
		SDL_SCANCODE_V
	};

	font = {
		0xF0, 0x90, 0x90, 0x90, 0xF0,
		0x20, 0x60, 0x20, 0x20, 0x70,
		0xF0, 0x10, 0xF0, 0x80, 0xF0,
		0xF0, 0x10, 0xF0, 0x10, 0xF0,
		0x90, 0x90, 0xF0, 0x10, 0x10,
		0xF0, 0x80, 0xF0, 0x10, 0xF0,
		0xF0, 0x80, 0xF0, 0x90, 0xF0,
		0xF0, 0x10, 0x20, 0x40, 0x40,
		0xF0, 0x90, 0xF0, 0x90, 0xF0,
		0xF0, 0x90, 0xF0, 0x10, 0xF0,
		0xF0, 0x90, 0xF0, 0x90, 0x90,
		0xE0, 0x90, 0xE0, 0x90, 0xE0,
		0xF0, 0x80, 0x80, 0x80, 0xF0,
		0xE0, 0x90, 0x90, 0x90, 0xE0,
		0xF0, 0x80, 0xF0, 0x80, 0xF0,
		0xF0, 0x80, 0xF0, 0x80, 0x80,
	};

	for (int i = 0; i < 80; ++i) 
	{
		memory[i] = font[i];
	}

	CreateInstructionTable();
}

void Chip8::EmulateCycle()
{
	opcode = memory[pc] << 8 | memory[pc + 1];
	m_instructionTable.at(opcode & 0xF000)();
}

void Chip8::Load(const std::string& romPath)
{
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

void Chip8::CreateInstructionTable()
{
	m_instructionTable.insert(std::make_pair<uint16_t, std::function<void()>>(0x0000, [&]() {
		std::map<uint16_t, std::function<void()>> subMask;

		subMask.insert(std::make_pair<uint16_t, std::function<void()>>(0x0000, [&]() {	//CLS
			for (int i = 0; i < (64 * 32); ++i)
			{
				screen[i] = 0;
			}
			pc += 2;
			}));

		subMask.insert(std::make_pair<uint16_t, std::function<void()>>(0x000E, [&]() {	//RET
			--sp;
			pc = stack[sp];
			pc += 2;
			}));

		subMask.at(opcode & 0x000F)();
	}));

	m_instructionTable.insert(std::make_pair<uint16_t, std::function<void()>>(0x1000, [&]() {	/* 1NNN | Jumps to address NNN */
		pc = opcode & 0x0FFF;
	}));

	m_instructionTable.insert(std::make_pair<uint16_t, std::function<void()>>(0x2000, [&]() {	/* 2NNN | Calls subroutine at NNN */
		stack[sp] = pc;
		++sp;
		pc = opcode & 0x0FFF;
	}));

	m_instructionTable.insert(std::make_pair<uint16_t, std::function<void()>>(0x3000, [&]() {	/* 3XNN | Skip the next instruction if Vx equals NN */
		if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) pc += 4;
		else												pc += 2;
	}));

	m_instructionTable.insert(std::make_pair<uint16_t, std::function<void()>>(0x4000, [&]() {	/* 4XNN | Skips the next instruction if VX doesnt equal NN */
		if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) pc += 4;
		else												pc += 2;
	}));

	m_instructionTable.insert(std::make_pair<uint16_t, std::function<void()>>(0x5000, [&]() {	/* 5XY0 | Skips the next instruction if VX equals VY. */
		if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) pc += 4;
		else														pc += 2;
	}));

	m_instructionTable.insert(std::make_pair<uint16_t, std::function<void()>>(0x6000, [&]() {	/* 6XNN | Sets VX to NN */
		V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
		pc += 2;
	}));

	m_instructionTable.insert(std::make_pair<uint16_t, std::function<void()>>(0x7000, [&]() {	/* 7XNN | Adds NN to VX (Carry flag is not changed) */
		V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
		pc += 2;
	}));

	m_instructionTable.insert(std::make_pair<uint16_t, std::function<void()>>(0x8000, [&]() {
		std::map<uint16_t, std::function<void()>> subMask;

		subMask.insert(std::make_pair<uint16_t, std::function<void()>>(0x0000, [&]() {	/* LD Vx, Vy | Sets Vx to the value of Vy */
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
			pc += 2;
		}));

		subMask.insert(std::make_pair<uint16_t, std::function<void()>>(0x0001, [&]() {	/* OR Vx, Vy | Sets Vx to Vx or Vy (Bitwise or) */
			V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
			pc += 2;
		}));

		subMask.insert(std::make_pair<uint16_t, std::function<void()>>(0x0002, [&]() {	/* AND Vx, Vy | Sets Vx to Vx and Vy (Bitwise and) */
			V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
			pc += 2;
		}));

		subMask.insert(std::make_pair<uint16_t, std::function<void()>>(0x0003, [&]() {	/* XOR Vx, Vy | Sets Vx to Vx xor Vy */
			V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
			pc += 2;
		}));

		subMask.insert(std::make_pair<uint16_t, std::function<void()>>(0x0004, [&]() {	/*ADD Vx, Vy | Adds Vy to Vx. VG is set to 1 when there's a carry, and to 0 when there isn't */
			V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
			if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])) V[0xF] = 1;
			else																V[0xF] = 0;
			pc += 2;
		}));

		subMask.insert(std::make_pair<uint16_t, std::function<void()>>(0x0005, [&]() {	/*SUB Vx, Vy | Vy is subtracted from Vx. Vf is set to 0 when there's a borrow, and 1 when there isnt'n */
			if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]) V[0xF] = 0;
			else													   V[0xF] = 1;
			V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
			pc += 2;
		}));

		subMask.insert(std::make_pair<uint16_t, std::function<void()>>(0x0006, [&]() {	/*SHR Vx {, Vy} | Stores the least significant bit of Vx in Vf and then shifts Vx to the right by 1 */
			V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
			V[(opcode & 0x0F00) >> 8] >>= 1;
			pc += 2;
		}));

		subMask.insert(std::make_pair<uint16_t, std::function<void()>>(0x0007, [&]() {	/*SUBN Vx, Vy | Sets vx to Vy minus Vx. Vf is set to 0 when there's a borrow, and 1 when isn't */
			if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]) V[0xF] = 0;
			else													   V[0xF] = 1;
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00 >> 8)];
			pc += 2;
		}));

		subMask.insert(std::make_pair<uint16_t, std::function<void()>>(0x000E, [&]() {	/*SHL Vx {, Vy} | Stores the most significant bit of Vx in Vf and then shifts Vx to left by 1 */
			V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
			V[(opcode & 0x0F00)] <<= 1;
			pc += 2;
		}));

		subMask.at(opcode & 0x000F)();
	}));

	m_instructionTable.insert(std::make_pair<uint16_t, std::function<void()>>(0x9000, [&]() { /*SNE Vx, Vy | Skips the next instruction if Vx doesn't equal Vy */
		if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) pc += 4;
		else														pc += 2;
	}));

	m_instructionTable.insert(std::make_pair<uint16_t, std::function<void()>>(0xA000, [&]() {
		i = opcode & 0x0FFF;
		pc += 2;
	}));

	m_instructionTable.insert(std::make_pair<uint16_t, std::function<void()>>(0xB000, [&]() {	/* BNNN - Jumps to the address NNN plus V0. */
		pc = (opcode & 0x0FFF) + V[0];
	}));

	m_instructionTable.insert(std::make_pair<uint16_t, std::function<void()>>(0xC000, [&]() {	/* RND Vx, Byte | Set Vx = random byte AND kk  check this!!*/
		V[(opcode & 0x0F00) >> 8] = (rand() % (0xFF + 1)) & (opcode & 0x00FF);
		pc += 2;
	}));

	m_instructionTable.insert(std::make_pair<uint16_t, std::function<void()>>(0xD000, [&]() {	/* DRW Vx, Vy, nibble | Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision*/
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
					if (screen[(x + xline + ((y + yline) * 64))] == 1)
					{
						V[0xF] = 1;
					}
					screen[x + xline + ((y + yline) * 64)] ^= 1;
				}
			}
		}
		pc += 2;
	}));

	m_instructionTable.insert(std::make_pair<uint16_t, std::function<void()>>(0xE000, [&]() {	/* RND Vx, Byte | Set Vx = random byte AND kk  check this!!*/
		std::map<uint16_t, std::function<void()>> subMask;

		subMask.insert(std::make_pair<uint16_t, std::function<void()>>(0x000E, [&]() {	/* SKP Vx | Skip next instruction if key with the value of Vx is pressed */
			if (IsKeyDown(V[(opcode & 0x0F00) >> 8])) pc += 4;
			else									  pc += 2;
			}));

		subMask.insert(std::make_pair<uint16_t, std::function<void()>>(0x0001, [&]() {	/* SKNP Vx | Skip next instruction if key with the value of Vx is not */
			if (!IsKeyDown(V[(opcode & 0x0F00) >> 8])) pc += 4;
			else									   pc += 2;
			}));

		subMask.at(opcode & 0x000F)();
	}));

	m_instructionTable.insert(std::make_pair<uint16_t, std::function<void()>>(0xF000, [&]() {
		std::map<uint16_t, std::function<void()>> subMask;

		subMask.insert(std::make_pair<uint16_t, std::function<void()>>(0x0007, [&]() {	/* LD Vx, Dt | Set Vx = delay timer value */
			V[(opcode & 0x0F00) >> 8] = delayTimer;
			pc += 2;
		}));

		subMask.insert(std::make_pair<uint16_t, std::function<void()>>(0x000A, [&]() {	/* LD Vx, Dt | Set Vx = delay timer value */
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
		}));

		subMask.insert(std::make_pair<uint16_t, std::function<void()>>(0x0015, [&]() {	/* LD DT, Vx | Set delay timer = Vx */
			delayTimer = V[(opcode & 0x0F00) >> 8];
			pc += 2;
		}));

		subMask.insert(std::make_pair<uint16_t, std::function<void()>>(0x0018, [&]() {	/* LD ST, Vx | Set sound timer = Vx  */
			soundTimer = V[(opcode & 0x0F00) >> 8];
			pc += 2;
		}));

		subMask.insert(std::make_pair<uint16_t, std::function<void()>>(0x001E, [&]() {	/* ADD I, Vx | Set I = I + Vx */
			if (i + V[(opcode & 0x0F00) >> 8] > 0xFFF) V[0xF] = 1;
			else									   V[0xF] = 0;
			i += V[(opcode & 0x0F00) >> 8];
			pc += 2;
		}));

		subMask.insert(std::make_pair<uint16_t, std::function<void()>>(0x0029, [&]() {	/* LD F, Vx | Set I = location of sprite for digit Vx */
			i = V[(opcode & 0x0F00) >> 8] * 0x5;
			pc += 2;
		}));

		subMask.insert(std::make_pair<uint16_t, std::function<void()>>(0x0033, [&]() {	/* LD F, Vx | Set I = location of sprite for digit Vx */
			memory[i] = V[(opcode & 0x0F00) >> 8] / 100;
			memory[i + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
			memory[i + 2] = V[(opcode & 0x0F00) >> 8] % 10;
			pc += 2;
		}));

		subMask.insert(std::make_pair<uint16_t, std::function<void()>>(0x0055, [&]() {	/* LD[I], Vx | Store registers V0 trough Vx in memory starting at location I */
			for (int j = 0; j <= ((opcode & 0x0F00) >> 8); j++)
				memory[i + j] = V[j];
			pc += 2;
		}));

		subMask.insert(std::make_pair<uint16_t, std::function<void()>>(0x0065, [&]() {	/*LD Vx, [I] | Read registers V0 through Vx from memory starting at location I*/
			for (int j = 0; j <= ((opcode & 0x0F00) >> 8); j++)
				V[j] = memory[i + j];
			pc += 2;
		}));

		subMask.at(opcode & 0x00FF)();

	}));
}

int Chip8::IsKeyDown(const char& key)
{
	return SDL_GetKeyboardState(NULL)[keys[key]];
}
