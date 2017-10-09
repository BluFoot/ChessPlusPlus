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
    using Movements_t = std::vector<Position_t>;

    board::Board& board;

  private:
    Position_t p_;
    Suit_t s_;
    Class_t c_;
    Score_t v_;
    std::size_t m_;

    Movements_t trajectories_;
    Movements_t capturings_;

  public:
    Position_t const& pos = p_;
    Suit_t const& suit = s_;
    virtual char const* pclass() const = 0;
    std::size_t const& moves = m_;
    Score_t const& value = v_;

    Movements_t const& trajectories = trajectories_;
    Movements_t const& capturings = capturings_;

    Piece(board::Board& b,
          Position_t const& p,
          Suit_t const& s,
          Score_t v,
          size_t m,
          Movements_t const& trajectories = {},
          Movements_t const& capturings = {});
    virtual ~Piece() = default;
    virtual std::unique_ptr<Piece> clone(board::Board& board) = 0;

  protected:
    void makeTrajectory();
    virtual void calcTrajectory() = 0;

    void addTrajectory(Position_t const& tile);
    void addCapturing(Position_t const& tile);

    void transform(Class_t const& to);

  private:
    void move(Position_t const& to);
    virtual void moveUpdate(Position_t const& from, Position_t const& to) {}

  public:
    friend std::ostream& operator<<(std::ostream& os, Piece const& p);
};
}
}

#endif
