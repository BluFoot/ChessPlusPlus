#ifndef chesspp_piece_Queen_HeaderPlusPlus
#define chesspp_piece_Queen_HeaderPlusPlus

#include "board/Board.hpp"
#include "piece/Piece.hpp"

namespace chesspp
{
namespace piece
{
class Queen: public virtual Piece
{
  public:
    static constexpr char const* pclass_ = "Queen";

    using Piece::Piece;
    virtual std::unique_ptr<Piece> clone(board::Board& board) override;
    char const* pclass() const override { return pclass_; };

  protected:
    virtual void calcTrajectory() override;
};
}
}

#endif
