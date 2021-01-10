# CMakeList.txt : CMake project for ChessEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options("/std:c++latest")
add_executable(chessengine main.cpp board.cpp "test.hpp" "attack.hpp" "attack.cpp" "move.hpp"  "__move.txt" "validate.hpp" "movegen.hpp" "movegen.cpp" "perft.hpp" "pvtable.hpp" "search.hpp" "search.cpp" "evaluate.hpp" "uci.hpp")
add_compile_definitions(USE_ASM)

# TODO: Add tests and install targets if needed.
