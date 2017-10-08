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
    bool en_passant = true;
    util::Direction facing;

  public:
    Pawn(board::Board& b,
         Position_t const& pos,
         Suit_t const& s,
         Class_t const& pc,
         Score_t const& v,
         size_t m,
         util::Direction const& face);
    virtual std::unique_ptr<Piece> clone(board::Board& board) override;

    virtual void tick(Position_t const& p) override;
    virtual void moveUpdate(Position_t const& from, Position_t const& to) override;

  protected:
    virtual void calcTrajectory() override;
};
}
}

#endif
