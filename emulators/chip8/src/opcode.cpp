#include "opcode.h"
#include "chip8.h"

// Opcode class functionality

// Executes the opcodes
void Opcodes::executeOpcode(unsigned short opcode, Chip8& chip8) {
    // TODO: O(N) lookup -> Not efficient ; Hashmap implementation?
    for(const auto &entry : opcodeLookup) {
        if((opcode & entry.mask) == entry.opcode)
            entry.opcodeHandler(opcode, chip8);
    }

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
    return;
}

// Jumps to address
void Opcodes::opJumpAddr(unsigned short opcode, Chip8& chip8) {
    chip8.setProgramCounter(opcode & 0xFFF);
    return;
}

// Calls subroutine
void Opcodes::opCallSub(unsigned short opcode, Chip8& chip8) {
    return;
}

// Skips next instruction if Vx == (opcode & 0xFF)
void Opcodes::opSEvxByte(unsigned short opcode, Chip8& chip8) {
    if(chip8.getRegisterValue(opcode & 0x0F00) == (opcode && 0xFF))
        chip8.addProgramCounter(2);
    return;
}

// Loads (opcode & 0xFF) into Vx
void Opcodes::opLoadVx(unsigned short opcode, Chip8& chip8) {
    chip8.setRegisterValue(opcode & 0xF00, opcode & 0xFF);
    
    return;
}

// Adds (opcodoe & 0xFF) to Vx
void Opcodes::opAddVx(unsigned short opcode, Chip8& chip8) {
    // Consider removing this char for optimization
    // Decreases readability but will increase speed marginally
    unsigned char registerNumber { static_cast<unsigned char>((opcode & 0xF00) >> 8) };
    chip8.setRegisterValue(registerNumber, chip8.getRegisterValue(registerNumber) + (opcode & 0xFF));
    
    return;
}

// Loads (opcode & 0xFFF) to I
void Opcodes::opLoadI(unsigned short opcode, Chip8& chip8) {
    chip8.setI(opcode & 0xFFF);
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
    
    for (unsigned char row = y; row < spriteHeight; row++) {
        if (row >= pixels::DISPLAY_HEIGHT) break; // Don't draw past screen
        
        unsigned char spriteByte = chip8.memory[spriteAddr + row];
        
        // Draw a byte at a time
        for (unsigned char col = x; col < x + 8; col++) {
            if (x + col >= pixels::DISPLAY_WIDTH) break; // Don't draw past right edge
            
            // Check if current pixel in sprite is set
            bool pixelSet = (spriteByte & (0x80 >> col));
            
            if (pixelSet) {
                // Get current screen pixel
                unsigned int& screenPixel = chip8.pixels[row][col];

                // If pixel was set, then collision has occured (VF = 1)
                if(screenPixel == pixels::WHITE_PIXEL)
                    chip8.setRegisterValue(0xF, 1);
                
                // XOR with the screen
                // TODO: Find a way to utilize bitwise ^= instead
                screenPixel = (screenPixel == pixels::WHITE_PIXEL) ? pixels::BLACK_PIXEL : pixels::WHITE_PIXEL;
            }
        }
    }
}
