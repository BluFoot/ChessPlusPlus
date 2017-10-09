#include "Bishop.hpp"

namespace chesspp
{
namespace piece
{
static auto BishopRegistration = board::Board::registerPieceClass(Bishop::pclass_, [](board::Board& b,
                                                                                      board::Board::Position_t const& p,
                                                                                      board::Board::Suit_t const& s) -> board::Board::Pieces_t::value_type {
    return board::Board::Pieces_t::value_type(new Bishop(b, p, s, 5, 0));
});

std::unique_ptr<Piece> Bishop::clone(board::Board& board) {
    return std::make_unique<Bishop>(board, pos, suit, value, moves, trajectories, capturings);
}

void Bishop::calcTrajectory() {
    //Bishops can move infinitely in the four diagonal directions
    using Dir = util::Direction;
    for (Dir d : {Dir::NorthEast, Dir::SouthEast, Dir::SouthWest, Dir::NorthWest}) {
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
