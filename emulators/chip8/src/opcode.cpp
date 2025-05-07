#include <cmath>
#include "opcode.h"
#include "chip8.h"

#define GET_VX_FROM_OP(opcode)      ((opcode & 0xF00) >> 8)
#define GET_VY_FROM_OP(opcode)      ((opcode & 0x00F0) >> 4)

// Opcode class functionality

// Executes the opcodes
void Opcodes::executeOpcode(unsigned short opcode, Chip8& chip8) {
    // TODO: O(N) lookup -> Not efficient ; Hashmap implementation?
    for(const auto &entry : opcodeLookup) {
        if((opcode & entry.mask) == entry.opcode)
        {
            chip8.addProgramCounter(2);
            entry.opcodeHandler(opcode, chip8);
            break;
        }
    }

    return;
}

// Clears the screen
void Opcodes::opClearScreen(unsigned short opcode, Chip8& chip8) {

    for (size_t height_index = 0; height_index < pixels::DISPLAY_HEIGHT; height_index++) {
        for (size_t width_index = 0; width_index < pixels::DISPLAY_WIDTH; width_index++) {
            chip8.pixels[height_index][width_index] = pixels::BLACK_PIXEL;
        }
    }

    return;
}

// Returns from subroutine
void Opcodes::opReturnFromSub(unsigned short opcode, Chip8& chip8) {
    chip8.setProgramCounter(chip8.stack[chip8.SP-1]);
    chip8.SP--;

    return;
}

// This opcode is kinda wack. Not sure how this would affect stack usage
void Opcodes::opCallMchnCode(unsigned short opcode, Chip8& chip8) {
    unsigned short addr {};
    unsigned short nextOpcode {};
    addr = (opcode & 0xFFF);
    nextOpcode = chip8.getMachineCode(addr);

    // TODO: Fix this being called as a public function
    //       Could result in corrupt data?
    executeOpcode(nextOpcode, chip8);

    return;
}

// Jumps to address
void Opcodes::opJumpAddr(unsigned short opcode, Chip8& chip8) {
    chip8.setProgramCounter(opcode & 0xFFF);

    return;
}

// Calls subroutine
void Opcodes::opCallSub(unsigned short opcode, Chip8& chip8) {
    chip8.stack[chip8.SP] = chip8.PC;
    chip8.SP++;
    chip8.setProgramCounter(opcode & 0xFFF);

    return;
}

// Skips next instruction if Vx == NN
void Opcodes::opSEVx(unsigned short opcode, Chip8& chip8) {
    if(chip8.getRegisterValue((opcode & 0x0F00) >> 8) == (opcode & 0xFF))
        chip8.addProgramCounter(2);
    
    return;
}

// Skips next instruction if Vx != NN
void Opcodes::opSNEVx(unsigned short opcode, Chip8& chip8) {
    if(chip8.getRegisterValue((opcode & 0x0F00) >> 8) != (opcode & 0xFF))
        chip8.addProgramCounter(2);

    return;
}

// Skips next instruction if Vx == Vy
void Opcodes::opSEVxVy(unsigned short opcode, Chip8& chip8) {
    if(chip8.getRegisterValue((opcode & 0x0F00) >> 8) == chip8.getRegisterValue(GET_VY_FROM_OP(opcode)))
        chip8.addProgramCounter(2);

    return;
}

// Loads NN into Vx
void Opcodes::opLoadVx(unsigned short opcode, Chip8& chip8) {
    chip8.setRegisterValue(GET_VX_FROM_OP(opcode), opcode & 0xFF);
    
    return;
}

// Adds (opcodoe & 0xFF) to Vx
void Opcodes::opAddVx(unsigned short opcode, Chip8& chip8) {
    // Consider removing this char for optimization
    // Decreases readability but will marginally increase speed
    unsigned char registerNumber { static_cast<unsigned char>(GET_VX_FROM_OP(opcode)) };
    chip8.setRegisterValue(registerNumber, chip8.getRegisterValue(registerNumber) + (opcode & 0xFF));
    
    return;
}

// Loads Vx to Vy
void Opcodes::opLoadVxVy(unsigned short opcode, Chip8& chip8) {
    chip8.setRegisterValue(GET_VX_FROM_OP(opcode), chip8.getRegisterValue(GET_VY_FROM_OP(opcode)));
    
    return;
}

// Loads Vx to Vx |= Vy
void Opcodes::opLoadORVxVy(unsigned short opcode, Chip8& chip8) {
    chip8.setRegisterValue(GET_VX_FROM_OP(opcode), 
                            chip8.getRegisterValue(GET_VX_FROM_OP(opcode)) | chip8.getRegisterValue(GET_VY_FROM_OP(opcode)));
    
    return;
}

// Loads Vx to Vx &= Vy
void Opcodes::opLoadANDVxVy(unsigned short opcode, Chip8& chip8) {
    chip8.setRegisterValue(GET_VX_FROM_OP(opcode), 
                            chip8.getRegisterValue(GET_VX_FROM_OP(opcode)) & chip8.getRegisterValue(GET_VY_FROM_OP(opcode)));

    return;
}

// Loads Vx to Vx ^= Vy
void Opcodes::opLoadXORVxVy(unsigned short opcode, Chip8&chip8) {
    chip8.setRegisterValue(GET_VX_FROM_OP(opcode), 
                            chip8.getRegisterValue(GET_VX_FROM_OP(opcode)) ^ chip8.getRegisterValue(GET_VY_FROM_OP(opcode)));
    return;
}

// Loads Vx to Vx += Vy. Vf = 1 if an overflow is detected, 0 otherwise
// TODO: Ignore addition when overflow? or not?
void Opcodes::opLoadADDVxVy(unsigned short opcode, Chip8& chip8) {
    // Not the most memory efficient if using continually on embedded system
    int vxValue = chip8.getRegisterValue(GET_VX_FROM_OP(opcode));
    int vyValue = chip8.getRegisterValue(GET_VY_FROM_OP(opcode));
    int registerValue = static_cast<int>(vxValue + vyValue);

    chip8.setRegisterValue(GET_VX_FROM_OP(opcode), static_cast<unsigned char>(registerValue));

    chip8.setOverflowRegister(registerValue > 255);
    
    return;
}

// Loads Vx to Vx -= Vy. Vf = 0 if an underflow is detected, 1 otherwise
void Opcodes::opLoadSUBVxVy(unsigned short opcode, Chip8& chip8) {
    // Not the most memory efficient if using continually on embedded system
    unsigned char vxValue = chip8.getRegisterValue(GET_VX_FROM_OP(opcode));
    unsigned char vyValue = chip8.getRegisterValue(GET_VY_FROM_OP(opcode));
    unsigned char registerValue = static_cast<unsigned char>(vxValue - vyValue);

    chip8.setRegisterValue(GET_VX_FROM_OP(opcode), registerValue);

    chip8.setOverflowRegister(vyValue <= vxValue);

    return;
}

// Shifts Vx right by one. Stores the least significant bit prior to shift in Vf
void Opcodes::opLoadShiftRightVx(unsigned short opcode, Chip8& chip8) {
    unsigned char registerValue = chip8.getRegisterValue(GET_VX_FROM_OP(opcode));

    chip8.setRegisterValue(GET_VX_FROM_OP(opcode), registerValue >> 1);
    chip8.setOverflowRegister(registerValue & 0x1); // Store least significant bit

    return;
}

// Loads Vx to Vy - Vx. Vf = 0 if an underflow is detected, 1 otherwise
void Opcodes::opLoadSUBVyVx(unsigned short opcode, Chip8& chip8) {
    // Not the most memory efficient if using continually on embedded system
    unsigned char vxValue = chip8.getRegisterValue(GET_VX_FROM_OP(opcode));
    unsigned char vyValue = chip8.getRegisterValue(GET_VY_FROM_OP(opcode));
    unsigned char registerValue = static_cast<unsigned char>(vyValue - vxValue);

    chip8.setRegisterValue(GET_VX_FROM_OP(opcode), registerValue);

    chip8.setOverflowRegister(vyValue >= vxValue);

    return;
}

// Shifts Vx left by one. Stores the most significant bit prior to shift in Vf
void Opcodes::opLoadShiftLeftVx(unsigned short opcode, Chip8& chip8) {
    unsigned char registerValue = chip8.getRegisterValue(GET_VX_FROM_OP(opcode));

    chip8.setRegisterValue(GET_VX_FROM_OP(opcode), static_cast<char>(registerValue << 1));
    chip8.setOverflowRegister((registerValue & 0x80) >> 7); // Store most significant bit
    
    return;
}

// Skips next instruction if Vx != Vy
void Opcodes::opSNEVxVy(unsigned short opcode, Chip8& chip8) {
    if(chip8.getRegisterValue(GET_VX_FROM_OP(opcode)) != chip8.getRegisterValue(GET_VY_FROM_OP(opcode)))
        chip8.addProgramCounter(2);

    return;
}

// Loads (opcode & 0xFFF) to I
void Opcodes::opLoadI(unsigned short opcode, Chip8& chip8) {
    chip8.setI(opcode & 0xFFF);
    return;
}

void Opcodes::opJumpAddrV0(unsigned short opcode, Chip8& chip8) {
    return;
}

void Opcodes::opLoadVxRand(unsigned short opcode, Chip8& chip8) {
    return;
}

// Draws a sprite at (Vx,Vy) that is N pixels tall
// TODO: Optimize this, it sucks lol
void Opcodes::opDrawSprite(unsigned short opcode, Chip8& chip8) {
    // Extract X and Y coordinates from registers
    unsigned char x { static_cast<unsigned char>(chip8.getRegisterValue((opcode & 0x0F00) >> 8) % pixels::DISPLAY_WIDTH) };
    unsigned char y { static_cast<unsigned char>(chip8.getRegisterValue((opcode & 0x00F0) >> 4) % pixels::DISPLAY_HEIGHT) };
    unsigned char spriteHeight { static_cast<unsigned char>(opcode & 0xF) };
    unsigned short spriteAddr { chip8.I };
    
    // Reset collision flag (VF)
    chip8.setRegisterValue(0xF, 0);
    
    for (unsigned char yOffset = 0; yOffset < spriteHeight; yOffset++) {
        if (y + yOffset >= pixels::DISPLAY_HEIGHT) break; // Don't draw past screen
        
        unsigned char spriteByte = chip8.memory[spriteAddr];
        
        // Draw a byte at a time
        for (unsigned char xOffset = 0; xOffset < 8; xOffset++) {
            if (x + xOffset >= pixels::DISPLAY_WIDTH) break; // Don't draw past right edge
            
            unsigned int& screenPixel = chip8.pixels[y + yOffset][x + xOffset];

            bool pixelSet = (spriteByte & (0x80 >> xOffset));

            // Only mess with pixels that are 1 in the sprite
            if(pixelSet)
            {
                // If pixel was set, then collision has occured (VF = 1)
                if(screenPixel == pixels::WHITE_PIXEL)
                    chip8.setRegisterValue(0xF, 1);
                
                // XOR with the screen
                // TODO: Find a way to utilize bitwise ^= instead
                screenPixel = (screenPixel == pixels::WHITE_PIXEL) ? pixels::BLACK_PIXEL : pixels::WHITE_PIXEL;
            }
        }

        spriteAddr++;
    }
}

// Skips next instruction if key in Vx is pressed
void Opcodes::opSEKey(unsigned short opcode, Chip8& chip8) {
    if((chip8.v[GET_VX_FROM_OP(opcode)] & 0xF) == chip8.key_pressed)
        chip8.addProgramCounter(2);

    return;
}

// Skips next instruction if key in Vx is not pressed
void Opcodes::opSNEKey(unsigned short opcode, Chip8& chip8) {
    if((chip8.v[GET_VX_FROM_OP(opcode)] & 0xF) != chip8.key_pressed)
        chip8.addProgramCounter(2);
    
    return;
}

// Loads Vx to value of delay timer
void Opcodes::opLoadVxDelay(unsigned short opcode, Chip8& chip8) {
    chip8.setRegisterValue(GET_VX_FROM_OP(opcode), chip8.delay_timer);
    
    return;
}

void Opcodes::opLoadVxKey(unsigned short opcode, Chip8& chip8) {
    return;
}

// Sets delay timer to Vx
void Opcodes::opLoadDelayToVx(unsigned short opcode, Chip8& chip8) {
    chip8.delay_timer = chip8.v[GET_VX_FROM_OP(opcode)];
    return;
}

// Set sound timer to Vx
void Opcodes::opLoadSoundToVx(unsigned short opcode, Chip8& chip8) {
    chip8.sound_timer = chip8.v[GET_VX_FROM_OP(opcode)];
    return;
}

// Loads I to I += Vx
void Opcodes::opLoadIVx(unsigned short opcode, Chip8& chip8) {
    chip8.setI(chip8.I + chip8.getRegisterValue(GET_VX_FROM_OP(opcode)));

    return;
}

void Opcodes::opLoadISpriteAddr(unsigned short opcode, Chip8& chip8) {
    return;
}

// Stores the binary-coded decimal of Vx in memory
void Opcodes::opLoadBCDVx(unsigned short opcode, Chip8& chip8) {
    unsigned char registerValue = chip8.getRegisterValue(GET_VX_FROM_OP(opcode));

    chip8.memory[chip8.I] = (registerValue / 100) % 10;
    chip8.memory[chip8.I + 1] = (registerValue / 10) % 10;
    chip8.memory[chip8.I + 2] = registerValue % 10;

    return;
}

// Stores registers in memory starting at I 
void Opcodes::opStoreRegisterValues(unsigned short opcode, Chip8& chip8) {
    unsigned char maxRegister = GET_VX_FROM_OP(opcode);

    for (unsigned char index = 0; index <= maxRegister; ++index) {
        chip8.memory[chip8.I + index] = chip8.getRegisterValue(index);
    }

    return;
}

// Load registers with memory starting at I
void Opcodes::opLoadRegisterValues(unsigned short opcode, Chip8& chip8) {
    unsigned char maxRegister = GET_VX_FROM_OP(opcode);

    for (unsigned char index = 0; index <= maxRegister; ++index) {
        chip8.setRegisterValue(index, chip8.memory[chip8.I + index]);
    }

    return;
}
