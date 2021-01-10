#pragma once

#include <iostream>

#include "util.hpp"
#include "bitboard.hpp"
#include "board.hpp"
#include "constants.hpp"
#include "perft.hpp"

inline void testAll() {

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            int square = util::_64xyToSquare(col, row);
            assert(util::_120ToRow(util::_64To120(square)) == row);
            assert(util::_120ToCol(util::_64To120(square)) == col);
        }
    }

    bitboard::Bitboard board = 0;
    board = bitboard::setBitAt(board, 0);
    board = bitboard::setBitAt(board, 63);
    board = bitboard::setBitAt(board, 32);
    board = bitboard::clearBitAt(board, 31);
    board = bitboard::clearBitAt(board, 0);
    board = bitboard::clearBitAt(board, 63);

    assert(bitboard::hasBitAt(board, 32));
    assert(!bitboard::hasBitAt(board, 0));
    assert(!bitboard::hasBitAt(board, 63));
    assert(!bitboard::hasBitAt(board, 31));

    board::BoardState state;
    state.loadFromFen(constants::FEN_START_POS);
    state.loadFromFen(constants::FEN_START_POS);
    perft::perftTest(3, state);

}

