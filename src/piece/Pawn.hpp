#ifndef chesspp_piece_Pawn_HeaderPlusPlus
#define chesspp_piece_Pawn_HeaderPlusPlus

#include "board/Board.hpp"
#include "piece/Piece.hpp"

namespace chesspp
{
namespace piece
{
class Pawn: public virtual Piece
{
    size_t moves;
    util::Direction facing;

  public:
    static constexpr char const* pclass_ = "Pawn";

    Pawn(board::Board& b,
         Position_t const& pos_,
         Suit_t const& s_,
         Score_t const& v,
         size_t m,
         util::Direction const& face,
         Movements_t const& trajectories = {},
         Movements_t const& capturings = {});
    virtual std::unique_ptr<Piece> clone(board::Board& board) override;
    char const* pclass() const override { return pclass_; };

    virtual void moveUpdate(Position_t const& from, Position_t const& to) override;

  protected:
    virtual void calcTrajectory() override;
};
}
}

#endif
