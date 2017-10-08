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
    for (auto const& slot : conf.initialLayout()) {
        if (slot.second) {
            pieces.emplace(factory().at(slot.second.value().first)(*this, slot.first, slot.second.value().second));
        }
    }

    for (auto const& p : pieces) {
        p->makeTrajectory();
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
    turn_ = std::find(player_order_.begin(), player_order_.end(), *board.turn_);
    if (board.winner_)
        winner_ = players_.find((*board.winner_)->first);

    for (auto const& piece : board.pieces) {
        auto it = pieces.insert(piece->clone(*this));
    }
    for (auto const& p : pieces) {
        p->makeTrajectory();
    }
}

bool Board::occupied(Position_t const& pos) const noexcept {
    for (auto const& p : *this) {
        if (p->pos == pos) {
            return true;
        }
    }
    return false;
}

auto Board::find(Position_t const& pos) const noexcept -> Pieces_t::const_iterator {
    return std::find_if(begin(), end(), [&](std::unique_ptr<piece::Piece> const& up) -> bool {
        return up->pos == pos;
    });
}

auto Board::find(piece::Piece const& p) const noexcept -> Pieces_t::const_iterator {
    return std::find_if(begin(), end(), [&](Pieces_t::value_type const& v) {
        return v.get() == std::addressof(p);
    });
}

void Board::addMovement(piece::Piece const& p, Position_t const& tile, Movements_t& m) {
    if (valid(tile)) {
        auto it = find(p);
        if (it != end()) {
            m.insert(Movements_t::value_type(it, tile));
        }
    }
}

void Board::addTrajectory(piece::Piece const& p, const Board::Position_t& tile) {
    addMovement(p, tile, trajectories);
}

void Board::addCapturing(piece::Piece const& p, const Board::Position_t& tile) {
    addMovement(p, tile, capturings);
}

//void Board::addCapturable(piece::Piece const& p, const Board::Position_t& tile) {
//    addMovement(p, tile, capturables);
//}

auto Board::pieceMovement(piece::Piece const& p, Movements_t const& m) const noexcept -> MovementsRange {
    auto range = m.equal_range(find(p));
    return {{range.first, range.second}};
}

auto Board::pieceTrajectory(piece::Piece const& p) const noexcept -> MovementsRange {
    return pieceMovement(p, trajectories);
}
auto Board::pieceCapturing(piece::Piece const& p) const noexcept -> MovementsRange {
    return pieceMovement(p, capturings);
}
//auto Board::pieceCapturable(piece::Piece const& p) const noexcept -> MovementsRange {
//    return pieceMovement(p, capturables);
//}

bool Board::valid(const Board::Position_t& pos) const noexcept {
    return config.initialLayout().find(pos) != config.initialLayout().end();
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
        players_.at((*piece)->suit).score += (*enemy)->value;
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
        pieces.erase(enemy);
    }

    (*piece)->move(move.to);
    update(move.to);
    return true;
}

bool Board::moveTo(Pieces_t::iterator piece, Position_t const& to) {
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

    (*piece)->move(to);
    update(to);
    return true;
}

bool Board::capture(Pieces_t::iterator piece, Pieces_t::iterator enemy, Position_t const& to) {
    if ((*enemy)->suit == (*piece)->suit) {
        std::cerr << "can't capture your own piece" << std::endl;
        return false;
    }

    auto capturing = std::find_if(capturings.begin(), capturings.end(), [&](board::Board::Movements_t::value_type const& m) {
        return m.first == piece && m.second == (*enemy)->pos;
    });

    if (capturing == capturings.end()) {
        std::cerr << "can't capture in that direction" << std::endl;
        return false;
    }

    players_.at((*piece)->suit).score += (*enemy)->value;
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

    pieces.erase(enemy);
    (*piece)->move(to);
    update(to);
    return true;
}

void Board::update(Position_t const& pos) {
    trajectories.clear();
    capturings.clear();
    //capturables.clear();
    for (auto& p : pieces) {
        p->tick(pos);
        p->makeTrajectory();
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
