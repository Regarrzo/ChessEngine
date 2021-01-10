#pragma once

#include <string>
#include <stdexcept>

#include "constants.hpp"
#include "util.hpp"
#include "board.hpp"
#include "move.hpp"


namespace movegen {

	std::vector<move::Move> generateAllMoves(board::BoardState& state);
	std::vector<move::Move> generateAllCaptures(board::BoardState& state);
	bool moveExists(board::BoardState& state, const move::Move& move);
}