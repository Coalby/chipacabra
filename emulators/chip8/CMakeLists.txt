cmake_minimum_required(VERSION 3.29)
project(Chip8Emulator VERSION 0.1)

find_package(SDL2 REQUIRED)

if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset       "${Esc}[m")
  set(ColourBold        "${Esc}[1m")
  set(PlatformColor     "${Esc}[38;5;130m")
endif()

message(STATUS "${PlatformColor}[INFO] Building for host platform...${ColourReset}")

# Standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Configurations
set(CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Config)
configure_file(${CONFIG_DIR}/Config.h.in ${CONFIG_DIR}/Config.h)

# Set source paths
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${SRC_DIR}/inc)

# Set third_party paths
# TODO: Consider a more modular include path instead of ../..
set(CHIPACABRA_HOME_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)

add_executable(Chip8Emulator
    ${SRC_DIR}/startup.cpp
    ${SRC_DIR}/display.cpp
    ${SRC_DIR}/chip8.cpp
    ${SRC_DIR}/opcode.cpp
    ${SRC_DIR}/rom.cpp
)

target_include_directories(Chip8Emulator PRIVATE
    ${INCLUDE_DIR}
    ${CONFIG_DIR}
    ${SDL2_INCLUDE_DIRS}
)

# TODO: Include if(WIN32) for Windows
target_link_libraries(Chip8Emulator
    ${SDL2_LIBRARIES}
)
