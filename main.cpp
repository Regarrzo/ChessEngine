#include <iostream>
#include <string>

#include "util.hpp"
#include "bitboard.hpp"
#include "board.hpp"
#include "constants.hpp"
#include "test.hpp"
#include "attack.hpp"
#include "move.hpp"
#include "movegen.hpp"
#include "perft.hpp"
#include "pvtable.hpp"
#include "evaluate.hpp"
#include "search.hpp"
#include "uci.hpp"


int main(int, char **)
{
	testAll();
	uci::uciLoop();
	return 0;
}
