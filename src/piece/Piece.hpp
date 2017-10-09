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
    using Player_t     = config::BoardConfig::Player_t;
    using Class_t    = config::BoardConfig::Piece_t;
    using Score_t    = config::BoardConfig::Score_t;
    using Movements_t = std::vector<Position_t>;

    board::Board& board;

  private:
    Position_t pos_;

    Movements_t trajectories_;
    Movements_t capturings_;

  public:
    Position_t const& pos = pos_;
    const Player_t player;
    virtual char const* pclass() const = 0;
    virtual Score_t value() const = 0;

    Movements_t const& trajectories = trajectories_;
    Movements_t const& capturings = capturings_;

    Piece(board::Board& b,
          Position_t const& p,
          Player_t const& s,
          Movements_t const& trajectories = {},
          Movements_t const& capturings = {});
    virtual ~Piece() = default;
    virtual std::unique_ptr<Piece> clone(board::Board& board) = 0;

  protected:
    void makeTrajectory();
    virtual void calcTrajectory() = 0;

    void addTrajectory(Position_t const& tile);
    void addCapturing(Position_t const& tile);

  private:
    void move(Position_t const& to);
    virtual void moveUpdate(Position_t const& from, Position_t const& to) {}

  public:
    friend std::ostream& operator<<(std::ostream& os, Piece const& p);
};
}
}

#endif
