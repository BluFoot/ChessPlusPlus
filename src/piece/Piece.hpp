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
    Position_t p_;
    Suit_t s_;
    Class_t c_;
    Score_t v_;
    std::size_t m_;

    std::vector<Position_t> trajectories_;
    std::vector<Position_t> capturings_;

  public:
    Position_t const& pos = p_;
    Suit_t const& suit = s_;
    Class_t const& pclass = c_;
    std::size_t const& moves = m_;
    Score_t const& value = v_;

    std::vector<Position_t> const& trajectories = trajectories_;
    std::vector<Position_t> const& capturings = capturings_;

    Piece(board::Board& b, Position_t const& p, Suit_t const& s, Class_t const& c, Score_t v, size_t m);
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
