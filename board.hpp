#pragma once

#include <array>
#include <cinttypes>
#include <unordered_map>
#include <vector>
#include <string>
#include <random>

#include "constants.hpp"
#include "bitboard.hpp"
#include "util.hpp"
#include "move.hpp"

namespace board
{

    class RandomGenerator
    {
    public:
        RandomGenerator();
        bitboard::Bitboard randomBitboard();

    private:
        std::default_random_engine generator;
        std::uniform_int_distribution<bitboard::Bitboard> distribution;
    };

    class RandomPieceKeys
    {
    public:
        RandomPieceKeys();

        std::array<std::array<bitboard::Bitboard, constants::SQUARES_AMOUNT_PADDED>, constants::PIECE_TYPE_COUNT> piece_keys;
        std::array<bitboard::Bitboard, 16> castle_keys; // 16 possible types of combinations for castling rights
        bitboard::Bitboard side_key;

    private:
        RandomGenerator generator;
    };
    typedef std::array<std::vector<int>, 13> PieceList;

    struct UndoInfo {
        move::Move move;
        int castle_permissions;
        int en_passant;
        int fifty_move;
        bitboard::Bitboard position_key;
    };

    constexpr int BETA_KILLER_STORAGE = 256;

    struct BoardState
    {
        BoardState();

        void loadFromFen(const std::string &fen);
        std::string toString() const;

        std::array<int, constants::SQUARES_AMOUNT_PADDED> pieces;
        std::array<bitboard::Bitboard, 3> pawns;

        constants::Color player;

        int en_passant;
        int fifty_move;

        int ply;
        int his_ply;

        int castle_permissions;

        bitboard::Bitboard position_key;

        std::array<int, 13> piece_count;
        std::array<int, 2> piece_count_no_pawns;
        std::array<int, 2> rooks_queens_count;
        std::array<int, 2> knights_bishops_count;
        std::array<int, 2> material;

        RandomPieceKeys piece_keys;

        std::array<std::vector<int>, 13> piece_list;
        std::vector<UndoInfo> undo_stack;
        std::vector<move::Move> pv_array;

        std::array<std::array<int, constants::SQUARES_AMOUNT_PADDED>, 13> search_history;
        std::array<std::array<move::Move, BETA_KILLER_STORAGE>, 2> search_killers;

        std::array<std::array<int, 13>, 13> move_ordering_scores;

        bool step(const move::Move& move);
        void stepNull();
        void undoNull();
        void undo();
        void generatePositionKey();
        void reset();
        void updateMaterialLists();
        int checkBoard();
        bool isRepetition() const;



    private:
        void boardFromFen(const std::string &board_fen);
        void sideFromFen(const std::string &side_fen);
        void castlingFromFen(const std::string &castling_fen);
        void enPassantFromFen(const std::string &en_passant_fen);
        void fiftyMoveFromFen(const std::string &fifty_moves_fen);
        void plyFromFen(const std::string &ply_fen);
        
        void clearPiece(int square);
        void addPiece(int square, constants::Piece piece);
        void movePiece(int from, int to);

  
    };
} // namespace board