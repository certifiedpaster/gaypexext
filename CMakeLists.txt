# Build on Linux or using Linux subsystem for Windows
# /mnt/c/Users/q/Desktop/amlegit/projects/xepa/build

# Set minimum required version for CMAKE
cmake_minimum_required(VERSION 3.10)

# Setup compiler variables
project(xepa)
set(CMAKE_C_COMPILER /usr/bin/x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/x86_64-w64-mingw32-g++)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows -static-libgcc -static-libstdc++ -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -mwindows -static-libgcc -static-libstdc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_CXX_STANDARD 11)

# Add source files
file(GLOB_RECURSE sourcefiles
    "*.h"
    "*.cc"
)

# Add required libraries
link_directories("/mnt/c/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Lib/x64")
link_directories("/mnt/c/Program Files/VMProtect Ultimate/Lib/Windows")

# Build main executable
add_executable(xepa.exe ${sourcefiles})
target_link_libraries(xepa.exe d3d9 dwmapi ntdll winmm ws2_32 VMProtectSDK64)