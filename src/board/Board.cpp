#include "Board.hpp"

#include "piece/Piece.hpp"

#include <iostream>

namespace chesspp
{
namespace board
{
Board::Board(config::BoardConfig const& conf)
    : config(conf)
      , players_{conf.players()} {
    auto layout = config.layout();
    for (Coord_t c = 0; c < BOARD_SIZE; ++c) {
        for (Coord_t r = 0; r < BOARD_SIZE; ++r) {
            auto slot = layout.find({c, r});
            if (slot == layout.end()) {
                positions_[c][r].state = Square::INVALID;
            } else if (slot->second.has_value()) {
                auto type = slot->second.value().first;
                auto pos = slot->first;
                auto player = slot->second.value().second;
                pieces_.emplace_back(factory().at(type)(*this, pos, player));
                positions_[c][r].occupy(pieces_.back().get());
            } else {
                positions_[c][r].state = Square::EMPTY;
            }
        }
    }

    for (auto const& piece : pieces_) {
        piece->makeTrajectory();
    }

    for (const auto& suit : config.players()) {
        players_details_.emplace(suit, PlayerDetails{});
    }
    turn_ = players_.begin();
}

Board::Board(const Board& board)
    : config{board.config}
      , players_{board.players_}
      , players_details_{board.players_details_} {
    pieces_.reserve(board.pieces_.size());

    for (Coord_t c = 0; c < BOARD_SIZE; ++c) {
        for (Coord_t r = 0; r < BOARD_SIZE; ++r) {
            auto const& square = board.positions_[c][r];
            positions_[c][r].state = square.state;
            if (square.occupied()) {
                pieces_.emplace_back(square.piece->clone(*this));
                positions_[c][r].piece = pieces_.back().get();
            }
        }
    }

    turn_ = std::find(players_.begin(), players_.end(), *board.turn_);
    if (board.winner_) {
        winner_ = players_details_.find((*board.winner_)->first);
    }
}

std::optional<Board::Piece_cpt> Board::find(Board::Position_t const& pos) const noexcept {
    if (!inBounds(pos))
        return std::nullopt;
    auto const& square = positions_[pos.x][pos.y];
    if (square.occupied())
        return square.piece;
    else
        return std::nullopt;
}

bool Board::inBounds(Board::Position_t const& pos) const noexcept {
    return pos.x < positions_.size() && pos.y < positions_.size();
}

bool Board::valid(Board::Position_t const& pos) const noexcept {
    return inBounds(pos) && positions_[pos.x][pos.y].valid();
}

bool Board::empty(Position_t const& pos) const noexcept {
    return inBounds(pos) && positions_[pos.x][pos.y].empty();
}

bool Board::occupied(Position_t const& pos) const noexcept {
    return inBounds(pos) && positions_[pos.x][pos.y].occupied();
}

bool Board::input(Move const& move) {
    if (!valid(move.from)) {
        std::cerr << "invalid source position" << std::endl;
        return false;
    }

    if (!valid(move.to)) {
        std::cerr << "invalid target position" << std::endl;
        return false;
    }

    auto piece = find(move.from);
    if (!piece) {
        std::cerr << "no piece found at source position" << std::endl;
        return false;
    }

    if (piece.value()->player != *turn_) {
        std::cerr << "can't move a piece that doesn't belong to you" << std::endl;
        return false;
    }

    auto enemy = find(move.to);
    if (!enemy) {
        return moveTo(piece.value(), move.to);
    } else {
        return capture(piece.value(), enemy.value(), move.to);
    }
}

void Board::inputQuick(Move const& move) {
    auto piece = find(move.from);
    auto enemy = find(move.to);

    if (enemy) {
        kill(piece.value()->player, enemy.value());
    }

    movePiece(piece.value(), move.to);
    update();
}

void Board::movePiece(Piece_cpt piece, Position_t const& to) {
    positions_[piece->pos.x][piece->pos.y].state = Square::EMPTY;
    positions_[to.x][to.y].occupy(piece);
    piece->move(to);
}

bool Board::moveTo(Piece_cpt piece, Position_t const& to) {
    if (occupied(to)) {
        std::cerr << "target position occupied" << std::endl;
        return false;
    }

    if (std::find(piece->trajectories.begin(), piece->trajectories.end(), to) == piece->trajectories.end()) {
        std::cerr << "can't move that piece in that direction" << std::endl;
        return false;
    }

    movePiece(piece, to);
    update();
    return true;
}

bool Board::capture(Piece_cpt piece, Piece_cpt enemy, Position_t const& to) {
    if (enemy->player == piece->player) {
        std::cerr << "can't capture your own piece" << std::endl;
        return false;
    }

    if (std::find(piece->capturings.begin(), piece->capturings.end(), to) == piece->capturings.end()) {
        std::cerr << "can't capture in that direction" << std::endl;
        return false;
    }

    kill(piece->player, enemy);
    movePiece(piece, to);
    update();
    return true;
}

void Board::kill(const Board::Player_t& suit, const Piece_cpt enemy) {
    if (enemy->pclass() == "King") {
        players_details_.at(enemy->player).alive = false;
        std::clog << "Player " << (enemy->player) << " has been eliminated" << std::endl;
        if (1 == std::count_if(players_details_.begin(), players_details_.end(), [](const auto& player) { return player.second.alive; })) {
            winner_ = std::max_element(players_details_.begin(), players_details_.end(), [](const auto& player1, const auto& player2) {
                return player1.second.score < player2.second.score;
            });
            std::clog << "Game over, winner: " << winner_.value()->first << std::endl;
        }
    }

    players_details_.at(suit).score += enemy->value();
    auto enemy_it = std::find_if(pieces_.begin(), pieces_.end(), [&](auto const& p) { return p.get() == enemy; });
    pieces_.erase(enemy_it);
}

void Board::update() {
    for (auto& p : pieces_) {
        p->makeTrajectory();
    }
    nextTurn();
}

void Board::nextTurn() {
    do {
        if (++turn_ == players_.end()) {
            turn_ = players_.begin();
        }
    } while (!players_details_.at(*turn_).alive);
}
}
}
