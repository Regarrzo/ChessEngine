#pragma once

#include "board.hpp"
#include "pvtable.hpp"

namespace search {

	class Searcher {
	public:
		Searcher(size_t table_size, int depth) : depth(depth), table(table_size), quit(false){
		};

		void checkTimeUp();
		void setupForSearch(board::BoardState& state);

		int quiesence(board::BoardState& state, int alpha, int beta);
		int alphaBeta(board::BoardState& state, int alpha, int beta, int depth, bool null);
		void searchPosition(board::BoardState& state);
		void readInput();

		pvtable::PVTable table;

		bool infinite;
		bool quit;
		bool stopped;
		long start_time;
		long stop_time;
		int depth;
		int depthset;
		int timeset;
		int remaining_moves;
		long nodes;

		float fh;
		float fhf;
	};




}

