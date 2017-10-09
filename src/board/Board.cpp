#include "Board.hpp"

#include "piece/Piece.hpp"

#include <iostream>

namespace chesspp
{
namespace board
{
Board::Board(config::BoardConfig const& conf)
    : config(conf)
      , player_order_{conf.suits()} {
    for (auto const& slot : config.initialLayout()) {
        if (slot.second) {
            pieces_.emplace(factory().at(slot.second.value().first)(*this, slot.first, slot.second.value().second));
        } else {
            positions_.emplace(slot.first, std::nullopt);
        }
    }

    for (auto const& piece : pieces_) {
        positions_.emplace(piece->pos, piece.get());
        piece->calcTrajectory();
    }

    for (const auto& suit : config.suits()) {
        players_.emplace(suit, PlayerDetails{});
    }
    turn_ = player_order_.begin();
}

Board::Board(const Board& board)
    : config{board.config}
      , player_order_{board.player_order_}
      , players_{board.players_} {
    for (auto const& slot : board.positions_) {
        if (!slot.second) {
            positions_.emplace(slot.first, std::nullopt);
        }
    }

    for (auto const& piece : board.pieces_) {
        pieces_.insert(piece->clone(*this));
    }

    for (auto const& piece : pieces_) {
        positions_.emplace(piece->pos, piece.get());
        piece->calcTrajectory();
    }

    turn_ = std::find(player_order_.begin(), player_order_.end(), *board.turn_);
    if (board.winner_) {
        winner_ = players_.find((*board.winner_)->first);
    }
}

bool Board::occupied(Position_t const& pos) const noexcept {
    auto it = positions_.find(pos);
    return it != positions_.end() && it->second;
}

bool Board::valid(Board::Position_t const& pos) const noexcept {
    return positions_.find(pos) != positions_.end();
}

void Board::addMovement(piece::Piece const& p, Position_t const& tile, Movements_t& m) {
    m.insert(Movements_t::value_type{&p, tile});
}

void Board::addTrajectory(piece::Piece const& p, const Board::Position_t& tile) {
    addMovement(p, tile, trajectories);
}
void Board::addCapturing(piece::Piece const& p, const Board::Position_t& tile) {
    addMovement(p, tile, capturings);
}

auto Board::pieceMovement(piece::Piece const& p, Movements_t const& m) const noexcept -> MovementsRange {
    auto range = m.equal_range(&p);
    return {{range.first, range.second}};
}

auto Board::pieceTrajectory(piece::Piece const& p) const noexcept -> MovementsRange {
    return pieceMovement(p, trajectories);
}
auto Board::pieceCapturing(piece::Piece const& p) const noexcept -> MovementsRange {
    return pieceMovement(p, capturings);
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

    auto it = find(move.from);
    if (it == positions_.end() || !(it->second)) {
        std::cerr << "no piece found at source position" << std::endl;
        return false;
    }

    auto piece = it->second.value();
    if (piece->suit != *turn_) {
        std::cerr << "can't move a piece that doesn't to you" << std::endl;
        return false;
    }

    auto jt = find(move.to);
    if (jt == end() || !(jt->second)) {
        return moveTo(piece, move.to);
    } else {
        auto enemy = jt->second.value();
        return capture(piece, enemy, move.to);
    }
}

bool Board::inputQuick(Move const& move) {
    auto piece = find(move.from)->second.value();
    auto enemy_it = find(move.to);

    if (enemy_it != end() && enemy_it->second) {
        kill(piece->suit, enemy_it->second.value());
    }

    movePiece(piece, move.to);
    update();
    return true;
}

void Board::movePiece(piece::Piece* const piece, Position_t const& to) {
    positions_.at(piece->pos) = std::nullopt;
    positions_.erase(to);
    positions_.emplace(to, piece);
    piece->move(to);
}

bool Board::moveTo(piece::Piece* const piece, Position_t const& to) {
    if (occupied(to)) {
        std::cerr << "target position occupied" << std::endl;
        return false;
    }

    auto target = std::find_if(trajectories.begin(), trajectories.end(), [&](board::Board::Movements_t::value_type const& m) {
        return m.first == piece && m.second == to;
    });

    if (target == trajectories.end()) {
        std::cerr << "can't move that piece in that direction" << std::endl;
        return false;
    }

    movePiece(piece, to);
    update();
    return true;
}

bool Board::capture(piece::Piece* const piece, const piece::Piece* const enemy, Position_t const& to) {
    if (enemy->suit == piece->suit) {
        std::cerr << "can't capture your own piece" << std::endl;
        return false;
    }

    auto capturing = std::find_if(capturings.begin(), capturings.end(), [&](board::Board::Movements_t::value_type const& m) {
        return m.first == piece && m.second == enemy->pos;
    });

    if (capturing == capturings.end()) {
        std::cerr << "can't capture in that direction" << std::endl;
        return false;
    }

    kill(piece->suit, enemy);
    movePiece(piece, to);
    update();
    return true;
}

void Board::kill(const Board::Suit_t& suit, const piece::Piece* const enemy) {
    if (enemy->pclass == "King") {
        players_.at(enemy->suit).alive = false;
        std::clog << "Player " << (enemy->suit) << " has been eliminated" << std::endl;
        if (1 == std::count_if(players_.begin(), players_.end(), [](const auto& player) { return player.second.alive; })) {
            winner_ = std::max_element(players_.begin(), players_.end(), [](const auto& player1, const auto& player2) {
                return player1.second.score < player2.second.score;
            });
            std::clog << "Game over, winner: " << winner_.value()->first << std::endl;
        }
    }

    players_.at(suit).score += enemy->value;
    auto enemy_it = std::find_if(pieces_.begin(), pieces_.end(), [&](auto const& p) { return p.get() == enemy; });
    pieces_.erase(enemy_it);
}

void Board::update() {
    trajectories.clear();
    capturings.clear();
    for (auto& p : pieces_) {
        p->calcTrajectory();
    }
    nextTurn();
}

void Board::nextTurn() {
    do {
        if (++turn_ == player_order_.end()) {
            turn_ = player_order_.begin();
        }
    } while (!players_.at(*turn_).alive);
}
}
}
