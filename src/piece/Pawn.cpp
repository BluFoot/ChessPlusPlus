#include "Pawn.hpp"

namespace chesspp
{
namespace piece
{
static auto PawnRegistration = board::Board::registerPieceClass(Pawn::pclass_, [](board::Board& b,
                                                                                  board::Board::Position_t const& p,
                                                                                  board::Board::Suit_t const& s) -> board::Board::Pieces_t::value_type {
    auto d = util::Direction::None;
    std::istringstream {std::string(b.config.metadata("pawn facing", p.y, p.x))} >> d;
    return board::Board::Pieces_t::value_type(new Pawn(b, p, s, 0, d, false));
});

Pawn::Pawn(board::Board& b,
           Position_t const& pos_,
           Suit_t const& s_,
           size_t m,
           util::Direction const& face,
           bool transformed,
           Movements_t const& trajectories,
           Movements_t const& capturings)
    : Piece{b, pos_, s_, trajectories, capturings}
      , moves_{m}
      , facing_{face}
      , transformed_{transformed} {
}

std::unique_ptr<Piece> Pawn::clone(board::Board& board) {
    return std::make_unique<Pawn>(board, pos, suit, moves_, facing_, transformed_, trajectories, capturings);
}

void Pawn::moveUpdate(const Piece::Position_t& from, const Piece::Position_t& to) {
    if (!transformed_) {
        if ((from.x == 6 && to.x == 7) || (from.x == 9 && to.x == 8) || (from.y == 6 && to.y == 7)
            || (from.y == 9 && to.y == 8)) {
            transformed_ = true;
        }
    }
}

void Pawn::calcTrajectory() {
    if (transformed_) {
        //Queens can move infinitely in all eight directions
        using Dir = util::Direction;
        for (Dir d : {Dir::North, Dir::NorthEast, Dir::East, Dir::SouthEast, Dir::South, Dir::SouthWest, Dir::West, Dir::NorthWest}) {
            Position_t t;
            for (signed i = 1; board.valid(t = Position_t(pos).move(d, i)); ++i) {
                if (board.empty(t)) {
                    addTrajectory(t);
                } else {
                    auto piece = board.find(t);
                    if (piece && piece.value()->suit != suit) {
                        addCapturing(t);
                    }
                    break;
                }
            }
        }
    } else {
        //Pawns can move 1 or 2 spaces forward on their first turn,
        //or only 1 space forward on any other turn.
        //On any turn they can move diagonally forward to capture,
        //but may not capture when moving straight forward.
        auto forward = Position_t(pos).move(facing_);
        if (board.empty(forward)) {
            addTrajectory(forward);
            if (moves_ == 0) {
                auto forward2 = Position_t(pos).move(facing_, 2);
                if (board.empty(forward2)) {
                    addTrajectory(forward2);
                }
            }
        }

        for (auto rotation : {+1, -1}) {
            Position_t diag = Position_t(pos).move(Rotate(facing_, rotation));
            auto piece = board.find(diag);
            if (piece && piece.value()->suit != suit) {
                addCapturing(diag);
            }
        }
    }
}
}
}
