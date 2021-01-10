#include <iostream>


#include "search.hpp"
#include "board.hpp"
#include "pvtable.hpp"
#include "util.hpp"
#include "move.hpp"
#include "constants.hpp"
#include "attack.hpp"
#include "movegen.hpp"
#include "evaluate.hpp"

namespace search {

	void Searcher::readInput() {
		if (util::inputWaiting()) {
			stopped = true;
			std::string input;

			std::getline(std::cin, input);

			if (!input.empty()) {
				if (input == "quit") {
					quit = true;
				}
			}
			return;
		}
	}

	constexpr int MAX_DEPTH = 128;

	void Searcher::setupForSearch(board::BoardState& state) {
		state.search_history = {};
		state.search_killers = {};

		state.ply = 0;
		table.clear();

		stopped = false;
		nodes = 0;
		fh = 0;
		fhf = 0;
	}

	void Searcher::checkTimeUp() {
		if (timeset && util::getTimeInMs() > stop_time)
			stopped = true;

		readInput();
	}

	void Searcher::searchPosition(board::BoardState& state) {
		int best_move = {};
		int best_score = -constants::INFINITE_VAL;
		int pv_moves = 0;
		int pv_number = 0;

		setupForSearch(state);

		for (int current_depth = 1; current_depth <= depth; current_depth++) {
			best_score = alphaBeta(state, -constants::INFINITE_VAL, constants::INFINITE_VAL, current_depth, true);

			pv_moves = table.getLine(state, current_depth);

			std::cout << "info score cp " << best_score << " depth " << current_depth << " nodes " << nodes
				<< " time " << util::getTimeInMs() - start_time << std::endl;

			std::cout << "Principle Variation: " << std::endl;

			for (const auto& move : state.pv_array) {
				std::cout << move.toString() << ", ";
			}
			std::cout << std::endl;

			std::cout << "Ordering: " << fhf / fh << std::endl;

			if (stopped)
				break;
		}

		std::cout << "bestmove " << state.pv_array[0].toString() << std::endl;

	}

	int Searcher::quiesence(board::BoardState& state, int alpha, int beta) {
		assert(state.checkBoard());

		if (nodes % 2047 == 0)
			checkTimeUp();


		nodes++;

		if ((state.isRepetition() || state.fifty_move >= 100) && state.ply) {
			return 0;
		}

		if(state.ply > MAX_DEPTH - 1) {
			return evaluate::evaluatePosition(state);
		}

		int score = evaluate::evaluatePosition(state);

		if (score >= beta)
			return beta;

		if (score > alpha)
			alpha = score;

		auto moves = movegen::generateAllCaptures(state);
		int legal_moves = 0;
		int prev_alpha = alpha;
		move::Move pv_move;
		move::Move best_move = {};

		pv_move = table.probe(state.position_key);

		if (!pv_move.isNull()) {
			for (auto& move : moves) {
				if (move == pv_move) {
					move.score = 2000000;
					break;
				}
			}
		}

		std::sort(moves.begin(), moves.end(), move::compareMoves);

		for (const auto& move : moves) {
			if (!state.step(move))
				continue;

			legal_moves++;
			int score = -quiesence(state, -beta, -alpha);
			state.undo();



			if (stopped == true)
				return 0;

			if (score > alpha) {
				if (score >= beta) {
					if (legal_moves == 1) {
						fhf++;
					}
					fh++;
					return beta;
				}

				alpha = score;
				best_move = move;
			}


		}
		if (alpha != prev_alpha) {
			table.add(state.position_key, best_move);
		}

		return alpha;
	}

	int Searcher::alphaBeta(board::BoardState& state, int alpha, int beta, int depth, bool null) {
		assert(state.checkBoard());
		

		nodes++;

		if (depth == 0) {
			return quiesence(state, alpha, beta);
		}

		if (nodes % 2047 == 0)
			checkTimeUp();


		if ((state.isRepetition() || state.fifty_move >= 100) && state.ply) {
			return 0;
		}

		if (state.ply > MAX_DEPTH - 1) {
			return evaluate::evaluatePosition(state);
		}

		int king = state.player == constants::Color::WHITE ? asInt(constants::Piece::wK) : asInt(constants::Piece::bK);
		bool is_in_check = attack::isSquareAttacked(state.piece_list[king][0], static_cast<constants::Color>(asInt(state.player) ^ 1), state);

		if (is_in_check) {
			depth++;
		}

		// fix this
		/*
		if (null && !is_in_check && state.ply && (state.knights_bishops_count[asInt(state.player)] + state.rooks_queens_count[asInt(state.player)]) > 0 && depth >= 4) {
			state.stepNull();
			int score = -alphaBeta(state, -beta, -beta + 1, depth - 4, false);
			state.undoNull();

			if (stopped)
				return 0;

			if (score >= beta)
				return beta;

		}

		*/
		

		auto moves = movegen::generateAllMoves(state);

		int legal_moves = 0;
		int prev_alpha = alpha;
		move::Move pv_move;
		move::Move best_move = {};

		pv_move = table.probe(state.position_key);

		if (!pv_move.isNull()) {
			for (auto& move : moves) {
				if (move == pv_move) {
					move.score = 2000000;
					break;
				}
			}
		}
		

		std::sort(moves.begin(), moves.end(), move::compareMoves);

		for (const auto& move : moves) {
			if (!state.step(move))
				continue;

			legal_moves++;
			int score = -alphaBeta(state, -beta, -alpha, depth - 1, true);
			state.undo();

			if (stopped == true)
				return 0;


			if (score > alpha) {
				if (score >= beta) {
					if (legal_moves == 1) {
						fhf++;
					}
					fh++;
					
					if (!move.captured) {
						state.search_killers[1][state.ply] = state.search_killers[0][state.ply];
						state.search_killers[0][state.ply] = move;
					}
					
					return beta;
				}

				alpha = score;
				best_move = move;

				if (!move.captured) {
					state.search_history[state.pieces[move.from]][move.to] += depth;
				}
			}

			
		}

		if (legal_moves == 0) {
			int king = state.player == constants::Color::WHITE ? asInt(constants::Piece::wK) : asInt(constants::Piece::bK);

			if (attack::isSquareAttacked(state.piece_list[king][0], static_cast<constants::Color>(asInt(state.player) ^ 1), state)) {
				return -constants::MATE + state.ply;
			}
			else {
				return 0;
			}
		}

		if (alpha != prev_alpha)
			table.add(state.position_key, best_move);

		return alpha;

	}
}