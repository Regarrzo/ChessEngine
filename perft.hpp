#pragma once

#include <iostream>

#include "board.hpp"
#include "movegen.hpp"

namespace perft {
	long node_count;

	inline void perft(board::BoardState& state, int depth) {
		assert(state.checkBoard());

		if (depth <= 0) {
			node_count++;
			return;
		}

		auto moves = movegen::generateAllMoves(state);

		for (const auto& move : moves) {
			if (!state.step(move)) {
				//state.undo();
				continue;
			}

			perft(state, depth - 1);
			state.undo();
		}

		return;
	}

	inline void perftTest(int depth, board::BoardState& state) {
		assert(state.checkBoard());
		long start_time = util::getTimeInMs();
		std::cout << "Perft Testing Board:" << std::endl;
		std::cout << state.toString() << std::endl;
		node_count = 0;


		auto moves = movegen::generateAllMoves(state);


		for (const auto& move : moves) {
			std::cout << move.toString() << std::endl;

			if (!state.step(move)) {
				std::cout << "test" << std::endl;
				continue;
			}
				

			long nodes_before = node_count;
			perft(state, depth - 1);
			state.undo();

			std::cout << "Move " << move.toString() << ": visited " << node_count - nodes_before << " Nodes." << std::endl;
		}

		std::cout << "Total Nodes Visited: " << node_count << std::endl;
		std::cout << "Test complete in " << util::getTimeInMs() - start_time << "ms." << std::endl;
	}
}