#include "Config.h"
#include "chip8.h"
#include "display.h"

int main() {

    Chip8 chip8interpreter;
    Display chip8display;

    chip8interpreter.printMemory();
    
    while(chip8display.closeDisplayCheck()) {
        chip8display.renderDisplay(chip8interpreter.getPixels());
    };

    return 0;
}