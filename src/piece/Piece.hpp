#ifndef chesspp_piece_Piece_HeaderPlusPlus
#define chesspp_piece_Piece_HeaderPlusPlus

#include "config/BoardConfig.hpp"

#include <memory>
#include <set>
#include <typeinfo>
#include <iosfwd>

namespace chesspp
{
namespace board
{
class Board;
}
namespace piece
{
/**
 * \brief
 * A chess piece.
 */
class Piece
{
    friend class ::chesspp::board::Board;

  public:
    using Position_t = config::BoardConfig::Position_t;
    using Suit_t     = config::BoardConfig::SuitClass_t;
    using Class_t    = config::BoardConfig::PieceClass_t;
    using Score_t    = config::BoardConfig::Score_t;

    board::Board& board;

  private:
    Position_t p;
    Suit_t s;
    Class_t c;
    Score_t v;
    std::size_t m;

  public:
    Position_t const& pos = p;
    Suit_t const& suit = s;
    Class_t const& pclass = c;
    std::size_t const& moves = m;
    Score_t const& value = v;

    Piece(board::Board& b,
          Position_t const& pos,
          Suit_t const& s,
          Class_t const& pc,
          Score_t v,
          size_t m);
    virtual ~Piece() = default;
    virtual std::unique_ptr<Piece> clone(board::Board& board) = 0;

  protected:
    virtual void calcTrajectory() = 0;

    void addTrajectory(Position_t const& tile);
    void addCapturing(Position_t const& tile);
    //void addCapturable(Position_t const& tile);

    void transform(Class_t const& to);

  private:
    void makeTrajectory() {
        //addCapturable(pos);
        calcTrajectory();
    }

    virtual void tick(Position_t const& m) {
    }

    void move(Position_t const& to) {
        Position_t from = pos;
        p = to;
        moveUpdate(from, to);
        ++m;
    }


    virtual void moveUpdate(Position_t const& from, Position_t const& to) {
    }

  public:
    friend std::ostream& operator<<(std::ostream& os, Piece const& p);
};
}
}

#endif
