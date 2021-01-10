#include <vector>
#include <array>
#include <iostream>

#include "board.hpp"
#include "move.hpp"
#include "movegen.hpp"
#include "constants.hpp"
#include "validate.hpp"
#include "util.hpp"
#include "attack.hpp"



constexpr std::array<constants::Piece, 3> WHITE_SLIDING_PIECES = { constants::Piece::wB,
																   constants::Piece::wR,
																   constants::Piece::wQ };

constexpr std::array<constants::Piece, 3> BLACK_SLIDING_PIECES = { constants::Piece::bB,
																   constants::Piece::bR,
																   constants::Piece::bQ };

constexpr std::array<constants::Piece, 3> WHITE_NON_SLIDING_PIECES = { constants::Piece::wN,
																	   constants::Piece::wK };

constexpr std::array<constants::Piece, 3> BLACK_NON_SLIDING_PIECES = { constants::Piece::bN,
																	   constants::Piece::bK };


static void addWhitePawnCaptureMove(int from, int to, int capture, std::vector<move::Move>& list, board::BoardState state) {

	assert(validate::isPieceValid(capture));
	assert(validate::is120OnBoard(from));
	assert(validate::is120OnBoard(to));
	// 1 million added for ordering, captures have highest order
	int priority = state.move_ordering_scores[asInt(state.pieces[to])][asInt(state.pieces[from])] + 1000000;

	if (util::_120ToRow(from) == asInt(constants::Rank::_7)) {
		list.push_back(move::Move(from, to, capture, false, false, asInt(constants::Piece::wQ), false, priority));
		list.push_back(move::Move(from, to, capture, false, false, asInt(constants::Piece::wR), false, priority));
		list.push_back(move::Move(from, to, capture, false, false, asInt(constants::Piece::wB), false, priority));
		list.push_back(move::Move(from, to, capture, false, false, asInt(constants::Piece::wN), false, priority));
	}
	else {
		list.push_back(move::Move(from, to, capture, false, false, 0, false, priority));
	}
}

static void addWhitePawnMove(int from, int to, std::vector<move::Move>& list, board::BoardState& state) {

	assert(validate::is120OnBoard(from));
	assert(validate::is120OnBoard(to));

	if (util::_120ToRow(from) == asInt(constants::Rank::_7)) {
		list.push_back(move::Move(from, to, 0, false, false, asInt(constants::Piece::wQ), false, 0));
		list.push_back(move::Move(from, to, 0, false, false, asInt(constants::Piece::wR), false, 0));
		list.push_back(move::Move(from, to, 0, false, false, asInt(constants::Piece::wB), false, 0));
		list.push_back(move::Move(from, to, 0, false, false, asInt(constants::Piece::wN), false, 0));
	}
	else {
		list.push_back(move::Move(from, to, 0, false, false, 0, false, 0));
	}

	int priority = 0;

	if (state.search_killers[0][state.ply] == list.back())
		priority = 900000;
	else if (state.search_killers[1][state.ply] == list.back())
		priority = 800000;
	else
		priority = state.search_history[state.pieces[from]][to];

	list.back().score = priority;
}

static void addBlackPawnCaptureMove(int from, int to, int capture, std::vector<move::Move>& list, board::BoardState& state) {

	assert(validate::isPieceValid(capture));
	assert(validate::is120OnBoard(from));
	assert(validate::is120OnBoard(to));

	int priority = state.move_ordering_scores[asInt(state.pieces[to])][asInt(state.pieces[from])] + 1000000;


	if (util::_120ToRow(from) == asInt(constants::Rank::_2)) {
		list.push_back(move::Move(from, to, capture, false, false, asInt(constants::Piece::bQ), false, priority));
		list.push_back(move::Move(from, to, capture, false, false, asInt(constants::Piece::bR), false, priority));
		list.push_back(move::Move(from, to, capture, false, false, asInt(constants::Piece::bB), false, priority));
		list.push_back(move::Move(from, to, capture, false, false, asInt(constants::Piece::bN), false, priority));
	}
	else {
		list.push_back(move::Move(from, to, capture, false, false, 0, false, priority));
	}
}

static void addBlackPawnMove(int from, int to, std::vector<move::Move>& list, board::BoardState& state) {
	assert(validate::is120OnBoard(from));
	assert(validate::is120OnBoard(to));


	if (util::_120ToRow(from) == asInt(constants::Rank::_2)) {
		list.push_back(move::Move(from, to, 0, false, false, asInt(constants::Piece::bQ), false, 0));
		list.push_back(move::Move(from, to, 0, false, false, asInt(constants::Piece::bR), false, 0));
		list.push_back(move::Move(from, to, 0, false, false, asInt(constants::Piece::bB), false, 0));
		list.push_back(move::Move(from, to, 0, false, false, asInt(constants::Piece::bN), false, 0));
	}
	else {
		list.push_back(move::Move(from, to, 0, false, false, 0, false, 0));
	}

	int priority = 0;

	if (state.search_killers[0][state.ply] == list.back())
		priority = 900000;
	else if (state.search_killers[1][state.ply] == list.back())
		priority = 800000;
	else
		priority = state.search_history[state.pieces[from]][to];

	list.back().score = priority;
}

static void addQuietMove(int from, int to, std::vector<move::Move>& list, board::BoardState& state) {
	assert(validate::is120OnBoard(from));
	assert(validate::is120OnBoard(to));

	int priority = 0;
	list.push_back(move::Move(from, to, 0, false, false, 0, false, 0));

	if (state.search_killers[0][state.ply] == list.back())
		priority = 900000;
	else if (state.search_killers[1][state.ply] == list.back())
		priority = 800000;
	else
		priority = state.search_history[state.pieces[from]][to];;

	list.back().score = priority;
}

static void addCaptureMove(int from, int to, int capture, std::vector<move::Move>& list, board::BoardState& state) {
	assert(validate::is120OnBoard(from));
	assert(validate::is120OnBoard(to));

	int priority = state.move_ordering_scores[asInt(state.pieces[to])][asInt(state.pieces[from])] + 1000000;
	list.push_back(move::Move(from, to, capture, false, false, 0, false, priority));
}

static void addCastlingMove(int from, int to, std::vector<move::Move>& list) {
	list.push_back(move::Move(from, to, 0, false, false, 0, true, 0));
}


std::vector<move::Move> movegen::generateAllMoves(board::BoardState& state) {
	std::vector<move::Move> result;

	if (state.player == constants::Color::WHITE) {
		// Castling

		if (state.castle_permissions & asInt(constants::Castle::wK)) {
			if (state.pieces[asInt(constants::Square::F1)] == asInt(constants::Piece::EMPTY) &&
				state.pieces[asInt(constants::Square::G1)] == asInt(constants::Piece::EMPTY)) {
				if (!attack::isSquareAttacked(asInt(constants::Square::E1), constants::Color::BLACK, state) &&
					!attack::isSquareAttacked(asInt(constants::Square::F1), constants::Color::BLACK, state)) {
					addCastlingMove(asInt(constants::Square::E1), asInt(constants::Square::G1), result);
				}
			}
		}

		if (state.castle_permissions & asInt(constants::Castle::wQ)) {
			if (state.pieces[asInt(constants::Square::D1)] == asInt(constants::Piece::EMPTY) &&
				state.pieces[asInt(constants::Square::C1)] == asInt(constants::Piece::EMPTY) &&
				state.pieces[asInt(constants::Square::B1)] == asInt(constants::Piece::EMPTY)) {
				if (!attack::isSquareAttacked(asInt(constants::Square::E1), constants::Color::BLACK, state) &&
					!attack::isSquareAttacked(asInt(constants::Square::D1), constants::Color::BLACK, state)) {
					addCastlingMove(asInt(constants::Square::E1), asInt(constants::Square::C1), result);
				}
			}
		}


		// Pawns
		for (int wp_square : state.piece_list[asInt(constants::Piece::wP)]) {
			assert(validate::is120OnBoard(wp_square));

			if (state.pieces[wp_square + constants::DIR_UP] == asInt(constants::Piece::EMPTY)) {
				addWhitePawnMove(wp_square, wp_square + constants::DIR_UP, result, state);

				if (util::_120ToRow(wp_square) == asInt(constants::Rank::_2) &&
					state.pieces[wp_square + 2 * constants::DIR_UP] == asInt(constants::Piece::EMPTY)) {
					result.push_back(move::Move(wp_square, wp_square + 2 * constants::DIR_UP, false, false, true, 0, false, 0));
				}
			}

			if (validate::is120OnBoard(wp_square + constants::DIR_UP_LEFT) &&
				constants::PIECE_COLOR[state.pieces[wp_square + constants::DIR_UP_LEFT]] == constants::Color::BLACK)
				addWhitePawnCaptureMove(wp_square, wp_square + constants::DIR_UP_LEFT, 
					state.pieces[wp_square + constants::DIR_UP_LEFT], result, state);

			if (validate::is120OnBoard(wp_square + constants::DIR_UP_RIGHT) &&
				constants::PIECE_COLOR[state.pieces[wp_square + constants::DIR_UP_RIGHT]] == constants::Color::BLACK)
				addWhitePawnCaptureMove(wp_square, wp_square + constants::DIR_UP_RIGHT, 
					state.pieces[wp_square + constants::DIR_UP_RIGHT], result, state);

			if (state.en_passant != asInt(constants::Square::OFFBOARD)) {
				if (wp_square + constants::DIR_UP_LEFT == state.en_passant)
					result.push_back(move::Move(wp_square, wp_square + constants::DIR_UP_LEFT, 0, true, false, 0, false, 105));
				else if (wp_square + constants::DIR_UP_RIGHT == state.en_passant)
					result.push_back(move::Move(wp_square, wp_square + constants::DIR_UP_RIGHT, 0, true, false, 0, false, 105));
			}
		}
	}
	else {
		// Castling


		if (state.castle_permissions & asInt(constants::Castle::bK)) {
			if (state.pieces[asInt(constants::Square::F8)] == asInt(constants::Piece::EMPTY) &&
				state.pieces[asInt(constants::Square::G8)] == asInt(constants::Piece::EMPTY)) {
				if (!attack::isSquareAttacked(asInt(constants::Square::E8), constants::Color::WHITE, state) &&
					!attack::isSquareAttacked(asInt(constants::Square::F8), constants::Color::WHITE, state)) {
					addCastlingMove(asInt(constants::Square::E8), asInt(constants::Square::G8), result);
				}
			}
		}

		if (state.castle_permissions & asInt(constants::Castle::bQ)) {
			if (state.pieces[asInt(constants::Square::D8)] == asInt(constants::Piece::EMPTY) &&
				state.pieces[asInt(constants::Square::C8)] == asInt(constants::Piece::EMPTY) &&
				state.pieces[asInt(constants::Square::B8)] == asInt(constants::Piece::EMPTY)) {
				if (!attack::isSquareAttacked(asInt(constants::Square::E8), constants::Color::WHITE, state) &&
					!attack::isSquareAttacked(asInt(constants::Square::D8), constants::Color::WHITE, state)) {
					addCastlingMove(asInt(constants::Square::E8), asInt(constants::Square::C8), result);
				}
			}
		}
		
		// Pawns
		for (int wp_square : state.piece_list[asInt(constants::Piece::bP)]) {
			assert(validate::is120OnBoard(wp_square));

			if (state.pieces[wp_square + constants::DIR_DOWN] == asInt(constants::Piece::EMPTY)) {
				addBlackPawnMove(wp_square, wp_square + constants::DIR_DOWN, result, state);

				if (util::_120ToRow(wp_square) == asInt(constants::Rank::_7) &&
					state.pieces[wp_square + 2 * constants::DIR_DOWN] == asInt(constants::Piece::EMPTY)) {
					result.push_back(move::Move(wp_square, wp_square + 2 * constants::DIR_DOWN, false, false, true, 0, false, 0));
				}
			}

			if (validate::is120OnBoard(wp_square + constants::DIR_DOWN_LEFT) &&
				constants::PIECE_COLOR[state.pieces[wp_square + constants::DIR_DOWN_LEFT]] == constants::Color::WHITE)
				addBlackPawnCaptureMove(wp_square, wp_square + constants::DIR_DOWN_LEFT, 
					state.pieces[wp_square + constants::DIR_DOWN_LEFT], result, state);

			if (validate::is120OnBoard(wp_square + constants::DIR_DOWN_RIGHT) &&
				constants::PIECE_COLOR[state.pieces[wp_square + constants::DIR_DOWN_RIGHT]] == constants::Color::WHITE)
				addBlackPawnCaptureMove(wp_square, wp_square + constants::DIR_DOWN_RIGHT, 
					state.pieces[wp_square + constants::DIR_DOWN_RIGHT], result, state);

			if (state.en_passant != asInt(constants::Square::OFFBOARD)) {
				// CHANGE MADE HERE, EN PASSANT SQUARE COUNTS AS CAPUTRE 
				if (wp_square + constants::DIR_DOWN_LEFT == state.en_passant)
					result.push_back(move::Move(wp_square, wp_square + constants::DIR_DOWN_LEFT, state.pieces[state.en_passant], true, false, 0, false, 105 + 1000000));

				if (wp_square + constants::DIR_DOWN_RIGHT == state.en_passant)
					result.push_back(move::Move(wp_square, wp_square + constants::DIR_DOWN_RIGHT, state.pieces[state.en_passant], true, false, 0, false, 105 + 1000000));
			}
		}
	}

	const auto& sliding_pieces = state.player == constants::Color::WHITE ? WHITE_SLIDING_PIECES : BLACK_SLIDING_PIECES;
	const auto& non_sliding_pieces = state.player == constants::Color::WHITE ? WHITE_NON_SLIDING_PIECES : BLACK_NON_SLIDING_PIECES;

	// This is fairly inefficient, should use magic bitboards instead
	// If engine plays too weak, optimize here
	for (constants::Piece piece : sliding_pieces) {
		for (int piece_square : state.piece_list[asInt(piece)]) {
			for (int direction : constants::PIECE_MOVEMENT[asInt(piece)]) {
				int _square = piece_square + direction;

				while (state.pieces[_square] != asInt(constants::Square::OFFBOARD)) {



					if (state.pieces[_square] == asInt(constants::Piece::EMPTY)) {
						addQuietMove(piece_square, _square, result, state);
					}
					// BLACK ^ 1 == WHITE, WHITE ^ 1 == BLACK
					else if (asInt(constants::PIECE_COLOR[state.pieces[_square]]) == (asInt(state.player) ^ 1)) {
						addCaptureMove(piece_square, _square, state.pieces[_square], result, state);
						break;
					}
					else {
						break;
					}

					_square += direction;

				}

			}
		}
	}

	for (constants::Piece piece : non_sliding_pieces) {
		for (int piece_square : state.piece_list[asInt(piece)]) {
			for (int direction : constants::PIECE_MOVEMENT[asInt(piece)]) {
				int _square = piece_square + direction;

				// if (!validate::is120OnBoard(_square)) continue; // <- maybe this is faster

				if (state.pieces[_square] == asInt(constants::Square::OFFBOARD)) continue;

				if (state.pieces[_square] == asInt(constants::Piece::EMPTY)) {
					addQuietMove(piece_square, _square, result, state);
				}
				// BLACK ^ 1 == WHITE, WHITE ^ 1 == BLACK
				else if (asInt(constants::PIECE_COLOR[state.pieces[_square]]) == (asInt(state.player) ^ 1)) {
					addCaptureMove(piece_square, _square, state.pieces[_square], result, state);
				}

			}
		}
	}

	return result;
}

bool movegen::moveExists(board::BoardState& state, const move::Move& test_move) {
	auto moves = generateAllMoves(state);

	for (const auto& move : moves) {
		if (!state.step(move)) {
			continue;
		}

		state.undo();

		if (move == test_move) {
			return true;
		}
	}

	return false;
}


std::vector<move::Move> movegen::generateAllCaptures(board::BoardState& state) {
	std::vector<move::Move> result;

	if (state.player == constants::Color::WHITE) {


		// Pawns
		for (int wp_square : state.piece_list[asInt(constants::Piece::wP)]) {
			assert(validate::is120OnBoard(wp_square));


			if (validate::is120OnBoard(wp_square + constants::DIR_UP_LEFT) &&
				constants::PIECE_COLOR[state.pieces[wp_square + constants::DIR_UP_LEFT]] == constants::Color::BLACK)
				addWhitePawnCaptureMove(wp_square, wp_square + constants::DIR_UP_LEFT,
					state.pieces[wp_square + constants::DIR_UP_LEFT], result, state);

			if (validate::is120OnBoard(wp_square + constants::DIR_UP_RIGHT) &&
				constants::PIECE_COLOR[state.pieces[wp_square + constants::DIR_UP_RIGHT]] == constants::Color::BLACK)
				addWhitePawnCaptureMove(wp_square, wp_square + constants::DIR_UP_RIGHT,
					state.pieces[wp_square + constants::DIR_UP_RIGHT], result, state);

			if (state.en_passant != asInt(constants::Square::OFFBOARD)) {
				if (wp_square + constants::DIR_UP_LEFT == state.en_passant)
					result.push_back(move::Move(wp_square, wp_square + constants::DIR_UP_LEFT, 0, true, false, 0, false, 105));
				else if (wp_square + constants::DIR_UP_RIGHT == state.en_passant)
					result.push_back(move::Move(wp_square, wp_square + constants::DIR_UP_RIGHT, 0, true, false, 0, false, 105));
			}
		}
	}
	else {
		// Pawns
		for (int wp_square : state.piece_list[asInt(constants::Piece::bP)]) {
			assert(validate::is120OnBoard(wp_square));

			if (validate::is120OnBoard(wp_square + constants::DIR_DOWN_LEFT) &&
				constants::PIECE_COLOR[state.pieces[wp_square + constants::DIR_DOWN_LEFT]] == constants::Color::WHITE)
				addBlackPawnCaptureMove(wp_square, wp_square + constants::DIR_DOWN_LEFT,
					state.pieces[wp_square + constants::DIR_DOWN_LEFT], result, state);

			if (validate::is120OnBoard(wp_square + constants::DIR_DOWN_RIGHT) &&
				constants::PIECE_COLOR[state.pieces[wp_square + constants::DIR_DOWN_RIGHT]] == constants::Color::WHITE)
				addBlackPawnCaptureMove(wp_square, wp_square + constants::DIR_DOWN_RIGHT,
					state.pieces[wp_square + constants::DIR_DOWN_RIGHT], result, state);

			if (state.en_passant != asInt(constants::Square::OFFBOARD)) {
				// CHANGE MADE HERE, EN PASSANT SQUARE COUNTS AS CAPUTRE 
				if (wp_square + constants::DIR_DOWN_LEFT == state.en_passant)
					result.push_back(move::Move(wp_square, wp_square + constants::DIR_DOWN_LEFT, state.pieces[state.en_passant], true, false, 0, false, 105 + 1000000));

				if (wp_square + constants::DIR_DOWN_RIGHT == state.en_passant)
					result.push_back(move::Move(wp_square, wp_square + constants::DIR_DOWN_RIGHT, state.pieces[state.en_passant], true, false, 0, false, 105 + 1000000));
			}
		}
	}

	const auto& sliding_pieces = state.player == constants::Color::WHITE ? WHITE_SLIDING_PIECES : BLACK_SLIDING_PIECES;
	const auto& non_sliding_pieces = state.player == constants::Color::WHITE ? WHITE_NON_SLIDING_PIECES : BLACK_NON_SLIDING_PIECES;

	// This is fairly inefficient, should use magic bitboards instead
	// If engine plays too weak, optimize here
	for (constants::Piece piece : sliding_pieces) {
		for (int piece_square : state.piece_list[asInt(piece)]) {
			for (int direction : constants::PIECE_MOVEMENT[asInt(piece)]) {
				int _square = piece_square + direction;

				while (state.pieces[_square] != asInt(constants::Square::OFFBOARD)) {



					if (state.pieces[_square] == asInt(constants::Piece::EMPTY)) {
					}
					// BLACK ^ 1 == WHITE, WHITE ^ 1 == BLACK
					else if (asInt(constants::PIECE_COLOR[state.pieces[_square]]) == (asInt(state.player) ^ 1)) {
						addCaptureMove(piece_square, _square, state.pieces[_square], result, state);
						break;
					}
					else {
						break;
					}

					_square += direction;

				}

			}
		}
	}

	for (constants::Piece piece : non_sliding_pieces) {
		for (int piece_square : state.piece_list[asInt(piece)]) {
			for (int direction : constants::PIECE_MOVEMENT[asInt(piece)]) {
				int _square = piece_square + direction;

				// if (!validate::is120OnBoard(_square)) continue; // <- maybe this is faster

				if (state.pieces[_square] == asInt(constants::Square::OFFBOARD)) continue;

				if (state.pieces[_square] == asInt(constants::Piece::EMPTY)) {
				}
				// BLACK ^ 1 == WHITE, WHITE ^ 1 == BLACK
				else if (asInt(constants::PIECE_COLOR[state.pieces[_square]]) == (asInt(state.player) ^ 1)) {
					addCaptureMove(piece_square, _square, state.pieces[_square], result, state);
				}

			}
		}
	}

	return result;
}