#pragma once

#include <array>

#include "board.hpp"

namespace attack {

	const std::array<int, 8> knight_dir = { -8, -19, -21, -12, 8, 19, 21, 12 };
	const std::array<int, 4> rook_dir = { -1, -10, 1, 10 };
	const std::array<int, 4> bishop_dir = { -9, -11, 11, 9 };
	const std::array<int, 8> king_dir = { -1, -10, 1, 10, -9, -11, 11, 9};

	int isSquareAttacked(int square, constants::Color player, board::BoardState& state);

}