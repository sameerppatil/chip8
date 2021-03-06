#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>

#define ROM_START_ADDRESS 0x200u
#define ROM_END_ADDRESS 0x1000u
#define FONT_START_LOCATION 0x50u
#define INIT_MEMORY	0x0u

class CPU {
private:
	uint16_t opcode;
	uint8_t memory[4096]{};
	uint8_t regV[16];
	uint16_t index{};
	uint16_t pc{};
	uint16_t stack[16]{};
	uint8_t stack_pointer{};


	// opcode specific portions
	uint8_t vx_id;
	uint8_t vy_id;
	uint8_t nn;
	uint16_t nnn;
	
	uint8_t delay_timer{};
	uint8_t sound_timer{};

	unsigned char chip8_fontset[80] =
	{ 
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};
	void clear_display(void);
public:
	bool screen_update;
	CPU() {}
	uint32_t display[ 64 * 32 ] {};
	void loadMemory(std::string& fs);
	void initialize(void);
	void emulateCycle(void);

	void extract_vx_nibble(void);
	void extract_vy_nibble(void);
	void extract_nn_nibbles(void);
	void extract_nnn_nibbles(void);

	
};

#define CHIP8_OPCODE_CALL_ROUTINE 0x0000
#define CHIP8_OPCODE_CLEAR_SCREEN 0x00E0
#define CHIP8_OPCODE_CALL_RETURN 0x00EE
#define CHIP8_OPCODE_JUMP_TO_ADDR 0x1000
#define CHIP8_OPCODE_CALL_SUBROUTINE 0x2000
#define CHIP8_OPCODE_VX_EQUAL_CONST 0x3000
#define CHIP8_OPCODE_VX_NOT_EQUAL_CONST 0x4000
#define CHIP8_OPCODE_VX_EQUAL_VY 0x5000
#define CHIP8_OPCODE_SET_VX_TO_CONST 0x6000
#define CHIP8_OPCODE_ADD_NN_TO_VX 0x7000
#define CHIP8_PARTIAL_OPCODE_ASSIGN_VX_TO_VY 0x8000

//following opcode require first and 4th nibble to be extracted
#define CHIP8_PARTIAL_OPCODE_SET_VX_TO_VY_OR_VX 0x8001
#define CHIP8_PARTIAL_OPCODE_SET_VX_TO_VY_AND_VX 0x8002
#define CHIP8_PARTIAL_OPCODE_SET_VX_TO_VY_XOR_VX 0x8003
#define CHIP8_PARTIAL_OPCODE_SET_VX_TO_VY_PLUS_VX 0x8004
#define CHIP8_PARTIAL_OPCODE_SET_VX_TO_VX_MINUS_VY 0x8005
#define CHIP8_PARTIAL_OPCODE_SET_RIGHT_SHIFT_VX 0x8006
#define CHIP8_PARTIAL_OPCODE_SET_VX_TO_VY_MINUS_VX 0x8007
#define CHIP8_PARTIAL_OPCODE_SET_LEFT_SHIFT_VX 0x800E
//partial opcode bit is over now

#define CHIP8_OPCODE_SKIP_NEXT_IF_VX_NOT_EQUAL_TO_VY 0x9000
#define CHIP8_OPCODE_SET_REG_INDEX_TO_ADDR 0xA000
#define CHIP8_OPCODE_JUMP_TO_ADDR_PLUS_V00 0xB000
#define CHIP8_OPCODE_SET_VX_TO_RAND_AND_NN 0xC000
#define CHIP8_OPCODE_DISPLAY 0xD000
#define CHIP8_PARTIAL_OPCODE_KEYPAD_PRESSED 0xE000

// partial opcodes start
#define CHIP8_PARTIAL_OPCODE_KEYPAD_PRESSED_IS_VX 0xE09E
#define CHIP8_PARTIAL_OPCODE_KEYPAD_PRESSED_IS_NOT_VX 0xE0A1
// partial opcodes end

#define CHIP8_PARTIAL_OPCODE_MISC_OPCODES 0xF000

// partial opcodes start
#define CHIP8_PARTIAL_OPCODE_SET_VX_TO_DELAY 0xF007
#define CHIP8_PARTIAL_OPCODE_SET_VX_TO_KEYPAD 0xF00A
#define CHIP8_PARTIAL_OPCODE_SET_DELAY_TIMER_TO_VX 0xF015
#define CHIP8_PARTIAL_OPCODE_SET_SOUND_TIMER_TO_VX 0xF018
#define CHIP8_PARTIAL_OPCODE_SET_INDX_REG_TO_INDEX_REG_PLUS_VX 0xF01E
#define CHIP8_PARTIAL_OPCODE_SET_INDX_REG_SPRITE_ADD_OF_VX 0xF029
#define CHIP8_PARTIAL_OPCODE_BCD_OF_VX 0xF033
#define CHIP8_PARTIAL_OPCODE_DUMP_ALL_REGS_TO_INDX_REG_MEM 0xF055
#define CHIP8_PARTIAL_OPCODE_LOAD_ALL_REGS_FROM_INDEX_REG_MEM 0xF065
// partial opcodes end

#define FIRST_NIBBLE_MASK 				0xF000
#define NNN_NIBBLES_MASK 				0x0FFF
#define VX_NIBBLE_MASK 					0x0F00
#define VY_NIBBLE_MASK 					0x00F0
#define NN_NIBBLES_MASK 				0x00FF
#define FIRST_AND_LAST_NIBBLES_MASK 	0xF00F
#define FIRST_THIRD_LAST_NIBBLES_MASK 	0xF0FF
#define LAST_NIBBLE_MASK 				0x000F

