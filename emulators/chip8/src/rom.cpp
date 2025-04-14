#include "rom.h"
#include <fstream>
#include <vector>

char FileRomManager::loadRom(std::string_view filename, Chip8& chip8) {
    char error {};
    
    // Read raw file data
    std::ifstream file(filename.data(), std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return -1;
    }

    // Get file size
    std::streamsize fileSize = file.tellg();
    if (fileSize > static_cast<std::streamsize>(ROM_MEM_SIZE)) {
        return -1; // ROM too large
    }

    // Jump back to start of file
    file.seekg(0, std::ios::beg);

    // Only using vector on desktop due to using dynamic memory
    std::vector<char> buffer(fileSize);
    if (!file.read(buffer.data(), fileSize)) {
        return -1;
    }

    // Write to Chip8 memory space
    if(chip8.writeMemory(buffer.data(), buffer.size(), ROM_MEM_START));
        return -1;

    return error;
};
