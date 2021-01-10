#pragma once

#include <cinttypes>
#include <array>
#include <string>

#define asInt(x) static_cast<int>(x)

namespace constants
{

    // Computer constants

    inline const char *NAME = "SeminarEngine";
    inline const char* AUTHOR = "Linus Röber";

    // General chess constants

    inline const int BOARD_LENGTH = 8;
    inline const int SQUARES_AMOUNT = 64;
    inline const int SQUARES_AMOUNT_PADDED = 120;

    inline const int PIECE_TYPE_COUNT = 13;

    inline const std::string FEN_START_POS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    // Hashing constants

    inline const int RANDOM_SEED = 1234;
    constexpr int INFINITE_VAL = 999999999;
    constexpr int MATE = 99999998;

    enum class Color
    {
        WHITE = 0,
        BLACK = 1,
        BOTH = 2
    };

    enum class Piece
    {
        EMPTY,
        wP,
        wN,
        wB,
        wR,
        wQ,
        wK,
        bP,
        bN,
        bB,
        bR,
        bQ,
        bK
    };

    inline const std::array<int, 13> IS_QUEEN = { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0 };
    inline const std::array<int, 13> IS_ROOK = { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0 };
    inline const std::array<int, 13> IS_BISHOP = { 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0 };

    inline const std::array<int, 13> IS_KNIGHT = { 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 };
    inline const std::array<int, 13> IS_BISHOP_QUEEN = { 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0 };
    inline const std::array<int, 13> IS_ROOK_QUEEN = { 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0 };
    inline const std::array<int, 13> IS_KING = { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 };

    inline const std::array<int, 13> IS_NOT_PAWN = { 0, 0, 1, 1, 1, 1, 1, 0, 1 ,1, 1 , 1, 1 };
    inline const std::array<int, 13> IS_ROOK_QUEEN_KING = {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1 };
    inline const std::array<int, 13> IS_KNIGHT_BISHOP ={ 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0 };
    inline const std::array<Color, 13> PIECE_COLOR = { Color::BOTH, Color::WHITE, Color::WHITE, Color::WHITE, Color::WHITE, Color::WHITE, Color::WHITE,
                                                       Color::BLACK, Color::BLACK, Color::BLACK, Color::BLACK, Color::BLACK, Color::BLACK};

    inline const std::array<int, 13> VICTIM_SCORE = { 0, 100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600 };

    inline const std::array<std::vector<int>, 13> PIECE_MOVEMENT = { 
        std::vector<int>{},
        std::vector<int>{}, 
        std::vector<int>{-8, -19, -21, -12, 8, 19, 21, 12},
        std::vector<int>{-9, -11, 11, 9},
        std::vector<int>{-1, -10, 1, 10},
        std::vector<int>{-1, -10, 1, 10, -9, -11, 11, 9},
        std::vector<int>{-1, -10, 1, 10, -9, -11, 11, 9},
        std::vector<int>{},
        std::vector<int>{-8, -19, -21, -12, 8, 19, 21, 12},
        std::vector<int>{-9, -11, 11, 9},
        std::vector<int>{-1, -10, 1, 10},
        std::vector<int>{-1, -10, 1, 10, -9, -11, 11, 9},
        std::vector<int>{-1, -10, 1, 10, -9, -11, 11, 9}                                         
    };

    constexpr int DIR_UP = 10;
    constexpr int DIR_DOWN = -10;
    constexpr int DIR_LEFT = -1;
    constexpr int DIR_RIGHT = 1;

    constexpr int DIR_UP_LEFT = DIR_UP + DIR_LEFT;
    constexpr int DIR_UP_RIGHT = DIR_UP + DIR_RIGHT;
    constexpr int DIR_DOWN_LEFT = DIR_DOWN + DIR_LEFT;
    constexpr int DIR_DOWN_RIGHT = DIR_DOWN + DIR_RIGHT;

    inline const char* FILE_CHAR = "abcdefgh";

    enum class File
    {
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        NONE
    };

    inline const char* RANK_CHAR = "12345678";

    enum class Rank
    {
        _1,
        _2,
        _3,
        _4,
        _5,
        _6,
        _7,
        _8,
        NONE
    };

    enum class Square
    {
        A1 = 21,
        B1,
        C1,
        D1,
        E1,
        F1,
        G1,
        H1,
        A2 = 31,
        B2,
        C2,
        D2,
        E2,
        F2,
        G2,
        H2,
        A3 = 41,
        B3,
        C3,
        D3,
        E3,
        F3,
        G3,
        H3,
        A4 = 51,
        B4,
        C4,
        D4,
        E4,
        F4,
        G4,
        H4,
        A5 = 61,
        B5,
        C5,
        D5,
        E5,
        F5,
        G5,
        H5,
        A6 = 71,
        B6,
        C6,
        D6,
        E6,
        F6,
        G6,
        H6,
        A7 = 81,
        B7,
        C7,
        D7,
        E7,
        F7,
        G7,
        H7,
        A8 = 91,
        B8,
        C8,
        D8,
        E8,
        F8,
        G8,
        H8,
        OFFBOARD
    };

    const std::array<int, 120> CASTLE_PERMISSIONS =
    {
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
     15, 13, 15, 15, 15, 12, 15, 15, 14, 15,
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
     15,  7, 15, 15, 15,  3, 15, 15, 11, 15,
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15
    };

    // evaluation

    inline const std::array<int, 13> PIECE_VALUE = { 0, 100, 325, 325, 550, 1000, 50000, 100, 325, 325, 550, 1000, 50000 };

    const std::array<int, 64> PAWN_VALUE_TABLE = {
    0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
    10	,	10	,	0	,	-10	,	-10	,	0	,	10	,	10	,
    5	,	0	,	0	,	5	,	5	,	0	,	0	,	5	,
    0	,	0	,	10	,	20	,	20	,	10	,	0	,	0	,
    5	,	5	,	5	,	10	,	10	,	5	,	5	,	5	,
    10	,	10	,	10	,	20	,	20	,	10	,	10	,	10	,
    20	,	20	,	20	,	30	,	30	,	20	,	20	,	20	,
    0	,	0	,	0	,	0	,	0	,	0	,	0	,	0
    };

    const std::array<int, 64> KNIGHT_VALUE_TABLE = {
    0	,	-10	,	0	,	0	,	0	,	0	,	-10	,	0	,
    0	,	0	,	0	,	5	,	5	,	0	,	0	,	0	,
    0	,	0	,	10	,	10	,	10	,	10	,	0	,	0	,
    0	,	0	,	10	,	20	,	20	,	10	,	5	,	0	,
    5	,	10	,	15	,	20	,	20	,	15	,	10	,	5	,
    5	,	10	,	10	,	20	,	20	,	10	,	10	,	5	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	0	,	0	,	0	,	0	,	0	,	0
    };

    const std::array<int, 64> BISHOP_VALUE_TABLE = {
    0	,	0	,	-10	,	0	,	0	,	-10	,	0	,	0	,
    0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
    0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
    0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
    0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
    0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
    0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
    0	,	0	,	0	,	0	,	0	,	0	,	0	,	0
    };

    const std::array<int, 64> ROOK_VALUE_TABLE = {
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0
    };

    const std::array<int, 64> MIRROR_SQUARE = {
    56	,	57	,	58	,	59	,	60	,	61	,	62	,	63	,
    48	,	49	,	50	,	51	,	52	,	53	,	54	,	55	,
    40	,	41	,	42	,	43	,	44	,	45	,	46	,	47	,
    32	,	33	,	34	,	35	,	36	,	37	,	38	,	39	,
    24	,	25	,	26	,	27	,	28	,	29	,	30	,	31	,
    16	,	17	,	18	,	19	,	20	,	21	,	22	,	23	,
    8	,	9	,	10	,	11	,	12	,	13	,	14	,	15	,
    0	,	1	,	2	,	3	,	4	,	5	,	6	,	7
    };



    enum class Castle
    {
        wK = 1,
        wQ = 2,
        bK = 4,
        bQ = 8
    };


} // namespace constants