#include "Knight.hpp"

namespace chesspp
{
namespace piece
{
static auto KnightRegistration = board::Board::registerPieceClass("Knight", [](board::Board& b,
                                                                               board::Board::Position_t const& p,
                                                                               board::Board::Suit_t const& s) -> board::Board::Pieces_t::value_type {
    return board::Board::Pieces_t::value_type(new Knight(b, p, s, "Knight", 3, 0));
});

std::unique_ptr<Piece> Knight::clone(board::Board& board) {
    return std::make_unique<Knight>(board, pos, suit, pclass, value, moves, trajectories, capturings);
}

void Knight::calcTrajectory() {
    //Knights can only move in 3-long 2-short L shapes
    for (Position_t m : {Position_t(1, -2), Position_t(2, -1), Position_t(2, 1), Position_t(1, 2), Position_t(-1, 2),
                         Position_t(-2, 1), Position_t(-2, -1), Position_t(-1, -2)}) {
        Position_t t = Position_t(pos).move(m.x, m.y);
        if (board.empty(t)) {
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
