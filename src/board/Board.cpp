#include "Board.hpp"

#include "piece/Piece.hpp"

#include <iostream>

namespace chesspp
{
namespace board
{
Board::Board(config::BoardConfig const& conf)
    : config(conf)
      , player_order{conf.suits()}  //can't use {}
{
    for (auto const& slot : conf.initialLayout()) {
        if (slot.second) {
            pieces.emplace(factory().at(slot.second.value().first)(*this, slot.first, slot.second.value().second));
        }
    }

    for (auto const& p : pieces) {
        p->makeTrajectory();
    }

    for (const auto& suit : config.suits()) {
        players.emplace(suit, PlayerDetails{});
    }
    turn = player_order.begin();
}

void Board::nextTurn() {
    do {
        if (++turn == player_order.end()) {
            turn = player_order.begin();
        }
    } while (!players.at(*turn).alive);
}

bool Board::occupied(Position_t const& pos) const noexcept {
    for (auto const& p : *this) {
        if (p->pos == pos) {
            return true;
        }
    }
    return false;
}
auto Board::find(piece::Piece const& p) const noexcept -> Pieces_t::const_iterator {
    return std::find_if(std::begin(pieces), std::end(pieces), [&](Pieces_t::value_type const& v) {
        return v.get() == std::addressof(p);
    });
}

void Board::Movements::add(piece::Piece const& p, Position_t const& tile) {
    if (b.valid(tile)) {
        auto it = b.find(p);
        if (it != b.end()) {
            m.insert(Movements_t::value_type(it, tile));
        }
    }
}
void Board::Movements::remove(piece::Piece const& p, Position_t const& tile) {
    auto it = b.find(p);
    if (it != b.end()) {
        auto range = m.equal_range(it);
        for (auto jt = range.first; jt != range.second;) {
            if (jt->second == tile) {
                jt = m.erase(jt);
            } else ++jt;
        }
    }
}

auto Board::pieceTrajectory(piece::Piece const& p) noexcept -> MovementsRange {
    auto range = trajectories.equal_range(find(p));
    return {{range.first, range.second}};
}
auto Board::pieceCapturing(piece::Piece const& p) noexcept -> MovementsRange {
    auto range = capturings.equal_range(find(p));
    return {{range.first, range.second}};
}
auto Board::pieceCapturable(piece::Piece const& p) noexcept -> MovementsRange {
    auto range = capturables.equal_range(find(p));
    return {{range.first, range.second}};
}

void Board::update(Position_t const& pos) {
    trajectories.clear();
    capturings.clear();
    capturables.clear();
    for (auto& p : pieces) {
        p->tick(pos);
        p->makeTrajectory();
    }
}

bool Board::capture(Pieces_t::iterator source, Movements_t::const_iterator target, Movements_t::const_iterator capturable) {
    if (source == pieces.end()) {
        std::cerr << "source iterator of piece to capture with is invalid" << std::endl;
        return false;
    }
    if (source != target->first) {
        std::cerr << "target iterator does not match source iterator, source{" << **source << "}, target {" << **(target->first)
                  << "}" << std::endl;
        return false;
    }
    if (capturable->second != target->second) {
        std::cerr << "capturable may not be captured at target" << std::endl;
    }

    auto captured = capturable->first;
    players.at((*source)->suit).score += (*captured)->value;
    if ((*captured)->pclass == "King") {
        players.at((*captured)->suit).alive = false;
        std::clog << "Player " << ((*captured)->suit) << " has been eliminated" << std::endl;
        if (1 == std::count_if(players.begin(), players.end(), [](const auto& player) { return player.second.alive; })) {
            winner_ = std::max_element(players.begin(), players.end(), [](const auto& player1, const auto& player2) {
                return player1.second.score < player2.second.score;
            });
            std::clog << "Game over, winner: " << winner_.value()->first << std::endl;
        }
    }
    pieces.erase(capturable->first);
    //std::clog << "Capture: ";
    return move(source, target); //re-use existing code
}
bool Board::move(Pieces_t::iterator source, Movements_t::const_iterator target) {
    if (source == pieces.end()) {
        std::cerr << "source iterator of piece to move is invalid" << std::endl;
        return false;
    }
    if (target == trajectories.end() && target == capturings.end()) {
        std::cerr << "target iterator of piece to move to is invalid" << std::endl;
        return false;
    }
    if (source != target->first) {
        std::cerr << "target iterator does not match source iterator, source{" << **source << "}, target {" << **(target->first)
                  << "}" << std::endl;
        return false;
    }
    if (occupied(target->second)) {
        std::cerr << "target iterator to move to is occupied:" << std::endl;
        for (auto& p : pieces) {
            if (p->pos == target->second) {
                std::cerr << "\t" << *p << std::endl;
            }
        }
        return false;
    }

    //std::clog << "Moved piece at " << (*source)->pos << std::flush;
    auto t = target->second;
    (*source)->move(t);
    update(t);
    //std::clog << " to " << t << std::endl;
    return true;
}
bool Board::valid(const Board::Position_t& pos) const noexcept {
    return config.initialLayout().find(pos) != config.initialLayout().end()
        && pos.isWithin(Position_t::Origin(), {config.boardWidth(), config.boardHeight()});
}
}
}
