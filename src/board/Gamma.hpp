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

  private:
    Suit_t player;

  public:
    std::optional<Move> chooseMove(Board board);

  private:
    int calc(Board const& board, size_t depth);
    std::vector<Move> legalMoves(Board const& board);
    int evalBoard(const Board& board, const Suit_t& turn);
};
}
}
