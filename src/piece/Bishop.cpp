#include "Bishop.hpp"

namespace chesspp
{
namespace piece
{
static auto BishopRegistration = board::Board::registerPieceClass("Bishop", [](board::Board& b,
                                                                               board::Board::Position_t const& p,
                                                                               board::Board::Suit_t const& s) -> board::Board::Pieces_t::value_type {
    return board::Board::Pieces_t::value_type(new Bishop(b, p, s, "Bishop", 5));
});

Bishop::Bishop(board::Board& b, Position_t const& pos_, Suit_t const& s_, Class_t const& pc, Score_t const& v)
    : Piece{b, pos_, s_, pc, v} {
}

std::unique_ptr<Piece> Bishop::clone() {
    return std::make_unique<Bishop>(*this);
}

void Bishop::calcTrajectory() {
    //Bishops can move infinitely in the four diagonal directions
    using Dir = util::Direction;
    for (Dir d : {Dir::NorthEast, Dir::SouthEast, Dir::SouthWest, Dir::NorthWest}) {
        Position_t t;
        for (signed i = 1; board.valid(t = Position_t(pos).move(d, i)); ++i) {
            addCapturing(t);
            if (!board.occupied(t)) {
                addTrajectory(t);
            } else break; //can't jump over pieces
        }
    }
}
}
}
