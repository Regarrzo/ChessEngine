#include "attack.hpp"
#include "constants.hpp"
#include "validate.hpp"

int attack::isSquareAttacked(int square, constants::Color player, board::BoardState& state) {

	assert(validate::is120OnBoard(square));
	assert(validate::isSideValid(asInt(player)));
	assert(state.checkBoard());

	if (player == constants::Color::WHITE) {
		if (state.pieces[square - 11] == asInt(constants::Piece::wP) || state.pieces[square - 9] == asInt(constants::Piece::wP))
			return true;
	}
	else {
		if (state.pieces[square + 11] == asInt(constants::Piece::bP) || state.pieces[square + 9] == asInt(constants::Piece::bP))
			return true;
	}

	for (int move : knight_dir) {
		int piece = state.pieces[square + move];
		if (piece == asInt(constants::Square::OFFBOARD))
			continue;

		if (constants::IS_KNIGHT[piece] && (constants::PIECE_COLOR[piece] == player))
			return state.pieces[square + move];
	}

	for (int move : rook_dir) {
		int _square = square + move;
		int piece = state.pieces[_square];

		while (piece != asInt(constants::Square::OFFBOARD)) {
			if (piece != asInt(constants::Piece::EMPTY)) {
				if (constants::IS_ROOK_QUEEN[piece] && constants::PIECE_COLOR[piece] == player)
					return true;
				break;
			}

			_square += move;
			piece = state.pieces[_square];
		}
	}

	for (int move : bishop_dir) {
		int _square = square + move;
		int piece = state.pieces[_square];

		while (piece != asInt(constants::Square::OFFBOARD)) {
			if (piece != asInt(constants::Piece::EMPTY)) {
				if (constants::IS_BISHOP_QUEEN[piece] && constants::PIECE_COLOR[piece] == player)
					return true;
				break;
			}

			_square += move;
			piece = state.pieces[_square];
		}
	}


	for (int move : king_dir) {
		int piece = state.pieces[square + move];

		if (piece == asInt(constants::Square::OFFBOARD))
			continue;

		if (constants::IS_KING[piece] && (constants::PIECE_COLOR[piece] == player))
			return state.pieces[square + move];
	}

	return false;
}