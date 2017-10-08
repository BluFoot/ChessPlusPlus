#include "Piece.hpp"

#include "board/Board.hpp"

#include <iostream>

namespace chesspp
{
namespace piece
{
Piece::Piece(board::Board& b, Position_t const& pos_, Suit_t const& s_, Class_t const& pc, Score_t v, size_t m)
    : board(b) //can't use {}
      , p{pos_}
      , s{s_}
      , c{pc}
      , v{v}
      , m{m} {}

void Piece::addTrajectory(Position_t const& tile) {
    board.addTrajectory(*this, tile);
}

void Piece::addCapturing(Position_t const& tile) {
    board.addCapturing(*this, tile);
}

void Piece::transform(const Piece::Class_t& to) {
    c = to;
}

std::ostream& operator<<(std::ostream& os, Piece const& p) {
    return os << "Piece (" << typeid(p).name() << ") \"" << p.suit << "\" \"" << p.pclass << "\" at " << p.pos << " having made "
              << p.moves << " moves";
}
}
}
