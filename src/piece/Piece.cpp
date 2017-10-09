#include "Piece.hpp"

#include "board/Board.hpp"

#include <iostream>

namespace chesspp
{
namespace piece
{
Piece::Piece(board::Board& b, Position_t const& p, Suit_t const& s, Class_t const& c, Score_t v, size_t m)
    : board(b) //can't use {}
      , p_{p}
      , s_{s}
      , c_{c}
      , v_{v}
      , m_{m} {}

void Piece::makeTrajectory() {
    trajectories_.clear();
    capturings_.clear();
    calcTrajectory();
}

void Piece::addTrajectory(Position_t const& tile) {
    trajectories_.emplace_back(tile);
}

void Piece::addCapturing(Position_t const& tile) {
    capturings_.emplace_back(tile);
}

void Piece::transform(const Piece::Class_t& to) {
    c_ = to;
}

std::ostream& operator<<(std::ostream& os, Piece const& p) {
    return os << "Piece (" << typeid(p).name() << ") \"" << p.suit << "\" \"" << p.pclass << "\" at " << p.pos << " having made "
              << p.moves << " moves";
}
void Piece::move(const Piece::Position_t& to) {
    Position_t from = pos;
    p_ = to;
    moveUpdate(from, to);
    ++m_;
}
}
}
