#pragma once

#include <iostream>
#include <stdexcept>

#include "board.hpp"
#include "move.hpp"
#include "search.hpp"
#include "attack.hpp"
#include "util.hpp"
#include "constants.hpp"

namespace uci {


	move::Move parseMove(board::BoardState& state, std::string move_string) {
		auto moves = movegen::generateAllMoves(state);

		for (const auto& move : moves) {
			if (move.toString() == move_string)
				return move;
		}

		throw std::runtime_error("Invalid Move.");
	}

	inline void parseGoCommand(std::string line, search::Searcher& searcher, board::BoardState& state) {
		
		int depth = -1;
		int remaining_moves = 30;
		int movetime = -1;
		int time = -1;
		int increment = 0;
		
		auto parts = util::splitString(line, " ");

		for (int i = 1; i < parts.size(); i += 2) {
			if (parts[i] == "infinite") { ; }

			if (state.player == constants::Color::WHITE) {
				if (parts[i] == "winc") { increment = std::stoi(parts[i + 1]); }
				if (parts[i] == "wtime") { time = std::stoi(parts[i + 1]); }
			}
			else {
				if (parts[i] == "binc") { increment = std::stoi(parts[i + 1]); }
				if (parts[i] == "btime") { time = std::stoi(parts[i + 1]); }
			}

			if (parts[i] == "movestogo") { remaining_moves = std::stoi(parts[i + 1]); }
			if (parts[i] == "movetime") { movetime = std::stoi(parts[i + 1]); }
			if (parts[i] == "depth") { depth = std::stoi(parts[i + 1]); }
		}

		if (movetime != -1) {
			time = movetime;
			remaining_moves = 1;
		}

		searcher.start_time = util::getTimeInMs();
		searcher.depth = depth;

		if (time != -1) {
			searcher.timeset = true;
			time /= remaining_moves;
			time -= 50;
			searcher.stop_time = searcher.start_time + time + increment;
		}
		else {
			searcher.timeset = false;
		}

		if (depth == -1) {
			searcher.depth = 64;
		}

		std::cout << "time " << time << std::endl;
		searcher.searchPosition(state);
	}

	inline void parsePositionCommand(std::string line, board::BoardState& state) {
		auto parts = util::splitString(line, " ");
		

		if (parts[1] == "startpos") {
			state.loadFromFen(constants::FEN_START_POS);
		}
		else {
			try {
				std::string only_fen = util::splitString(line, "position ")[1];
				only_fen = util::splitString(only_fen, " moves ")[0];
				state.loadFromFen(only_fen);
			}
			catch (std::invalid_argument) {
				std::cout << "Invalid Fen" << std::endl;
				state.reset();
				return;
			}
		}

		auto command_moves = util::splitString(line, "moves ");
		if (command_moves.size() == 1)
			return;
		else if (command_moves.size() != 2) {
			std::cout << "Invalid Command Format" << std::endl;
			return;
		}
		
		auto moves = util::splitString(command_moves[1], " ");
		move::Move current_move;

		for (const auto& move_str : moves) {
			move::Move current_move = parseMove(state, move_str);

			if (current_move.isNull())
				break;

			std::cout << current_move.toString() << std::endl;
			state.step(current_move);
		}

		state.ply = 0;

		
	}

	inline void uciLoop() {
		std::string input;

		board::BoardState state = board::BoardState();
		search::Searcher searcher = search::Searcher(1000000*4, 10);


		while(true) {

			std::getline(std::cin, input);

			if (input.empty())
				continue;

			if (input[0] == '\n')
				continue;

			auto parts = util::splitString(input, " ");
			const std::string &command = parts[0];
			
			if (command == "isready") {
				std::cout << "readyok" << std::endl;
			}
			else if(command == "position") {
				parsePositionCommand(input, state);
				std::cout << state.toString() << std::endl;
			}
			else if (command == "ucinewgame") {
				parsePositionCommand("position startpos", state);
			}
			else if (command == "quit") {
				searcher.quit = true;
				break;
			}
			else if (command == "go") {
				parseGoCommand(input, searcher, state);
			}
			else if (command == "uci") {
				std::cout << "id name " << constants::NAME << std::endl;
				std::cout << "id author " << constants::AUTHOR << std::endl;
				std::cout << "uciok" << std::endl;
			}

			if (searcher.quit)
				break;
		
		}
		
	}





}