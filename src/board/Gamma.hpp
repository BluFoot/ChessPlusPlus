#pragma once

#include "Board.hpp"

namespace chesspp
{
namespace board
{
class Gamma
{
  public:
    using Move = Board::Move;
    std::optional<Move> calc(Board const& board);

  private:
    std::vector<Move> legalMoves(Board const& board);
    std::optional<Move> best_move;

};
}
}
