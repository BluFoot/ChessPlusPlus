#include "Pawn.hpp"

namespace chesspp
{
namespace piece
{
static auto PawnRegistration = board::Board::registerPieceClass("Pawn", [](board::Board& b,
                                                                           board::Board::Position_t const& p,
                                                                           board::Board::Suit_t const& s) -> board::Board::Pieces_t::value_type {
    auto d = util::Direction::None;
    std::istringstream {std::string(b.config.metadata("pawn facing", p.y, p.x))} >> d;
    return board::Board::Pieces_t::value_type(new Pawn(b, p, s, "Pawn", 1, 0, d));
});

Pawn::Pawn(board::Board& b,
           Position_t const& pos_,
           Suit_t const& s_,
           Class_t const& pc,
           Score_t const& v,
           size_t m,
           util::Direction const& face)
    : Piece{b, pos_, s_, pc, v, m}
      , facing{face} {
}

std::unique_ptr<Piece> Pawn::clone(board::Board& board) {
    return std::make_unique<Pawn>(board, pos, suit, pclass, value, moves, facing);
}

void Pawn::moveUpdate(const Piece::Position_t& from, const Piece::Position_t& to) {
    if ((from.x == 6 && to.x == 7) || (from.x == 9 && to.x == 8) || (from.y == 6 && to.y == 7) || (from.y == 9 && to.y == 8)) {
        transform(Piece::Class_t{"Queen"});
    }
}

void Pawn::calcTrajectory() {
    if (pclass == "Queen") {
        //Queens can move infinitely in all eight directions
        using Dir = util::Direction;
        for (Dir d : {Dir::North, Dir::NorthEast, Dir::East, Dir::SouthEast, Dir::South, Dir::SouthWest, Dir::West,
                      Dir::NorthWest}) {
            Position_t t;
            for (signed i = 1; board.valid(t = Position_t(pos).move(d, i)); ++i) {
                addCapturing(t);
                if (!board.occupied(t)) {
                    addTrajectory(t);
                } else break; //can't jump over pieces
            }
        }
    } else {
        //Pawns can move 1 or 2 spaces forward on their first turn,
        //or only 1 space forward on any other turn.
        //On any turn they can move diagonally forward to capture,
        //but may not capture when moving straight forward.
        auto forward = Position_t(pos).move(facing);
        if (board.empty(forward)) {
            addTrajectory(forward);
            if (moves == 0) {
                auto forward2 = Position_t(pos).move(facing, 2);
                if (board.empty(forward2)) {
                    addTrajectory(forward2);
                }
            }
        }

        for (auto rotation : {+1, -1}) {
            Position_t diagr = Position_t(pos).move(Rotate(facing, rotation));
            if (board.occupied(diagr)) { //can capture diagonally forward-right
                addCapturing(diagr);
            }
        }
    }
}
}
}
