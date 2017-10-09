#include "Queen.hpp"

namespace chesspp
{
namespace piece
{
static auto QueenRegistration = board::Board::registerPieceClass("Queen", [](board::Board& b,
                                                                             board::Board::Position_t const& p,
                                                                             board::Board::Suit_t const& s) -> board::Board::Pieces_t::value_type {
    return board::Board::Pieces_t::value_type(new Queen(b, p, s, "Queen", 9, 0));
});

Queen::Queen(board::Board& b, Position_t const& pos_, Suit_t const& s_, Class_t const& pc, Score_t const& v, size_t m)
    : Piece{b, pos_, s_, pc, v, m} {
}

std::unique_ptr<Piece> Queen::clone(board::Board& board) {
    return std::make_unique<Queen>(board, pos, suit, pclass, value, moves);
}

void Queen::calcTrajectory() {
    //Queens can move infinitely in all eight directions
    using Dir = util::Direction;
    for (Dir d : {Dir::North, Dir::NorthEast, Dir::East, Dir::SouthEast, Dir::South, Dir::SouthWest, Dir::West, Dir::NorthWest}) {
        Position_t t;
        for (signed i = 1; board.valid(t = Position_t(pos).move(d, i)); ++i) {
            if (board.empty(t)) {
                addTrajectory(t);
            } else {
                auto piece = board.find(t);
                if (piece && piece.value()->suit != suit) {
                    addCapturing(t);
                }
                break;
            }
        }
    }
}
}
}
