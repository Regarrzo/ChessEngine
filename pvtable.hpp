#pragma once

#include <memory>
#include <stdexcept>

#include "move.hpp"
#include "board.hpp"
#include "movegen.hpp"
#include "bitboard.hpp"

namespace pvtable {

	struct Entry {
		Entry(bitboard::Bitboard position_key, move::Move move) : position_key(position_key), move(move) {}
		Entry() : position_key(), move() {}

		move::Move move;
		bitboard::Bitboard position_key;
	};

	class PVTable {
	public:
		PVTable(size_t size) : size(size) {
			data = std::make_unique<Entry[]>(size);
		}

		PVTable() : size(10000) {
			data = std::make_unique<Entry[]>(size);
		}

		void clear() {
			// maybe a bit slow, but no need for optimization

			for (int i = 0; i < size; i++) {
				data[i].move = {};
				data[i].position_key = {};
			}
		}

		void add(bitboard::Bitboard key, move::Move value) {
			size_t index = key % size;
			assert(0 <= index && index < size);
			data[index] = Entry(key, value);
		}

		move::Move probe(bitboard::Bitboard key) const {
			size_t index = key % size;
			assert(0 <= index && index < size);
			
			if (data[index].position_key == key)
				return data[index].move;

			return {};
		}

		int getLine(board::BoardState& state, int depth) {
			state.pv_array.clear();
			move::Move move = probe(state.position_key);
			int count = 0;

			for (int i = 0; i < depth; i++) {
				if (movegen::moveExists(state, move)) {
					state.step(move);
					state.pv_array.push_back(move);
					count++;
				}
				else {
					break;
				}

				move = probe(state.position_key);

				if (move.isNull())
					break;
			}

			while (state.ply > 0) {
				state.undo();
			}

			return count;
		}

	private:

		size_t size;
		std::unique_ptr<Entry[]> data;
		
	};


}