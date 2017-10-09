#include "Rook.hpp"

namespace chesspp
{
namespace piece
{
static auto RookRegistration = board::Board::registerPieceClass(Rook::pclass_, [](board::Board& b,
                                                                                  board::Board::Position_t const& p,
                                                                                  board::Board::Suit_t const& s) -> board::Board::Pieces_t::value_type {
    return board::Board::Pieces_t::value_type(new Rook(b, p, s));
});

std::unique_ptr<Piece> Rook::clone(board::Board& board) {
    return std::make_unique<Rook>(board, pos, suit, trajectories, capturings);
}

void Rook::calcTrajectory() {
    //Rooks can move infinitely in the four straight directions
    using Dir = util::Direction;
    for (Dir d : {Dir::North, Dir::East, Dir::South, Dir::West}) {
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
