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
    using Pieces_t = std::vector<std::unique_ptr<piece::Piece>>;
    using Coord_t = Position_t::Coord_t;
    using Players_t = std::unordered_map<Suit_t, PlayerDetails>;

    static constexpr Coord_t BOARD_SIZE = 16;
    struct Square
    {
        enum SquareState { INVALID, EMPTY, OCCUPIED };
        SquareState state;
        Piece_pt piece; // only valid if state is occupied

        void occupy(Piece_cpt p) {
            state = OCCUPIED;
            piece = p;
        };

        bool valid() const { return state != INVALID; };
        bool empty() const { return state == EMPTY; };
        bool occupied() const { return state == OCCUPIED; };
    };
    using Positions_t = std::array<std::array<Square, BOARD_SIZE>, BOARD_SIZE>;

    struct Move
    {
        Position_t from;
        Position_t to;
    };

    friend std::ostream& operator<<(std::ostream& os, Move const& m) {
        return os << m.from << " => " << m.to;
    }

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

    std::optional<Piece_cpt> find(Position_t const& pos) const noexcept;
    bool inBounds(Position_t const& pos) const noexcept;
    bool valid(Position_t const& pos) const noexcept;
    bool empty(Position_t const& pos) const noexcept;
    bool occupied(Position_t const& pos) const noexcept;

  public:
    bool input(Move const& move);
    void inputQuick(Move const& move);

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
