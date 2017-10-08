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
            pieces.emplace(factory().at(slot.second.value().first)(*this, slot.first, slot.second.value().second));
        } else {
            positions.emplace(slot.first, std::nullopt);
        }
    }

    for (auto const& piece : pieces) {
        positions.emplace(piece->pos, piece.get());
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
    for (auto const& slot : board.positions) {
        if (!slot.second) {
            positions.emplace(slot.first, std::nullopt);
        }
    }

    for (auto const& piece : board.pieces) {
        pieces.insert(piece->clone(*this));
    }

    for (auto const& piece : pieces) {
        positions.emplace(piece->pos, piece.get());
        piece->calcTrajectory();
    }

    turn_ = std::find(player_order_.begin(), player_order_.end(), *board.turn_);
    if (board.winner_) {
        winner_ = players_.find((*board.winner_)->first);
    }
}

auto Board::find(Position_t const& pos) const noexcept -> Pieces_t::const_iterator {
    return std::find_if(begin(), end(), [&](std::unique_ptr<piece::Piece> const& up) -> bool {
        return up->pos == pos;
    });
}

bool Board::occupied(Position_t const& pos) const noexcept {
    auto it = positions.find(pos);
    return it != positions.end() && it->second;
}

bool Board::valid(Board::Position_t const& pos) const noexcept {
    return positions.find(pos) != positions.end();
}

void Board::addMovement(piece::Piece const& p, Position_t const& tile, Movements_t& m) {
    if (valid(tile)) {
        m.insert(Movements_t::value_type{&p, tile});
    }
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

    auto piece = find(move.from);
    if (piece == end()) {
        std::cerr << "no piece found at source position" << std::endl;
        return false;
    }

    if ((*piece)->suit != *turn_) {
        std::cerr << "can't move a piece that doesn't to you" << std::endl;
        return false;
    }

    auto enemy = find(move.to);
    if (enemy == end()) {
        return moveTo(piece, move.to);
    } else {
        return capture(piece, enemy, move.to);
    }
}

bool Board::inputQuick(Move const& move) {
    auto piece = find(move.from);
    auto enemy = find(move.to);

    if (enemy != end()) {
        kill((*piece)->suit, enemy);
    }

    movePiece(piece, move.to);
    update();
    return true;
}

void Board::movePiece(Pieces_t::iterator piece, Position_t const& to) {
    positions.at((*piece)->pos) = std::nullopt;
    positions.erase(to);
    positions.emplace(to, &(**piece));
    (*piece)->move(to);
}

bool Board::moveTo(Pieces_t::iterator piece, Position_t const& to) {
    if (occupied(to)) {
        std::cerr << "target position occupied" << std::endl;
        return false;
    }

    auto target = std::find_if(trajectories.begin(), trajectories.end(), [&](board::Board::Movements_t::value_type const& m) {
        return m.first == &(**piece) && m.second == to;
    });

    if (target == trajectories.end()) {
        std::cerr << "can't move that piece in that direction" << std::endl;
        return false;
    }

    movePiece(piece, to);
    update();
    return true;
}

bool Board::capture(Pieces_t::iterator piece, Pieces_t::iterator enemy, Position_t const& to) {
    if ((*enemy)->suit == (*piece)->suit) {
        std::cerr << "can't capture your own piece" << std::endl;
        return false;
    }

    auto capturing = std::find_if(capturings.begin(), capturings.end(), [&](board::Board::Movements_t::value_type const& m) {
        return m.first == &(**piece) && m.second == (*enemy)->pos;
    });

    if (capturing == capturings.end()) {
        std::cerr << "can't capture in that direction" << std::endl;
        return false;
    }

    kill((*piece)->suit, enemy);
    movePiece(piece, to);
    update();
    return true;
}

void Board::kill(const Board::Suit_t& suit, Pieces_t::iterator enemy) {
    if ((*enemy)->pclass == "King") {
        players_.at((*enemy)->suit).alive = false;
        std::clog << "Player " << ((*enemy)->suit) << " has been eliminated" << std::endl;
        if (1 == std::count_if(players_.begin(), players_.end(), [](const auto& player) { return player.second.alive; })) {
            winner_ = std::max_element(players_.begin(), players_.end(), [](const auto& player1, const auto& player2) {
                return player1.second.score < player2.second.score;
            });
            std::clog << "Game over, winner: " << winner_.value()->first << std::endl;
        }
    }

    players_.at(suit).score += (*enemy)->value;
    pieces.erase(enemy);
}

void Board::update() {
    trajectories.clear();
    capturings.clear();
    for (auto& p : pieces) {
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
