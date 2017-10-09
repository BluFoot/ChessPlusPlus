#ifndef chesspp_piece_King_HeaderPlusPlus
#define chesspp_piece_King_HeaderPlusPlus

#include "board/Board.hpp"
#include "piece/Piece.hpp"

namespace chesspp
{
namespace piece
{
class King: public virtual Piece
{
  public:
    using Piece::Piece;
    virtual std::unique_ptr<Piece> clone(board::Board& board) override;

  protected:
    virtual void calcTrajectory() override;
};
}
}

#endif
