#include <cpu.hpp>
#include <conio.h>

void CPU::loadMemory(std::string& fname) {
    std::cout << "do nothing, going to open file as ROM: " << fname  << std::endl;
    
    std::ifstream file(fname, std::ios::binary | std::ios::ate);

    if(file.is_open()){
        std::streampos size = file.tellg();
        char* temp_buffer = new char[size];

        file.seekg(0, std::ios::beg);
        file.read(temp_buffer, size);
        file.close();

        for(int i = 0; i < size; i++) {
            memory[ROM_START_ADDRESS + i] = temp_buffer[i];
            printf("%s: Addr: %02x, Memory contents: 0x%02x, file contents: %x\n", __func__ , (ROM_START_ADDRESS + i), memory[ROM_START_ADDRESS + i],
                temp_buffer[i]);
        }
        pc = ROM_START_ADDRESS;
        delete[] temp_buffer;
        
    }
    else {
        std::cout << "fatal couldn't open ROM" << std::endl;
    }
}

void CPU::initialize(void) {
    // set up the graphics stuff
    // set up the SFML windows
    // set initial values to registers/internal flags
    
    pc = ROM_START_ADDRESS;
    index = INIT_MEMORY;
    stack_pointer = INIT_MEMORY;
    opcode = INIT_MEMORY;
    

    // initialize op code specific bits
    vx_id = INIT_MEMORY;
    vy_id = INIT_MEMORY;
    nn = INIT_MEMORY;
    nnn = INIT_MEMORY;


    for(int i = 0; i < 80; i++) {
        memory[FONT_START_LOCATION + i] = chip8_fontset[i];
    }

    for(int i = 0; i <= 0xf; i++) {
        regV[i] = INIT_MEMORY;
    }

    screen_update = false;
}

void CPU::clear_display(void) {
    memset(&display, 0, 64 * 32);
}

void CPU::extract_vx_nibble(void) {
    vx_id = static_cast<uint8_t>((opcode & VX_NIBBLE_MASK) >> 8u);
}

void CPU::extract_vy_nibble(void) {
    vy_id = static_cast<uint8_t>((opcode & VY_NIBBLE_MASK) >> 4u);
}

void CPU::extract_nn_nibbles(void) {
    nn = static_cast<uint8_t>(opcode & NN_NIBBLES_MASK);
}

void CPU::extract_nnn_nibbles(void) {
    nnn = static_cast<uint16_t>(opcode & NNN_NIBBLES_MASK);
}

void CPU::emulateCycle(void) {
    // printf("%s: entered\n", __func__);

    uint8_t nn;
    uint16_t nnn;
    uint16_t old_vx, old_vy;
    // fetch cycle
    opcode = static_cast<uint16_t>(memory[pc] << 8u) | static_cast<uint16_t>(memory[pc+1]);
    pc +=2;

    std::cout <<  std::hex  << opcode << "<-: " << static_cast<uint16_t>(pc - 2) << ", " 
        << static_cast<uint16_t>(pc - 1) << ", (next time) pc->: " << pc << std::endl;
    
    switch(opcode & FIRST_NIBBLE_MASK) {
        case CHIP8_OPCODE_CALL_ROUTINE:
        {
            if(opcode == CHIP8_OPCODE_CLEAR_SCREEN) {
                std::cout << __func__ << ": Emulating clear screen" << std::endl;
                clear_display();
            }
            else if(opcode == CHIP8_OPCODE_CALL_RETURN) {
                std::cout << __func__ << ": Emulating call return" << std::endl;
                --stack_pointer;
                pc = stack[stack_pointer];
                std::cout << __func__ << ": Returning back to " << std::hex << pc << std::endl;
            }
            else if(opcode == CHIP8_OPCODE_CALL_ROUTINE) {
                uint16_t taddr = opcode & NNN_NIBBLES_MASK;
                std::cout << __func__ << ": Setting PC to " << std::hex << taddr << std::endl;
                
            }
            break;
        }

        case CHIP8_OPCODE_VX_EQUAL_CONST:
        {
            old_vx = static_cast<uint16_t>((opcode & VX_NIBBLE_MASK) >> 8);
            nn = static_cast<uint8_t>(opcode & NN_NIBBLES_MASK);
            
            if(regV[old_vx] == nn) {
                pc += 2; 
                std::cout << __func__ << ": Setting PC to " << std::hex << pc << std::endl;
            }
            else {
                std::cout << __func__ << ": Setting PC to " << std::hex << pc << std::endl;
            }
            break;
        }

        case CHIP8_OPCODE_VX_NOT_EQUAL_CONST:
        {
            old_vx = static_cast<uint16_t>((opcode & VX_NIBBLE_MASK) >> 8);
            nn = static_cast<uint8_t>(opcode & NN_NIBBLES_MASK);
            
            if(regV[old_vx] != nn) {
                pc += 2;
                std::cout << __func__ << ": Setting PC to " << std::hex << pc << std::endl;
            }
            else {
                std::cout << __func__ << ": Setting PC to " << std::hex << pc << std::endl;
            }
            break;
        }

        case CHIP8_OPCODE_VX_EQUAL_VY:
        {
            old_vx = static_cast<uint16_t>((opcode & VX_NIBBLE_MASK) >> 8);
            old_vy = static_cast<uint16_t>((opcode & VY_NIBBLE_MASK) >> 4);
            
            if(regV[old_vx] == regV[old_vy]) {
                pc += 2;
                std::cout << __func__ << ": Setting PC (skip) to " << std::hex << pc << std::endl;
            }
            else {
                std::cout << __func__ << ": Setting PC to " << std::hex << pc << std::endl;
            }
            break;
        }
        case CHIP8_OPCODE_SET_REG_INDEX_TO_ADDR:
            index = static_cast<uint16_t>(opcode & NNN_NIBBLES_MASK);
            std::cout << __func__ << ": emulating setting index register, index register is now: " << std::hex << index << std::endl;
            
            break;
        
        case CHIP8_OPCODE_SET_VX_TO_CONST:
        {
            old_vx = static_cast<uint16_t>((opcode & VX_NIBBLE_MASK) >> 8);
            if(old_vx >= 16) {
                std::cout << __func__ << ": Illegal Vx provided " << old_vx << std::endl;
                exit(1);
            }
            else {
                regV[old_vx] = static_cast<uint8_t>(opcode & NN_NIBBLES_MASK);
            }
            std::cout << __func__ << ": Setting register " << old_vx << " to " << std::hex << static_cast<uint16_t>(opcode & NN_NIBBLES_MASK) << std::endl;
            break;
        }

        case CHIP8_OPCODE_JUMP_TO_ADDR_PLUS_V00:
        {
            nnn = static_cast<uint16_t>(opcode & NNN_NIBBLES_MASK);
            pc = regV[0] + nnn;
            std::cout << __func__ << ": Setting PC to " << std::hex << pc << std::endl;
            break;
        }

        case CHIP8_OPCODE_SET_VX_TO_RAND_AND_NN:
        {
            old_vx = static_cast<uint16_t>((opcode & VX_NIBBLE_MASK) >> 8);
            nn = static_cast<uint8_t>(opcode & NN_NIBBLES_MASK);
            regV[old_vx] = rand() % nn;
            std::cout << __func__ << ": Setting reg[" << std::dec << old_vx << "]: to: " << std::hex << static_cast<uint16_t>(regV[old_vx]) << std::endl;
            break;
        }
        case CHIP8_PARTIAL_OPCODE_KEYPAD_PRESSED:
        {
            std::cout << __func__ << "-debug: " << std::hex << opcode << " key code not implemented;" << std::endl;
            if((opcode & 0xF0FFu) == CHIP8_PARTIAL_OPCODE_KEYPAD_PRESSED_IS_VX) {
                std::cout << __func__ << "-debug: " << std::hex << CHIP8_PARTIAL_OPCODE_KEYPAD_PRESSED_IS_VX << " key code not implemented;" << std::endl;
            }
            else if((opcode & 0xF0FFu) == CHIP8_PARTIAL_OPCODE_KEYPAD_PRESSED_IS_NOT_VX) {
                std::cout << __func__ << "-debug: " << std::hex << CHIP8_PARTIAL_OPCODE_KEYPAD_PRESSED_IS_NOT_VX << " key code not implemented;" << std::endl;
            }
            exit(-1);
            break;
        }

        case CHIP8_PARTIAL_OPCODE_MISC_OPCODES:
        {
            old_vx = static_cast<uint16_t>((opcode & VX_NIBBLE_MASK) >> 8);

            if((opcode & 0xF0FFu) == CHIP8_PARTIAL_OPCODE_SET_VX_TO_DELAY) {
                regV[old_vx] = delay_timer;
            }
            else if((opcode & 0xF0FFu) == CHIP8_PARTIAL_OPCODE_SET_VX_TO_KEYPAD) {
                std::cout << __func__ << "-debug: " << std::hex << opcode << " key code not implemented;" << std::endl;
                exit(1);
            }
            else if((opcode & 0xF0FFu) == CHIP8_PARTIAL_OPCODE_SET_DELAY_TIMER_TO_VX) {
                delay_timer = regV[old_vx];
            }
            else if((opcode & 0xF0FFu) == CHIP8_PARTIAL_OPCODE_SET_SOUND_TIMER_TO_VX) {
                sound_timer = regV[old_vx];
            }
            else if((opcode & 0xF0FFu) == CHIP8_PARTIAL_OPCODE_SET_INDX_REG_TO_INDEX_REG_PLUS_VX) {
                index += static_cast<uint16_t>(regV[old_vx]);
            }
            else if((opcode & 0xF0FFu) == CHIP8_PARTIAL_OPCODE_SET_INDX_REG_SPRITE_ADD_OF_VX) {
                uint8_t num = regV[old_vx];
                index = FONT_START_LOCATION + (5 * num);
            }
            else if((opcode & 0xF0FFu) == CHIP8_PARTIAL_OPCODE_BCD_OF_VX) {
                memory[index]     = regV[(opcode & 0x0F00) >> 8] / 100;
                memory[index + 1] = (regV[(opcode & 0x0F00) >> 8] / 10) % 10;
                memory[index + 2] = (regV[(opcode & 0x0F00) >> 8] % 100) % 10;
                std::cout << __func__ << "-debug: " << std::hex << opcode << " key code not implemented;" << std::endl;
             
                // uint8_t num = regV[old_vx];

                // memory[index + 2] = num % 10;
                // num /= 10;

                // memory[index + 1] = num % 10;
                // num /= 10;

                // memory[index] = num % 10;
                                
            }
            else if((opcode & 0xF0FFu) == CHIP8_PARTIAL_OPCODE_DUMP_ALL_REGS_TO_INDX_REG_MEM) {
                uint16_t vx = old_vx;
                for(uint16_t i = 0; i <= vx; ++i) {
                    memory[index + i] = regV[i];
                    
                }
            }
            else if((opcode & 0xF0FFu) == CHIP8_PARTIAL_OPCODE_LOAD_ALL_REGS_FROM_INDEX_REG_MEM) {
                std::cout << __func__ << ": vx ID is " << static_cast<uint16_t>(old_vx) << ", contents are " << static_cast<uint16_t>(regV[old_vx]) << std::endl;
                uint16_t vx = old_vx;
                for(uint16_t i = 0; i <= static_cast<uint16_t>(vx); ++i) {
                    std::cout << __func__ << ": copying from memory[ " << std::hex << static_cast<uint16_t>(index + i) << "] to regV["
                        << std::dec << static_cast<uint16_t>(i)  << "]" << std::endl;
                    std::cout << __func__ << ": contents, reg: " << static_cast<uint16_t>(i) << ", memory: " << static_cast<uint16_t>(memory[index + i])  << std::endl;
                    regV[i] = memory[index + i];
                    
                }
            }
            else {
                ; // do nothing
            }
            break;
        }

        case CHIP8_OPCODE_DISPLAY:
            {
                std::cout << __func__ << ": got call for displaying something: " << std::setfill('0') << std::setw(3) << (opcode & 0x0FFF) << std::endl;          
                old_vx = static_cast<uint16_t>((opcode & VX_NIBBLE_MASK) >> 8);
                old_vy = static_cast<uint16_t>((opcode & VY_NIBBLE_MASK) >> 4);
                
                uint8_t ht;
                ht = static_cast<uint8_t>((opcode & LAST_NIBBLE_MASK));

                std::cout << __func__ << ": old_vx-> " << old_vx << ", old_vy-> " << old_vy << ", ht-> " << static_cast<uint16_t>(ht) <<  std::endl;
                
                uint8_t cordinate_x = regV[old_vx] % 64;
                uint8_t cordinate_y = regV[old_vy] % 32;

                std::cout << __func__ << ": Contents: old_vx-> " << static_cast<uint16_t>(cordinate_x) << ", old_vy-> " << static_cast<uint16_t>(cordinate_y) <<
                    ", index reg is: " << std::hex << index<< std::endl;

                // set carry flag to zero
                regV[0xF] = 0;

                for(uint32_t row = 0; row < ht; row++) {
                    uint8_t spriteData = memory[index + row];
                    for(uint32_t col = 0; col < 8; col++) {
                        uint8_t spritePixel = spriteData & (0x80 >> col);
                        uint32_t pixIndex = (cordinate_y + row) * 64 + (cordinate_x + col);
                        if(spritePixel != 0) {
                            if(display[pixIndex]  == 1) {
                                regV[0xF] = 1;
                            }
                            display[pixIndex] ^= 1;
                            // std::cout << __func__ << "-debug: spritePixel: " << static_cast<uint16_t>(spritePixel)
                                // << ", pixindex: " << std::dec << pixIndex << ", display mem: " << display[pixIndex] << std::endl;
                        }
                    }
                }
                screen_update = true;
            }
            
            
            break;

        case CHIP8_OPCODE_ADD_NN_TO_VX:
        {
            old_vx = static_cast<uint16_t>((opcode & VX_NIBBLE_MASK) >> 8);
            nn = static_cast<uint8_t>(opcode & NN_NIBBLES_MASK);
            std::cout << __func__ << ": Before old_vx-> " << old_vx << ", Reg contents: " << static_cast<uint16_t>(regV[old_vx]) << ", nn-> " << static_cast<uint16_t>(nn) <<  std::endl;
            regV[old_vx] += nn;
            std::cout << __func__ << ": After old_vx-> " << old_vx << ", Reg contents: " << static_cast<uint16_t>(regV[old_vx]) << ", nn-> " << static_cast<uint16_t>(nn) <<  std::endl;
            
        
            break;
        }

        case CHIP8_OPCODE_JUMP_TO_ADDR:
        {
            nnn = static_cast<uint16_t>(opcode & NNN_NIBBLES_MASK);
            std::cout << __func__ << ": Jump provided address: " << std::hex << nnn << std::endl;
            if(nnn < 0x1000) {
                pc = nnn;
            }
            else {
                std::cout << __func__ << ": fatal, trying to jump to addresss outside memory: 0x" << std::hex<< nnn << std::endl;
                exit(-1);
            }
            break;
        }
        
        case CHIP8_OPCODE_CALL_SUBROUTINE:
        {
            std::cout << __func__ << ": Saving PC to stack " << std::hex << static_cast<uint16_t>(pc) << std::endl;
            nnn = static_cast<uint16_t>(opcode & NNN_NIBBLES_MASK);
            stack[stack_pointer] = pc;
            ++stack_pointer;
            pc = nnn; 
            std::cout << __func__ << ": Jumping to nnn " << std::hex << static_cast<uint16_t>(pc) << std::endl;
            break;
        }


        case CHIP8_PARTIAL_OPCODE_ASSIGN_VX_TO_VY:
        {
            old_vx = static_cast<uint16_t>((opcode & VX_NIBBLE_MASK) >> 8);
            old_vy = static_cast<uint16_t>((opcode & VY_NIBBLE_MASK) >> 4);
            
            if((opcode & 0xF00F) == CHIP8_PARTIAL_OPCODE_ASSIGN_VX_TO_VY) {
                regV[old_vx] = regV[old_vy];
            }
            else if((opcode & 0xF00F) == CHIP8_PARTIAL_OPCODE_SET_VX_TO_VY_OR_VX) {
                regV[old_vx] = regV[old_vx] | regV[old_vy];
            }
            else if((opcode & 0xF00F) == CHIP8_PARTIAL_OPCODE_SET_VX_TO_VY_AND_VX) {
                regV[old_vx] = regV[old_vx] & regV[old_vy];
            }
            else if((opcode & 0xF00F) == CHIP8_PARTIAL_OPCODE_SET_VX_TO_VY_XOR_VX) {
                regV[old_vx] = regV[old_vx] ^ regV[old_vy];
            }
            else if((opcode & 0xF00F) == CHIP8_PARTIAL_OPCODE_SET_VX_TO_VY_PLUS_VX) {
                if(regV[old_vx] + regV[old_vy] > 0xffu) {
                    regV[old_vx] = regV[old_vx] + regV[old_vy];
                    regV[0xf] = 1u; 
                }
                else {
                    regV[old_vx] = regV[old_vx] + regV[old_vy];
                    regV[0xf] = 0u;
                }
            }
            else if((opcode & 0xF00F) == CHIP8_PARTIAL_OPCODE_SET_VX_TO_VX_MINUS_VY) {
                if(regV[old_vx] > regV[old_vy]) {
                    regV[0xf] = 1u;
                }
                else {
                    regV[0xf] = 0u;   
                }
                regV[old_vx] -= regV[old_vy];
            }
            else if((opcode & 0xF00F) == CHIP8_PARTIAL_OPCODE_SET_RIGHT_SHIFT_VX) {
                regV[0xf] = regV[old_vx] & 0x1u; 
                
                regV[old_vx] = regV[old_vx] >> 1;
            }
            else if((opcode & 0xF00F) == CHIP8_PARTIAL_OPCODE_SET_VX_TO_VY_MINUS_VX) {
                if(regV[old_vy] > regV[old_vx]) {
                    regV[0xf] = 1u;
                }
                else {
                    regV[0xf] = 0u;   
                }
                regV[old_vx] = regV[old_vy] - regV[old_vx];
            }
            else if((opcode & 0xF00F) == CHIP8_PARTIAL_OPCODE_SET_LEFT_SHIFT_VX) {
                regV[0xf] = (regV[old_vx] & 0x80u) >> 7u;
                regV[old_vx] = regV[old_vx] << 1;
            }
            else {
                ; // do nothing
            }
            break;
        }

        case CHIP8_OPCODE_SKIP_NEXT_IF_VX_NOT_EQUAL_TO_VY:
        {
            old_vx = static_cast<uint16_t>((opcode & VX_NIBBLE_MASK) >> 8);
            old_vy = static_cast<uint16_t>((opcode & VY_NIBBLE_MASK) >> 4);

            if(regV[old_vx] != regV[old_vy]) {
                pc += 2;
            }
        
            break;
        }
        default:
            std::cout << __func__ << ": unsupport instruction provided, halting -> " << std::hex << opcode << std::endl;
            exit(-1);
            break;
        
    }
    if (delay_timer > 0)
    {
        --delay_timer;
    }
    
    if (sound_timer > 0)
    {
        --sound_timer;
    }
    // printf("%s: exit\n", __func__);
}