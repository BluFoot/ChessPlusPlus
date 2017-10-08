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
    using Suit_t = PlayerDetails::Suit_t;
    std::optional<Move> calc(Board const& board);

  private:
    std::vector<Move> legalMoves(Board const& board);
    int evalBoard(const Board& board, const Suit_t& turn);
};
}
}
