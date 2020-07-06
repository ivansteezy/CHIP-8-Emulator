//#ifndef ROMLOADER_H
//#define ROMLOADER_H
//
//#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
//
//#include <experimental/filesystem>
//#include <iostream>
//#include <fstream>
//#include <string>
//
//#include "cpu.h"
//
//auto LoadRomInMachine(Chip8* machine, const std::string& romPath)
//{
//	machine->initialize();
//	std::cout << "Loading ROM: " << romPath << "..." << std::endl;
//
//	auto fileSize = std::experimental::filesystem::file_size(romPath);
//	auto buffer = std::make_unique<unsigned char>(fileSize);
//	std::basic_ifstream<unsigned char> ifs(romPath, std::ios::binary);
//	ifs.read(buffer.get(), fileSize);
//
//	if ((4096 - 512) > fileSize)
//	{
//		for (auto i = 0; i < fileSize - 1; ++i)
//		{
//			machine->memory[i + 512] = static_cast<uint8_t>(buffer.get()[i]); // Rom Memory begins at 0x200
//			std::cout << "Instruccion " << std::hex << static_cast<uint8_t>(buffer.get()[i]) << "." << std::endl;
//		}
//	}
//	else
//	{
//		std::cout << "ROM too large to fit in memory" << std::endl;
//	}
//
//	std::cout << "ROM correctly loaded" << std::endl;
//
//	return machine;
//}
//
//#endif
