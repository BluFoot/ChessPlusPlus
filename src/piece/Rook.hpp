#ifndef chesspp_piece_Rook_HeaderPlusPlus
#define chesspp_piece_Rook_HeaderPlusPlus

#include "board/Board.hpp"
#include "piece/Piece.hpp"

namespace chesspp
{
namespace piece
{
class Rook: public virtual Piece
{
  public:
    static constexpr char const* pclass_ = "Rook";
    static constexpr Score_t value_ = 5;

    using Piece::Piece;
    virtual std::unique_ptr<Piece> clone(board::Board& board) override;
    char const* pclass() const override { return pclass_; };
    Score_t value() const override { return value_; };

  protected:
    virtual void calcTrajectory() override;
};
}
}

#endif
