#ifndef chesspp_board_Board_HeaderPlusPlus
#define chesspp_board_Board_HeaderPlusPlus

#include "config/BoardConfig.hpp"
#include "util/Utilities.hpp"
#include "PlayerDetails.hpp"

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
    using Piece_pt = piece::Piece*;
    using Piece_cpt = Piece_pt const;
    using Pieces_t = std::set<std::unique_ptr<piece::Piece>>;
    using Positions_t = std::unordered_map<Position_t, std::optional<Piece_cpt>>;
    using Players_t = std::unordered_map<Suit_t, PlayerDetails>;

    struct Move
    {
        Position_t from;
        Position_t to;
    };

    friend std::ostream& operator<<(std::ostream& os, Move const& m) {
        return os << m.from << " => " << m.to;
    }

    using Movements_t = std::multimap<Piece_cpt, Position_t>;
    using Factory_t = std::map<config::BoardConfig::PieceClass_t, std::function<
        Pieces_t::value_type(Board&, Position_t const&, Suit_t const&)>>; //Used to create new pieces

    config::BoardConfig const& config;

  private:
    Pieces_t pieces_;
    Positions_t positions_;

    Suits_t player_order_;
    Suits_t::const_iterator turn_;
    Players_t players_;
    std::optional<Players_t::const_iterator> winner_;

    Movements_t trajectories; //where pieces can go
    Movements_t capturings;   //where pieces can capture

    static Factory_t& factory() {
        static Factory_t f;
        return f;
    }

  public:
    explicit Board(config::BoardConfig const& conf);
    Board(const Board& board);

    Pieces_t const& pieces() const { return pieces_; }
    Suit_t const& turn() const { return *turn_; };
    Suits_t const& player_order() const { return player_order_; }
    Players_t const& players() const { return players_; }
    std::optional<Players_t::const_iterator> winner() const { return winner_; };

    static auto registerPieceClass(Factory_t::key_type const& type, Factory_t::mapped_type ctor) -> Factory_t::iterator {
        return factory().insert({type, ctor}).first;
    }

    auto find(Position_t const& pos) const noexcept -> std::optional<Piece_cpt> {
        auto it = positions_.find(pos);
        if (it != positions_.end() && it->second)
            return it->second;
        else
            return std::nullopt;
    }

    bool valid(Position_t const& pos) const noexcept;
    bool empty(Position_t const& pos) const noexcept;
    bool occupied(Position_t const& pos) const noexcept;

  public:
    void addTrajectory(Piece_cpt p, Position_t const& tile);
    void addCapturing(Piece_cpt p, Position_t const& tile);

    Movements_t const& pieceTrajectories() const noexcept { return trajectories; }
    Movements_t const& pieceCapturings() const noexcept { return capturings; }

    using MovementsRange = util::Range<Movements_t::const_iterator>;
    MovementsRange pieceTrajectory(Piece_cpt p) const noexcept;
    MovementsRange pieceCapturing(Piece_cpt p) const noexcept;

  private:
    void addMovement(Piece_cpt p, Position_t const& tile, Movements_t& m);
    auto pieceMovement(Piece_cpt p, Movements_t const& m) const noexcept -> MovementsRange;

  public:

    bool input(Move const& move);
    bool inputQuick(Move const& move);

  private:
    void movePiece(Piece_cpt piece, Position_t const& to);
    bool moveTo(Piece_cpt piece, Position_t const& to);
    bool capture(Piece_cpt piece, Piece_cpt enemy, Position_t const& to);
    void kill(const Board::Suit_t& suit, Piece_cpt enemy);

    void update();
    void nextTurn();
};
}
}

#endif
