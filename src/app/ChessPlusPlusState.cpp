#include "ChessPlusPlusState.hpp"

#include "StartMenuState.hpp"
#include "GameOverState.hpp"

#include <iostream>

namespace chesspp
{
namespace app
{
ChessPlusPlusState::ChessPlusPlusState(Application& app_, sf::RenderWindow& disp)
    : AppState(disp)                    //can't use {}
      , app(app_)                         //can't use {}
      , res_config(app.resourcesConfig()) //can't use {}
      , board_config{res_config}
      , graphics{display, res_config, board_config}
      , board{board_config} {}

void ChessPlusPlusState::nextTurn() {
    selected = board.end();
    target = {127, 127};
    board.nextTurn();
}

board::Board::Pieces_t::iterator ChessPlusPlusState::find(board::Board::Position_t const& pos) const {
    return std::find_if(board.begin(), board.end(), [&](std::unique_ptr<piece::Piece> const& up) -> bool {
        return up->pos == pos;
    });
}

void ChessPlusPlusState::onRender() {
    graphics.drawBoard(board);
    if (selected != board.end()) {
        graphics.drawTrajectory(**selected);
    }
    if (board.valid(target)) {
        auto piece = find(target);
        if (piece != board.end()) {
            graphics.drawTrajectory(**piece, (*piece)->suit != board.turnSuit());
        }
    }
}

void ChessPlusPlusState::onKeyPressed(sf::Keyboard::Key key, bool /*alt*/, bool /*control*/, bool /*shift*/, bool /*system*/) {
    if (key == sf::Keyboard::Escape) { //Exit to menu screen
        return app.changeState<StartMenuState>(std::ref(app), std::ref(display));
    }
}
void ChessPlusPlusState::onMouseMoved(int x, int y) {
    target.x = static_cast<board::Board::Position_t::value_type>(x / board.config.cellWidth());
    target.y = static_cast<board::Board::Position_t::value_type>(y / board.config.cellHeight());
}
void ChessPlusPlusState::onLButtonPressed(int x, int y) {
}
void ChessPlusPlusState::onLButtonReleased(int x, int y) {
    if (selected == board.end()) {
        select();
    } else {
        if (!moveOrCapture())
            selected = board.end();
    }
}

bool ChessPlusPlusState::waitingForUser() {
    //return false;
    return board.turnSuit() == "Red";
}

void ChessPlusPlusState::aiMove() {
    std::vector<std::vector<config::BoardConfig::BoardSize_t>>
        points_vec{4, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};

    for (auto& points : points_vec)
        std::shuffle(points.begin(), points.end(), gen);

    for (auto x1 : points_vec[0]) {
        for (auto y1 : points_vec[1]) {
            target = {x1, y1};
            selected = board.end();
            if (select()) {
                for (auto x2 : points_vec[2]) {
                    for (auto y2 : points_vec[3]) {
                        target = {x2, y2};
                        if (moveOrCapture())
                            return;
                    }
                }
            }
        }
    }

    nextTurn();
}

bool ChessPlusPlusState::select() {
    if (!board.valid(target))
        return false;

    selected = find(target); //doesn't matter if board.end(), selected won't change then
    if (selected != board.end() && (*selected)->suit != board.turnSuit()) {
        selected = board.end(); //can't select enemy pieces
    }

    return selected != board.end();
}

bool ChessPlusPlusState::moveOrCapture() {
    if (!board.valid(target))
        return false;

    auto enemy = find(target);
    if (enemy == board.end() || (*enemy)->suit != (*selected)->suit) {
        if (capture() || move()) {
            if (board.winner()) {
                auto winner = board.winner().value();
                app.changeState<GameOverState>(std::ref(app), std::ref(display), winner->first,
                                               std::to_string(winner->second.score));
            } else {
                nextTurn();
            }
            return true;
        }
    }
    return false;
}

bool ChessPlusPlusState::capture() {
    auto it = std::find_if(board.pieceCapturings().begin(), board.pieceCapturings().end(),
                           [&](board::Board::Movements_t::value_type const& m) {
                               return m.first == selected && m.second == target;
                           });
    if (it != board.pieceCapturings().end()) {
        for (auto jt = board.pieceCapturables().begin(); jt != board.pieceCapturables().end(); ++jt) {
            if (jt->second == target) {
                return board.capture(selected, it, jt);
            }
        }
    }
    return false;
}

bool ChessPlusPlusState::move() {
    auto it = std::find_if(board.pieceTrajectories().begin(), board.pieceTrajectories().end(),
                           [&](board::Board::Movements_t::value_type const& m) {
                               return m.first == selected && m.second == target;
                           });
    return it != board.pieceTrajectories().end() && board.move(selected, it);
}

}
}
