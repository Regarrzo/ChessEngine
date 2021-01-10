#pragma once

#include <string>
#include <stdexcept>

#include "constants.hpp"
#include "util.hpp"


namespace move {

	class Move {
	public:
		Move() : from(), to(), captured(), en_passant(), pawn_start(), promoted_piece(), is_castle(), score()
		{}

		Move(int from, int to, int captured, bool en_passant, bool pawn_start, int promoted_piece, bool is_castle, int score) :
			from(from),
			to(to), 
			captured(captured),
			en_passant(en_passant),
			pawn_start(pawn_start), 
			promoted_piece(promoted_piece),
			is_castle(is_castle),
			score(score)
		{}

		bool operator==(Move const& other) const {
			// terrible
			return from == other.from && to == other.to && captured == other.captured &&
				en_passant == other.en_passant && pawn_start == other.pawn_start && promoted_piece == other.promoted_piece &&
				is_castle == other.is_castle;
		}


		bool isNull() const {
			return from == 0 && to == 0;
		}

		std::string toSquareString() const {
			std::string result;

			result += util::_120ToString(from);
			result += util::_120ToString(to);

			return result;
		}

		std::string toString() const {
			std::string result;

			result += toSquareString();
		
			if (promoted_piece) {
				if (constants::IS_QUEEN[promoted_piece])
					result += "q";
				else if (constants::IS_KNIGHT[promoted_piece])
					result += "n";
				else if (constants::IS_BISHOP[promoted_piece])
					result += "b";
				else if (constants::IS_ROOK[promoted_piece])
					result += "r";
				else
					throw std::runtime_error("Invalid promoted piece.");
			}

			return result;
		}

		int from;
		int to;
		int captured;
		bool en_passant;
		bool pawn_start;
		int promoted_piece;
		bool is_castle;
		int score;
	};

	inline bool compareMoves(const Move& a, const Move& b) { return a.score > b.score; }

}