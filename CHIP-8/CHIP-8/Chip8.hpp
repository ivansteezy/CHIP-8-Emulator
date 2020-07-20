#ifndef CPU_HPP_
#define CPU_HPP_

#include <SDL.h>

#include <functional>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <random>
#include <array>
#include <map>

namespace Machine
{
	struct Chip8
	{
		Chip8();

		void EmulateCycle();
		void CreateInstructionTable();
		int  IsKeyDown(const char& key);
		void Load(const std::string& romPath);

		char screen[2048];
		std::array<uint8_t, 16> V;
		std::array<uint8_t, 16> keys;
		std::array<uint8_t, 80> font;
		std::array<uint16_t, 16> stack;
		std::array<uint8_t, 4096> memory;

		std::map<uint16_t, std::function<void()>> instructionTable;

		uint16_t i;
		uint16_t sp;
		uint16_t pc;
		uint16_t opcode;

		uint8_t delayTimer;
		uint8_t soundTimer;
	};
}

#endif
