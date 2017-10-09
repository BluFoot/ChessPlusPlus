#include "King.hpp"

namespace chesspp
{
namespace piece
{
static auto KingRegistration = board::Board::registerPieceClass(King::pclass_, [](board::Board& b,
                                                                                  board::Board::Position_t const& p,
                                                                                  board::Board::Suit_t const& s) -> board::Board::Pieces_t::value_type {
    return board::Board::Pieces_t::value_type(new King(b, p, s, 20, 0));
});

std::unique_ptr<Piece> King::clone(board::Board& board) {
    return std::make_unique<King>(board, pos, suit, value, moves, trajectories, capturings);
}

void King::calcTrajectory() {
    //Kings can move one space in all eight directions
    using Dir = util::Direction;
    for (Dir d : {Dir::North, Dir::NorthEast, Dir::East, Dir::SouthEast, Dir::South, Dir::SouthWest, Dir::West, Dir::NorthWest}) {
        Position_t t = Position_t(pos).move(d);
        if (board.empty(pos)) {
            addTrajectory(t);
        } else {
            auto piece = board.find(t);
            if (piece && piece.value()->suit != suit) {
                addCapturing(t);
            }
        }
    }
}
}
}
