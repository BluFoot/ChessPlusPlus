#include "Queen.hpp"

namespace chesspp
{
namespace piece
{
const std::string Queen::qclass = "Queen";

static auto QueenRegistration = board::Board::registerPieceClass(Queen::qclass, [](board::Board& b,
                                                                                   board::Board::Position_t const& p,
                                                                                   board::Board::Suit_t const& s) -> board::Board::Pieces_t::value_type {
    return board::Board::Pieces_t::value_type(new Queen(b, p, s, Queen::qclass, 9));
});

Queen::Queen(board::Board& b, Position_t const& pos_, Suit_t const& s_, Class_t const& pc, Score_t const& v)
    : Piece{b, pos_, s_, pc, v} {
}

void Queen::calcTrajectory() {
    //Queens can move infinitely in all eight directions
    using Dir = util::Direction;
    for (Dir d : {Dir::North, Dir::NorthEast, Dir::East, Dir::SouthEast, Dir::South, Dir::SouthWest, Dir::West, Dir::NorthWest}) {
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
