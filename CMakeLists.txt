# Set minimum required version for CMAKE
cmake_minimum_required(VERSION 3.10)

# Setup compiler variables
project(xepa)
set(CMAKE_C_COMPILER /usr/bin/x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/x86_64-w64-mingw32-g++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -mwindows -static-libgcc -static-libstdc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

# Add source files
file(GLOB_RECURSE sourcefiles
    "*.h"
    "*.cc"
)

# Add required libraries
link_directories("/mnt/c/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Lib/x64")

# Build main executable
add_executable(xepa.exe ${sourcefiles})
target_link_libraries(xepa.exe d3d9 dwmapi)