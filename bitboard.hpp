#pragma once

#include <array>
#include <vector>
#include <cassert>
#include <cinttypes>
#include <bitset>
#include <string>

#include "constants.hpp"
#include "util.hpp"

namespace bitboard
{
    typedef std::uint64_t Bitboard;

    constexpr bool hasBitAt(const Bitboard board, int position)
    {
        assert(position >= 0 && position < constants::SQUARES_AMOUNT);
        return (board >> position) & 1;
    }

    constexpr Bitboard setBitAt(Bitboard board, int position)
    {
        assert(position >= 0 && position < constants::SQUARES_AMOUNT);
        return board | (static_cast<uint64_t>(1ULL) << position);
    }

    constexpr Bitboard clearBitAt(Bitboard board, int position)
    {
        assert(position >= 0 && position < constants::SQUARES_AMOUNT);
        return board & ~(static_cast<uint64_t>(1ULL) << position);
    }

    constexpr Bitboard mask(Bitboard board, Bitboard mask)
    {
        return board & mask;
    }

    constexpr Bitboard swapMask(Bitboard board, Bitboard mask)
    {
        return board ^ mask;
    }

    inline std::string toString(Bitboard board)
    {
        std::string s;

        for (int rank = asInt(constants::Rank::_8); rank >= asInt(constants::Rank::_1); --rank)
        {
            for (int file = asInt(constants::File::A); file <= asInt(constants::File::H); file++)
            {
                int square = util::_120xyToSquare(file, rank);
                int _64_square = util::_120To64(square);

                if (hasBitAt(board, _64_square))
                {
                    s += "1 ";
                }
                else
                {
                    s += "0 ";
                }
            }

            s += "\n";
        }
        return s;
    }

#ifdef USE_ASM
# ifndef _MSC_VER 
    inline int bitscanForward(uint64_t x)
    {
        uint64_t r;

        asm(
            "bsf %[x],%[r];"
            : [ r ] "=a"(r)
            : [ x ] "r"(x)
        );

        return r;
    }

    inline int bitscanReverse(uint64_t x)--
    {
        uint64_t r;

        asm(
            "bsr %[x],%[r];"
            : [ r ] "=a"(r)
            : [ x ] "r"(x));

        return r;
    }
# else
    
    inline int bitscanForward(uint64_t x) {
        unsigned long result = -1;
        _BitScanForward64(&result, x);
        return result;
    }

    inline int bitscanReverse(uint64_t x) {
        unsigned long result = -1;
        _BitScanReverse64(&result, x);
        return result;
    }
# endif
#else
#pragma message("USE_ASM not defined: Using slow alternative implementation of bitscan.")

    inline int bitscanForward(uint64_t x)
    {
        int result = 0;
        while ((x & 1) == 0)
        {
            x >>= 1;
            ++result;
        }

        return result;
    }

    inline int bitscanReverse(uint64_t x)
    {
        int result = 0;
        while ((x & (0ULL | 1ULL << 63)) == 0)
        {
            x <<= 1;
            ++result;
        }

        return result;
    }
#endif

    inline int countBits(Bitboard board)
    {
        int bit_count = 0;
        Bitboard current_board = board;

        while (current_board)
        {
            current_board = current_board >> (1 + bitscanForward(current_board));
            bit_count++;
        }

        return bit_count;
    }

} // namespace bitboard
