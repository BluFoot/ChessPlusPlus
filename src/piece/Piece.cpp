#include "Piece.hpp"

#include "board/Board.hpp"

#include <iostream>

namespace chesspp
{
namespace piece
{
Piece::Piece(board::Board& b,
             Position_t const& p,
             Player_t const& s,
             Movements_t const& trajectories,
             Movements_t const& capturings)
    : board(b) //can't use {}
      , pos_{p}
      , player{s}
      , trajectories_{trajectories}
      , capturings_{capturings} {}

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

std::ostream& operator<<(std::ostream& os, Piece const& p) {
    return os << "Piece (" << typeid(p).name() << ") \"" << p.player << "\" \"" << p.pclass() << "\" at " << p.pos;
}
void Piece::move(const Piece::Position_t& to) {
    Position_t from = pos;
    pos_ = to;
    moveUpdate(from, to);
}
}
}
