#ifndef CPU_HPP_
#define CPU_HPP_

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <array>

#include <SDL.h>

struct Chip8
{
	Chip8();
	void DecodeOpcode();
	void EmulateCycle();
	int  IsKeyDown(const char& key);
	void Load(const std::string& romPath);
	
	std::array<uint8_t, 16> V;
	std::array<uint8_t, 16> keys;
	std::array<uint8_t, 80> font;
	std::array<uint16_t, 16> stack;
	std::array<uint8_t, 4096> memory;
	std::array<uint8_t, 64 * 32> screen;
	
	uint16_t i;
	uint16_t sp;
	uint16_t pc;
	uint16_t opcode;

	uint8_t delayTimer;
	uint8_t soundTimer;
};

static void Expansion(std::array<uint8_t, 64 * 32> from, uint32_t* to)
{
	for (int i = 0; i < 2048; i++)
		to[i] = (from[i]) ? -1 : 0;
}


#endif // !CPU_HPP_
