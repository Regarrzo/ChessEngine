#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <cassert>
#include <stdio.h>

#include "constants.hpp"

namespace util
{
    #ifdef WIN32
    #include "windows.h"
    #else
    #include "time.h"
    #endif
    constexpr int getCapturePriority(constants::Piece attacker, constants::Piece victim) {
        return constants::PIECE_VALUE[asInt(victim)] - constants::PIECE_VALUE[asInt(attacker)];
    }

    constexpr long getTimeInMs() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    constexpr int _64To120(int _64)
    {
        assert(0 <= _64 && _64 <= 63);

        // Division here needs to be integer division, thus further simplification isn't wanted
        return _64 + 21 + 2 * (_64 / 8);
    }

    constexpr int _120To64(int _120)
    {
        assert(21 <= _120 && _120 <= 98);

        // Division here needs to be integer division, thus further simplification isn't wanted
        return _120 - 17 - 2 * (_120 / 10);
    }

    constexpr int _120xyToSquare(int x, int y)
    {
        return (21 + x) + (y * 10);
    }

    constexpr int _64xyToSquare(int x, int y)
    {
        return x + constants::BOARD_LENGTH * y;
    }

    constexpr int _120ToRow(int _120) {
        return (_120 / 10) - 2;
    }

    constexpr int _120ToCol(int _120) {
        return (_120 % 10) - 1;
    }

    inline int stringTo64(const std::string &pos)
    {
        // Chess rows are indexed from 1 onwards
        int row = atoi(&pos[1]) - 1;
        int column = pos[0] - 97;

        return row * constants::BOARD_LENGTH + column;
    }

    inline std::string _120ToString(int _120)
    {
        // Chess rows are indexed from 1 onwards
        
        std::string result;

        int row = _120ToRow(_120);
        int col = _120ToCol(_120);

        result += constants::FILE_CHAR[col];
        result += constants::RANK_CHAR[row];

        return result;
    }

    inline std::vector<std::string> splitString(const std::string &s, const std::string &delimiter)
    {
        std::vector<std::string> result;

        size_t last = 0;
        size_t next = 0;

        while ((next = s.find(delimiter, last)) != std::string::npos)
        {
            result.push_back(s.substr(last, next - last));
            last = next + delimiter.size();
        }

        result.push_back(s.substr(last));
        return result;
    }


    // Code from Olithink
    inline int inputWaiting()
    {
#ifndef WIN32
        fd_set readfds;
        struct timeval tv;
        FD_ZERO(&readfds);
        FD_SET(fileno(stdin), &readfds);
        tv.tv_sec = 0; tv.tv_usec = 0;
        select(16, &readfds, 0, 0, &tv);

        return (FD_ISSET(fileno(stdin), &readfds));
#else
        static int init = 0, pipe;
        static HANDLE inh;
        DWORD dw;

        if (!init) {
            init = 1;
            inh = GetStdHandle(STD_INPUT_HANDLE);
            pipe = !GetConsoleMode(inh, &dw);
            if (!pipe) {
                SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
                FlushConsoleInputBuffer(inh);
            }
        }
        if (pipe) {
            if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL)) return 1;
            return dw;
        }
        else {
            GetNumberOfConsoleInputEvents(inh, &dw);
            return dw <= 1 ? 0 : dw;
        }
#endif
    }



} // namespace util