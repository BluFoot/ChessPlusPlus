#ifndef chesspp_board_Board_HeaderPlusPlus
#define chesspp_board_Board_HeaderPlusPlus

#include "config/BoardConfig.hpp"
#include "util/Utilities.hpp"
#include "PlayerDetails.h"

#include <map>
#include <set>
#include <memory>
#include <functional>
#include <typeinfo>
#include <algorithm>
#include <iostream>
#include <unordered_map>

namespace chesspp
{
namespace piece
{
class Piece;
}
namespace board
{

class Board
{
  public:
    using BoardSize_t = config::BoardConfig::BoardSize_t;
    using Position_t = config::BoardConfig::Position_t;
    using Suit_t = config::BoardConfig::SuitClass_t;
    using Suits_t = config::BoardConfig::Suits_t;
    using Pieces_t = std::set<std::unique_ptr<piece::Piece>>;
    using Players_t = std::unordered_map<Suit_t, PlayerDetails>;

  private:
    struct Pieces_t_const_iterator_compare
    {
        bool operator()(Pieces_t::const_iterator const& a, Pieces_t::const_iterator const& b) const {
            return *a < *b;
        }
    };
  public:
    using Movements_t = std::multimap<Pieces_t::const_iterator, Position_t, Pieces_t_const_iterator_compare>;
    using Factory_t = std::map<config::BoardConfig::PieceClass_t, std::function<
        Pieces_t::value_type(Board&, Position_t const&, Suit_t const&)>>; //Used to create new pieces

    config::BoardConfig const& config;

  private:
    Pieces_t pieces;
    Suits_t player_order_;
    Suits_t::const_iterator turn_;
    Players_t players_;
    std::optional<Players_t::const_iterator> winner_;

    Movements_t trajectories; //where pieces can go
    Movements_t capturings;   //where pieces can capture
    Movements_t capturables;  //where pieces can be captured

    static Factory_t& factory() {
        static Factory_t f;
        return f;
    }

  public:
    explicit Board(config::BoardConfig const& conf);
    Board(const Board& board);

    Suit_t turn() const { return *turn_; };
    Suits_t const& player_order() const { return player_order_; }
    Players_t const& players() const { return players_; }
    std::optional<Players_t::const_iterator> winner() const { return winner_; };

    void nextTurn();

    static auto registerPieceClass(Factory_t::key_type const& type, Factory_t::mapped_type ctor) -> Factory_t::iterator {
        return factory().insert({type, ctor}).first;
    }

    bool occupied(Position_t const& pos) const noexcept;
    auto find(Position_t const& pos) const noexcept -> Pieces_t::const_iterator;
    auto find(piece::Piece const& p) const noexcept -> Pieces_t::const_iterator;
    auto begin() const noexcept -> Pieces_t::const_iterator {
        return pieces.cbegin();
    }
    auto end() const noexcept -> Pieces_t::const_iterator {
        return pieces.cend();
    }

  public:
    void addTrajectory(piece::Piece const& p, Position_t const& tile);
    void addCapturing(piece::Piece const& p, Position_t const& tile);
    void addCapturable(piece::Piece const& p, Position_t const& tile);

    Movements_t const& pieceTrajectories() const noexcept { return trajectories; }
    Movements_t const& pieceCapturings() const noexcept { return capturings; }
    Movements_t const& pieceCapturables() const noexcept { return capturables; }

    using MovementsRange = util::Range<Movements_t::const_iterator>;
    MovementsRange pieceTrajectory(piece::Piece const& p) noexcept;
    MovementsRange pieceCapturing(piece::Piece const& p) noexcept;
    MovementsRange pieceCapturable(piece::Piece const& p) noexcept;

  private:
    void addMovement(piece::Piece const& p, Position_t const& tile, Movements_t& m);
    auto pieceMovement(piece::Piece const& p, Movements_t& m) const noexcept -> MovementsRange;

    void update(Position_t const& pos);

  public:
    bool input(Position_t const& from, Position_t const& to);
    bool valid(Position_t const& pos) const noexcept;

  private:
    bool capture(Pieces_t::iterator piece, Pieces_t::iterator enemy);
    bool move(Pieces_t::iterator piece, Position_t const& to);
};
}
}

#endif
