
#pragma once

#include "constants.hpp"
#include "util.hpp"


namespace validate {
	constexpr bool isFileRankValid(int file_rank) {
		return file_rank >= 0 && file_rank <= 7;
	}

	constexpr bool is120OnBoard(int square) {
		int rank = util::_120ToRow(square);
		int file = util::_120ToCol(square);
		return isFileRankValid(rank) && isFileRankValid(file);
	}

	constexpr bool isSideValid(int side) {
		return side == asInt(constants::Color::WHITE) || side == asInt(constants::Color::BLACK);
	}



	constexpr bool isPieceValid(int piece) {
		return piece >= asInt(constants::Piece::EMPTY) && piece <= asInt(constants::Piece::bK);
	}

	constexpr bool isPieceValidNotEmpty(int piece) {
		return piece >= asInt(constants::Piece::wP) && piece <= asInt(constants::Piece::bK);
	}


}