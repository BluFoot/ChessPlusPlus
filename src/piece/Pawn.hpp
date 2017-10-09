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
    size_t moves_;
    util::Direction facing_;
    bool transformed_;

  public:
    static constexpr char const* pclass_ = "Pawn";
    static constexpr char const* pclass_transformed_ = "Queen";
    static constexpr Score_t value_ = 1;

    Pawn(board::Board& b,
         Position_t const& pos_,
         Suit_t const& s_,
         size_t m,
         util::Direction const& face,
         bool transformed,
         Movements_t const& trajectories = {},
         Movements_t const& capturings = {});
    virtual std::unique_ptr<Piece> clone(board::Board& board) override;
    char const* pclass() const override { return transformed_ ? pclass_transformed_ : pclass_; };
    Score_t value() const override { return value_; };

    virtual void moveUpdate(Position_t const& from, Position_t const& to) override;

  protected:
    virtual void calcTrajectory() override;
};
}
}

#endif
