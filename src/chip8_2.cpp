// chip8_2.cpp : Defines the entry point for the application.
//



#include <iostream>
#include <chip8_2.h>
#include <game.hpp>

using namespace std;

#include <direct.h>
#define GetCurrentDir _getcwd

int main()
{
	// load ROM file
	// have a class for CPU model
	// will have copy of CPU model in our class
	// for each line of ROM, we will execute the instructions (some changes in memory)
	// 
	cout << "Hello CMake." << endl;

	// std::ifstream rom_file("ROMs\\IBM_Logo.ch8", std::ios::binary | std::ios::ate);
	// std::string rom_fname ("ROMs\\IBM_Logo.ch8");
	std::string rom_fname ("ROMs\\test_opcode.ch8");
	// std::string rom_fname ("ROMs\\bc_test.ch8");

	bool debugEnable = false;
	
	Game chip8_emu;

	chip8_emu.initialize(rom_fname, debugEnable);

	chip8_emu.run();

	// small snippet to check current directory
	char buff[FILENAME_MAX]; //create string buffer to hold path
   	GetCurrentDir( buff, FILENAME_MAX );
	std::cout << __func__ << ": current cwd is " << string(buff) << std::endl;
	// end of snippet

}