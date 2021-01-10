#pragma once

#include <iostream>
#include "board.hpp"
#include "constants.hpp"
#include "util.hpp"

namespace evaluate {
	inline int evaluatePosition(const board::BoardState& state) {
		int score = state.material[asInt(constants::Color::WHITE)] - state.material[asInt(constants::Color::BLACK)];
		// pawns

		for (int piece_square : state.piece_list[asInt(constants::Piece::wP)]){
			score += constants::PAWN_VALUE_TABLE[util::_120To64(piece_square)];
		}

		for (int piece_square : state.piece_list[asInt(constants::Piece::bP)]) {
			score -= constants::PAWN_VALUE_TABLE[constants::MIRROR_SQUARE[util::_120To64(piece_square)]];
		}

		// knights

		for (int piece_square : state.piece_list[asInt(constants::Piece::wN)]) {
			score += constants::KNIGHT_VALUE_TABLE[util::_120To64(piece_square)];
		}

		for (int piece_square : state.piece_list[asInt(constants::Piece::bN)]) {
			score -= constants::KNIGHT_VALUE_TABLE[constants::MIRROR_SQUARE[util::_120To64(piece_square)]];
		}

		// bishops

		for (int piece_square : state.piece_list[asInt(constants::Piece::wB)]) {
			score += constants::BISHOP_VALUE_TABLE[util::_120To64(piece_square)];
		}

		for (int piece_square : state.piece_list[asInt(constants::Piece::bB)]) {
			score -= constants::BISHOP_VALUE_TABLE[constants::MIRROR_SQUARE[util::_120To64(piece_square)]];
		}

		// rooks

		for (int piece_square : state.piece_list[asInt(constants::Piece::wR)]) {
			score += constants::ROOK_VALUE_TABLE[util::_120To64(piece_square)];
		}

		for (int piece_square : state.piece_list[asInt(constants::Piece::bR)]) {
			score -= constants::ROOK_VALUE_TABLE[constants::MIRROR_SQUARE[util::_120To64(piece_square)]];
		}
		
		return state.player == constants::Color::WHITE ? score : -score;
	}
}