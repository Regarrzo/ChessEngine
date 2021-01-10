
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <random>

#include "constants.hpp"
#include "util.hpp"
#include "board.hpp"
#include "validate.hpp"
#include "attack.hpp"

namespace board
{
    RandomGenerator::RandomGenerator() : generator(constants::RANDOM_SEED), distribution(0, 0xFFFFFFFFFFFFFFFF) {}

    bitboard::Bitboard RandomGenerator::randomBitboard()
    {
        return distribution(generator);
    }

    RandomPieceKeys::RandomPieceKeys() : generator() {
        int piece_index = 0;
        int square_index = 0;

        for (int piece_index = 0; piece_index < constants::PIECE_TYPE_COUNT; piece_index++)
        {
            for (int square_index = 0; square_index < constants::SQUARES_AMOUNT_PADDED; square_index++)
            {
                piece_keys[piece_index][square_index] = generator.randomBitboard();
            }
        }

        side_key = generator.randomBitboard();

        // 16 possible castling rights combinations
        for (int i = 0; i < 16; i++)
        {
            castle_keys[i] = generator.randomBitboard();
        }
    }

    BoardState::BoardState()
        : pawns(),
          player(constants::Color::WHITE),
          en_passant(asInt(constants::Square::OFFBOARD)),
          fifty_move(),
          ply(),
          his_ply(),
          castle_permissions(),
          position_key(),
          piece_count(),
          piece_count_no_pawns(),
          rooks_queens_count(),
          knights_bishops_count(),
          piece_keys(),
          material(),
          pv_array(),
          search_killers()
          

    {
        for (int i = 0; i < constants::SQUARES_AMOUNT_PADDED; i++)
            pieces[i] = asInt(constants::Square::OFFBOARD);

        for (int i = 0; i < constants::SQUARES_AMOUNT; i++)
            pieces[util::_64To120(i)] = asInt(constants::Piece::EMPTY);

        for (int attacker = asInt(constants::Piece::wP); attacker <= asInt(constants::Piece::bK); attacker++) {
            for (int victim = asInt(constants::Piece::wP); victim <= asInt(constants::Piece::bK); victim++) {
                move_ordering_scores[victim][attacker] = constants::VICTIM_SCORE[victim] + 6 - (constants::VICTIM_SCORE[attacker] / 100);
            }
        }
    }

    void BoardState::loadFromFen(const std::string &fen)
    {
        reset();
        auto fen_parts = util::splitString(fen, " ");

        if (fen_parts.size() != 6)
        {
            throw std::invalid_argument("FEN string is invalid.");
        }

        boardFromFen(fen_parts[0]);
        sideFromFen(fen_parts[1]);
        castlingFromFen(fen_parts[2]);
        enPassantFromFen(fen_parts[3]);
        fiftyMoveFromFen(fen_parts[4]);
        plyFromFen(fen_parts[5]);


        generatePositionKey();
        updateMaterialLists();
    }

    void BoardState::boardFromFen(const std::string &board_fen)
    {
        int index = 0;

        for (char c : board_fen)
        {
            if (c == '/' || c == ' ')
            {
                continue;
            }

            if (isdigit(c))
            {
                index += atoi(&c);
                continue;
            }

            constants::Piece piece;

            switch (c)
            {
            case 'p':
                piece = constants::Piece::bP;
                break;
            case 'n':
                piece = constants::Piece::bN;
                break;
            case 'b':
                piece = constants::Piece::bB;
                break;
            case 'r':
                piece = constants::Piece::bR;
                break;
            case 'q':
                piece = constants::Piece::bQ;
                break;
            case 'k':
                piece = constants::Piece::bK;
                break;
            case 'P':
                piece = constants::Piece::wP;
                break;
            case 'N':
                piece = constants::Piece::wN;
                break;
            case 'B':
                piece = constants::Piece::wB;
                break;
            case 'R':
                piece = constants::Piece::wR;
                break;
            case 'Q':
                piece = constants::Piece::wQ;
                break;
            case 'K':
                piece = constants::Piece::wK;
                break;
            default:
                throw std::invalid_argument("Invalid piece in FEN string.");
            }
            // This loads board flipped along ranks, so flip coordinates before adding

            int square = util::_64To120(index);
            int rank = 7 - util::_120ToRow(square);
            int file = util::_120ToCol(square);

            pieces[util::_120xyToSquare(file, rank)] = asInt(piece);
            index++;
        }
    }

    void BoardState::sideFromFen(const std::string &side_fen)
    {
        if (side_fen == "w")
            player = constants::Color::WHITE;
        else if (side_fen == "b")
            player = constants::Color::BLACK;
        else
            throw std::invalid_argument("Invalid player in FEN string.");
    }

    void BoardState::castlingFromFen(const std::string &castling_fen)
    {
        castle_permissions = 0;

        if (castling_fen.find("K") != std::string::npos)
            castle_permissions |= asInt(constants::Castle::wK);

        if (castling_fen.find("Q") != std::string::npos)
            castle_permissions |= asInt(constants::Castle::wQ);

        if (castling_fen.find("k") != std::string::npos)
            castle_permissions |= asInt(constants::Castle::bK);

        if (castling_fen.find("q") != std::string::npos)
            castle_permissions |= asInt(constants::Castle::bQ);

        assert(castle_permissions >= 0 && castle_permissions < 16);
    }

    void BoardState::enPassantFromFen(const std::string &en_passant_fen)
    {
        if (en_passant_fen == "-")
            en_passant = asInt(constants::Square::OFFBOARD);
        else
            en_passant = util::_64To120(util::stringTo64(en_passant_fen));
    }

    void BoardState::fiftyMoveFromFen(const std::string &fifty_moves_fen)
    {
        fifty_move = std::stoi(fifty_moves_fen);

        assert(fifty_move >= 0);
    }

    void BoardState::plyFromFen(const std::string &ply_fen)
    {
        // This function needs information about the current player
        // player as int returns 1 if black and 0 if white
        //his_ply = ((std::stoi(ply_fen) - 1) * 2) + asInt(player);
    }

    std::string BoardState::toString() const
    {
        std::string s;

        for (int rank = asInt(constants::Rank::_8); rank >= asInt(constants::Rank::_1); --rank)
        {
            s += constants::RANK_CHAR[rank];
            s += "   ";

            for (int file = asInt(constants::File::A); file <= asInt(constants::File::H); file++)
            {
                int square = util::_120xyToSquare(file, rank);
                switch (pieces[square])
                {
                case asInt(constants::Piece::wP):
                    s += "P ";
                    break;
                case asInt(constants::Piece::wN):
                    s += "N ";
                    break;
                case asInt(constants::Piece::wB):
                    s += "B ";
                    break;
                case asInt(constants::Piece::wR):
                    s += "R ";
                    break;
                case asInt(constants::Piece::wQ):
                    s += "Q ";
                    break;
                case asInt(constants::Piece::wK):
                    s += "K ";
                    break;
                case asInt(constants::Piece::bP):
                    s += "p ";
                    break;
                case asInt(constants::Piece::bN):
                    s += "n ";
                    break;
                case asInt(constants::Piece::bB):
                    s += "b ";
                    break;
                case asInt(constants::Piece::bR):
                    s += "r ";
                    break;
                case asInt(constants::Piece::bQ):
                    s += "q ";
                    break;
                case asInt(constants::Piece::bK):
                    s += "k ";
                    break;
                default:
                    s += ". ";
                }
            }

            s += "\n";
        }

        s += "\n";
        s += "    a b c d e f g h\n"; 

        return s;
    }

    void BoardState::generatePositionKey() {
        position_key = 0;

        for (int i = 0; i < constants::SQUARES_AMOUNT_PADDED; i++)
        {
            int piece = pieces[i];

            if (piece != asInt(constants::Square::OFFBOARD) && piece != asInt(constants::Piece::EMPTY))
            {
                position_key ^= piece_keys.piece_keys[asInt(piece)][i];
            }
        }

        if (player == constants::Color::WHITE)
        {
            position_key ^= piece_keys.side_key;
        }

        if (en_passant != asInt(constants::Square::OFFBOARD))
        {
            assert(en_passant >= 0 && en_passant < constants::SQUARES_AMOUNT_PADDED);
            position_key ^= piece_keys.piece_keys[asInt(constants::Piece::EMPTY)][en_passant];
        }

        assert(castle_permissions >= 0 && castle_permissions < 16);

        position_key ^= piece_keys.castle_keys[castle_permissions];
    }
  

    void BoardState::reset() {
        piece_list = {};
        piece_count = {};
        pv_array = {};
        search_killers = {};

        for (int i = 0; i < constants::SQUARES_AMOUNT_PADDED; i++) 
            pieces[i] = asInt(constants::Square::OFFBOARD);

        for (int i = 0; i < constants::SQUARES_AMOUNT; i++)
            pieces[util::_64To120(i)] = asInt(constants::Piece::EMPTY);

        for (int i = 0; i < 2; i++) {
            piece_count_no_pawns[i] = 0;
            rooks_queens_count[i] = 0;
            knights_bishops_count[i] = 0;
            material[i] = 0;
        }


        for (int i = 0; i < 3; i++) {
            pawns[i] = 0;
        }

        player = constants::Color::BOTH;
        en_passant = asInt(constants::Square::OFFBOARD);
        fifty_move = 0;
        ply = 0;
        his_ply = 0;
        castle_permissions = 0;
        position_key = 0;

    }

    void BoardState::updateMaterialLists() {
        for(int i = 0; i < constants::SQUARES_AMOUNT_PADDED; i++) {
            int piece = pieces[i];

            if (piece != asInt(constants::Square::OFFBOARD) && piece != asInt(constants::Piece::EMPTY)) {
                int colour = asInt(constants::PIECE_COLOR[piece]);

                if (constants::IS_NOT_PAWN[piece]) piece_count_no_pawns[colour]++;
                if (constants::IS_KNIGHT_BISHOP[piece]) knights_bishops_count[colour]++;
                if (constants::IS_ROOK_QUEEN_KING[piece]) rooks_queens_count[colour]++;

                material[colour] += constants::PIECE_VALUE[piece];

                piece_list[piece].push_back(i);
                piece_count[piece]++;

                if (piece == asInt(constants::Piece::wP)) 
                    pawns[asInt(constants::Color::WHITE)] = bitboard::setBitAt(pawns[asInt(constants::Color::WHITE)], util::_120To64(i));

                if (piece == asInt(constants::Piece::bP))
                    pawns[asInt(constants::Color::BLACK)] = bitboard::setBitAt(pawns[asInt(constants::Color::BLACK)], util::_120To64(i));

                pawns[asInt(constants::Color::BOTH)] = pawns[asInt(constants::Color::WHITE)] | pawns[asInt(constants::Color::BLACK)];
            }
        }
    }

    int BoardState::checkBoard() {
            
        std::array<int, 13> _piece_count = {};
        std::array<int, 2> _piece_count_no_pawns = {};
        std::array<int, 2> _knights_bishops_count = {};
        std::array<int, 2> _rooks_queens_count = {};
        std::array<int, 2> _material = {};

        std::array<bitboard::Bitboard, 3> _pawns = pawns;

        for (int i = 0; i < constants::SQUARES_AMOUNT; i++) {
            int _120 = util::_64To120(i);
            int _piece = pieces[_120];
            

            if (_piece == asInt(constants::Square::OFFBOARD) || _piece == asInt(constants::Piece::EMPTY))
                continue;

            _piece_count[_piece]++;

            int colour = asInt(constants::PIECE_COLOR[_piece]);

            if (constants::IS_NOT_PAWN[_piece])
                _piece_count_no_pawns[colour]++;

            if (constants::IS_ROOK_QUEEN_KING[_piece])
                _rooks_queens_count[colour]++;
            
            if (constants::IS_KNIGHT_BISHOP[_piece])
                _knights_bishops_count[colour]++;

            _material[colour] += constants::PIECE_VALUE[_piece];
   
        }

        for (int i = asInt(constants::Piece::wP); i <= asInt(constants::Piece::bK); i++) {
            assert(_piece_count[i] == piece_count[i]);
        }


        assert(bitboard::countBits(_pawns[asInt(constants::Color::WHITE)]) == piece_count[asInt(constants::Piece::wP)]);
        assert(bitboard::countBits(_pawns[asInt(constants::Color::BLACK)]) == piece_count[asInt(constants::Piece::bP)]);
        assert(bitboard::countBits(_pawns[asInt(constants::Color::BOTH)]) == piece_count[asInt(constants::Piece::wP)] +
            piece_count[asInt(constants::Piece::bP)]);




        while (_pawns[asInt(constants::Color::WHITE)]) {
            int _64 = bitboard::bitscanForward(_pawns[asInt(constants::Color::WHITE)]);
            _pawns[asInt(constants::Color::WHITE)] = bitboard::clearBitAt(_pawns[asInt(constants::Color::WHITE)], _64);
            assert(pieces[util::_64To120(_64)] == asInt(constants::Piece::wP));
        }

        while (_pawns[asInt(constants::Color::BLACK)]) {
            int _64 = bitboard::bitscanForward(_pawns[asInt(constants::Color::BLACK)]);
            _pawns[asInt(constants::Color::BLACK)] = bitboard::clearBitAt(_pawns[asInt(constants::Color::BLACK)], _64);
            assert(pieces[util::_64To120(_64)] == asInt(constants::Piece::bP));
        }

        while (_pawns[asInt(constants::Color::BOTH)]) {
            int _64 = bitboard::bitscanForward(_pawns[asInt(constants::Color::BOTH)]);
            _pawns[asInt(constants::Color::BOTH)] = bitboard::clearBitAt(_pawns[asInt(constants::Color::BOTH)], _64);
            assert(pieces[util::_64To120(_64)] == asInt(constants::Piece::wP) || pieces[util::_64To120(_64)] == asInt(constants::Piece::bP));
        }

        assert(_material[asInt(constants::Color::WHITE)] == material[asInt(constants::Color::WHITE)]);
        assert(_material[asInt(constants::Color::BLACK)] == material[asInt(constants::Color::BLACK)]);

        assert(_piece_count_no_pawns[asInt(constants::Color::WHITE)] == piece_count_no_pawns[asInt(constants::Color::WHITE)]);
        assert(_piece_count_no_pawns[asInt(constants::Color::BLACK)] == piece_count_no_pawns[asInt(constants::Color::BLACK)]);

        assert(_rooks_queens_count[asInt(constants::Color::WHITE)] == rooks_queens_count[asInt(constants::Color::WHITE)]);
        assert(_rooks_queens_count[asInt(constants::Color::BLACK)] == rooks_queens_count[asInt(constants::Color::BLACK)]);

        assert(_knights_bishops_count[asInt(constants::Color::WHITE)] == knights_bishops_count[asInt(constants::Color::WHITE)]);
        assert(_knights_bishops_count[asInt(constants::Color::BLACK)] == knights_bishops_count[asInt(constants::Color::BLACK)]);

        assert(player == constants::Color::WHITE || player == constants::Color::BLACK);
        // Hash key not checked here

        bitboard::Bitboard _position_key = position_key;
        generatePositionKey();
        assert(_position_key == position_key);

        assert(en_passant == asInt(constants::Square::OFFBOARD) ||
            (util::_120ToRow(en_passant) == asInt(constants::Rank::_6) && player == constants::Color::WHITE) ||
            (util::_120ToRow(en_passant) == asInt(constants::Rank::_3) && player == constants::Color::BLACK)
        );


        return true;
    }

    void BoardState::clearPiece(int square) {
        assert(pieces[square] != asInt(constants::Square::OFFBOARD) && validate::isPieceValidNotEmpty(asInt(pieces[square])));


        int piece = asInt(pieces[square]);
        int color = asInt(constants::PIECE_COLOR[piece]);

        position_key ^= piece_keys.piece_keys[piece][square];
        
        if (constants::IS_NOT_PAWN[piece]) {
            piece_count_no_pawns[color]--;

            if (constants::IS_ROOK_QUEEN[piece]) {
                rooks_queens_count[color]--;
            }
            else {
                knights_bishops_count[color]--;
            }
        }
        else {
            pawns[color] = bitboard::clearBitAt(pawns[color], util::_120To64(square));
            pawns[asInt(constants::Color::BOTH)] = bitboard::clearBitAt(pawns[asInt(constants::Color::BOTH)], util::_120To64(square));
        }

        piece_list[piece].erase(std::remove_if(piece_list[piece].begin(), piece_list[piece].end(), [square](int i) {return i == square; }));
        piece_count[piece]--;

        material[color] -= constants::PIECE_VALUE[piece];

        assert(piece_count[piece] >= 0);
    
        pieces[square] = asInt(constants::Piece::EMPTY); 
    }

    void BoardState::addPiece(int square, constants::Piece piece) {
        assert(pieces[square] != asInt(constants::Square::OFFBOARD) && validate::isPieceValidNotEmpty(asInt(piece)));
        assert(pieces[square] == asInt(constants::Piece::EMPTY));

        int color = asInt(constants::PIECE_COLOR[asInt(piece)]);

        position_key ^= piece_keys.piece_keys[asInt(piece)][square];

        if (constants::IS_NOT_PAWN[asInt(piece)]) {
            piece_count_no_pawns[color]++;

            if (constants::IS_ROOK_QUEEN[asInt(piece)]) {
                rooks_queens_count[color]++;
            }
            else {
                knights_bishops_count[color]++;
            }
        }
        else {
            pawns[color] = bitboard::setBitAt(pawns[color], util::_120To64(square));
            pawns[asInt(constants::Color::BOTH)] = bitboard::setBitAt(pawns[asInt(constants::Color::BOTH)], util::_120To64(square));
        }

        piece_list[asInt(piece)].push_back(square);
        piece_count[asInt(piece)]++;

        material[color] += constants::PIECE_VALUE[asInt(piece)];

        pieces[square] = asInt(piece);
    }

    void BoardState::movePiece(int from, int to) {
        assert(validate::is120OnBoard(from) && validate::is120OnBoard(to));
        assert(pieces[to] == asInt(constants::Piece::EMPTY));
        assert(validate::isPieceValidNotEmpty(pieces[from]));
         
        int piece = pieces[from];
        int color = asInt(constants::PIECE_COLOR[piece]);

        position_key ^= piece_keys.piece_keys[piece][from];
        position_key ^= piece_keys.piece_keys[piece][to];

        pieces[from] = asInt(constants::Piece::EMPTY);
        pieces[to] = piece;

        if (!constants::IS_NOT_PAWN[piece]) {
            int _64_from = util::_120To64(from);
            int _64_to = util::_120To64(to);
            pawns[color] = bitboard::clearBitAt(pawns[color], _64_from);
            pawns[asInt(constants::Color::BOTH)] = bitboard::clearBitAt(pawns[asInt(constants::Color::BOTH)], _64_from);
            pawns[color] = bitboard::setBitAt(pawns[color], _64_to);
            pawns[asInt(constants::Color::BOTH)] = bitboard::setBitAt(pawns[asInt(constants::Color::BOTH)], _64_to);
        }

        auto it = std::find(piece_list[piece].begin(), piece_list[piece].end(), from);
        assert(it != piece_list[piece].end());
        auto index = std::distance(piece_list[piece].begin(), it);

        piece_list[piece][index] = to;
    }


    bool BoardState::step(const move::Move& move) {
        assert(checkBoard());
        assert(validate::is120OnBoard(move.from) && validate::is120OnBoard(move.to));
        assert(validate::isSideValid(asInt(player)));
        assert(validate::isPieceValidNotEmpty(pieces[move.from]));

        undo_stack.push_back({});
        undo_stack.back().position_key = position_key;

      
        if (move.en_passant) {

            if (player == constants::Color::WHITE) {
                clearPiece(move.to + constants::DIR_DOWN);
            } 
            else {
                clearPiece(move.to + constants::DIR_UP);
            }

        }
        else if (move.is_castle) {
            switch (move.to) {
            case asInt(constants::Square::C1):
                movePiece(asInt(constants::Square::A1), asInt(constants::Square::D1));
                break;
            case asInt(constants::Square::C8):
                movePiece(asInt(constants::Square::A8), asInt(constants::Square::D8));
                break;
            case asInt(constants::Square::G1):
                movePiece(asInt(constants::Square::H1), asInt(constants::Square::F1));
                break;
            case asInt(constants::Square::G8):
                movePiece(asInt(constants::Square::H8), asInt(constants::Square::F8));
                break;
            default: assert(false); break;
            }
        }

        if (en_passant != asInt(constants::Square::OFFBOARD))
        {
            assert(en_passant >= 0 && en_passant < constants::SQUARES_AMOUNT_PADDED);
            position_key ^= piece_keys.piece_keys[asInt(constants::Piece::EMPTY)][en_passant];
        }

        assert(castle_permissions >= 0 && castle_permissions < 16);

        // "Remove" castling information from position key
        position_key ^= piece_keys.castle_keys[castle_permissions];

        // A bit slow because it copies the entire move class, may be subject to optimization
        undo_stack.back().move = move;
        undo_stack.back().fifty_move = fifty_move;
        undo_stack.back().en_passant = en_passant;
        undo_stack.back().castle_permissions = castle_permissions;

        castle_permissions &= constants::CASTLE_PERMISSIONS[move.from];
        castle_permissions &= constants::CASTLE_PERMISSIONS[move.to];
        en_passant = asInt(constants::Square::OFFBOARD);

        // Add new castling information to position key
        position_key ^= piece_keys.castle_keys[castle_permissions];

        fifty_move++;

        if (move.captured) {
            assert(validate::isPieceValidNotEmpty(move.captured));

            clearPiece(move.to);
            fifty_move = 0;
        }

        his_ply++;
        ply++;

        if (!constants::IS_NOT_PAWN[pieces[move.from]]) {
            fifty_move = 0;

            if (move.pawn_start) {
                if (player == constants::Color::WHITE) {
                    en_passant = move.from + constants::DIR_UP;
                    assert(util::_120ToRow(en_passant) == asInt(constants::Rank::_3));
                }
                else {
                    en_passant = move.from + constants::DIR_DOWN;
                    assert(util::_120ToRow(en_passant) == asInt(constants::Rank::_6));
                }

                position_key ^= piece_keys.piece_keys[asInt(constants::Piece::EMPTY)][en_passant];
            }
        }

        movePiece(move.from, move.to);

        if (move.promoted_piece) {
            assert(validate::isPieceValidNotEmpty(move.promoted_piece) && constants::IS_NOT_PAWN[move.promoted_piece]);
            clearPiece(move.to);
            addPiece(move.to, static_cast<constants::Piece>(move.promoted_piece));
        }

        // switch side
        constants::Piece king = (player == constants::Color::WHITE) ? constants::Piece::wK : constants::Piece::bK;

        player = static_cast<constants::Color>(asInt(player) ^ 1);
        position_key ^= piece_keys.side_key;

        assert(checkBoard());


        if (attack::isSquareAttacked(piece_list[asInt(king)][0], player, *this)) {
            undo();
            return false;
        }

        

        return true;
    }

    void BoardState::stepNull() {
        constants::Piece king = (player == constants::Color::WHITE) ? constants::Piece::wK : constants::Piece::bK;

        assert(checkBoard());
        assert(!attack::isSquareAttacked(piece_list[asInt(king)][0], static_cast<constants::Color>(asInt(player) ^ 1), *this));

        ply++;
        his_ply++;
        position_key ^= piece_keys.side_key;
        
        undo_stack.push_back(UndoInfo());

        if(en_passant != asInt(constants::Square::OFFBOARD)) {
            position_key ^= piece_keys.piece_keys[asInt(constants::Piece::EMPTY)][en_passant];
        }
        en_passant = asInt(constants::Square::OFFBOARD);

        undo_stack.back().move = move::Move();
        undo_stack.back().fifty_move = fifty_move;
        undo_stack.back().en_passant = en_passant;
        undo_stack.back().castle_permissions = castle_permissions;
        undo_stack.back().position_key = position_key;
        
        player = static_cast<constants::Color>(asInt(player) ^ 1);

        

        assert(checkBoard());
    }

    void BoardState::undoNull() {
        constants::Piece king = (player == constants::Color::WHITE) ? constants::Piece::wK : constants::Piece::bK;

        assert(checkBoard());
        assert(!attack::isSquareAttacked(piece_list[asInt(king)][0], static_cast<constants::Color>(asInt(player) ^ 1), *this));

        ply--;
        his_ply--;

        const UndoInfo& info = undo_stack.back();

        castle_permissions = info.castle_permissions;
        en_passant = info.en_passant;
        position_key = info.position_key;
        fifty_move = info.fifty_move;
        position_key ^= piece_keys.side_key;

        player = static_cast<constants::Color>(asInt(player) ^ 1);

        undo_stack.pop_back();
        assert(checkBoard());
    }

    void BoardState::undo() {

        assert(checkBoard());

        his_ply--;
        ply--;

        const UndoInfo& info = undo_stack.back();
        
        assert(validate::is120OnBoard(info.move.to));
        assert(validate::is120OnBoard(info.move.from));

        castle_permissions = info.castle_permissions;
        fifty_move = info.fifty_move;
        en_passant = info.en_passant;

        player = static_cast<constants::Color>(asInt(player) ^ 1);

        if (info.move.en_passant) {

            if (player == constants::Color::WHITE) {
                addPiece(info.move.to + constants::DIR_DOWN, constants::Piece::bP);
            }
            else {
                addPiece(info.move.to + constants::DIR_UP, constants::Piece::wP);
            }
        }
        else if (info.move.is_castle) {
            switch (info.move.to) {
            case asInt(constants::Square::C1):
                movePiece(asInt(constants::Square::D1), asInt(constants::Square::A1));
                break;
            case asInt(constants::Square::C8):
                movePiece(asInt(constants::Square::D8), asInt(constants::Square::A8));
                break;
            case asInt(constants::Square::G1):
                movePiece(asInt(constants::Square::F1), asInt(constants::Square::H1));
                break;
            case asInt(constants::Square::G8):
                movePiece(asInt(constants::Square::F8), asInt(constants::Square::H8));
                break;
            default: assert(false); break;
            }
        }

        movePiece(info.move.to, info.move.from);

        if (info.move.captured) {
            assert(validate::isPieceValid(info.move.captured));
            addPiece(info.move.to, static_cast<constants::Piece>(info.move.captured));
        }

        if (info.move.promoted_piece) {
            assert(validate::isPieceValidNotEmpty(info.move.promoted_piece) && constants::IS_NOT_PAWN[info.move.promoted_piece]);
            clearPiece(info.move.from);
            addPiece(info.move.from, 
                (constants::PIECE_COLOR[info.move.promoted_piece] == constants::Color::WHITE ? constants::Piece::wP : constants::Piece::bP));
        }

        position_key = info.position_key;
        undo_stack.pop_back();
        assert(checkBoard());
        

    }

    bool BoardState::isRepetition() const {
        for (int i = his_ply - fifty_move; i < his_ply - 1; i++) {
            const auto& info = undo_stack[i];

            if (info.position_key == position_key)
                return true;
        }

        return false;
    }

} // namespace board