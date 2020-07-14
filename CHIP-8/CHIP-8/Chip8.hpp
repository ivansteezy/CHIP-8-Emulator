#ifndef CPU_HPP_
#define CPU_HPP_

#include <functional>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <array>
#include <map>

#include <SDL.h>

struct Chip8
{
	Chip8();
	void DecodeOpcode();
	void EmulateCycle();
	int  IsKeyDown(const char& key);
	void Load(const std::string& romPath);
	void CreateInstructionTable();

	std::map<uint16_t, std::function<void()>> m_instructionTable;
	
	std::array<uint8_t, 16> V;
	std::array<uint8_t, 16> keys;
	std::array<uint8_t, 80> font;
	std::array<uint16_t, 16> stack;
	std::array<uint8_t, 4096> memory;
	char screen[2048];
	
	uint16_t i;
	uint16_t sp;
	uint16_t pc;
	uint16_t opcode;

	uint8_t delayTimer;
	uint8_t soundTimer;
};


#endif // !CPU_HPP_
