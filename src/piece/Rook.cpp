#include "Rook.hpp"

namespace chesspp
{
namespace piece
{
static auto RookRegistration = board::Board::registerPieceClass("Rook", [](board::Board& b,
                                                                           board::Board::Position_t const& p,
                                                                           board::Board::Suit_t const& s) -> board::Board::Pieces_t::value_type {
    return board::Board::Pieces_t::value_type(new Rook(b, p, s, "Rook", 5, 0));
});

Rook::Rook(board::Board& b, Position_t const& pos_, Suit_t const& s_, Class_t const& pc, Score_t const& v, size_t m)
    : Piece{b, pos_, s_, pc, v, m} {
}

std::unique_ptr<Piece> Rook::clone(board::Board& board) {
    return std::make_unique<Rook>(board, pos, suit, pclass, value, moves);
}

void Rook::calcTrajectory() {
    //Rooks can move infinitely in the four straight directions
    using Dir = util::Direction;
    for (Dir d : {Dir::North, Dir::East, Dir::South, Dir::West}) {
        Position_t t;
        for (signed i = 1; board.valid(t = Position_t(pos).move(d, i)); ++i) {
            if (board.empty(t)) {
                addTrajectory(t);
            } else if (board.occupied(t)) {
                addCapturing(t);
                break;
            }
        }
    }
}
}
}
