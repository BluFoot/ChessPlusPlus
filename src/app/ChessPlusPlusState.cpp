#include "ChessPlusPlusState.hpp"

#include "StartMenuState.hpp"
#include "GameOverState.hpp"

#include <iostream>

namespace chesspp
{
namespace app
{
using Font_res = res::SfmlFileResource<sf::Font>;

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
}

void ChessPlusPlusState::onRender() {
    graphics.drawBoard(board);
    if (selected != board.end()) {
        graphics.drawTrajectory(**selected);
    }
    if (board.valid(target)) {
        auto piece = board.find(target);
        if (piece != board.end()) {
            graphics.drawTrajectory(**piece, (*piece)->suit != board.turn());
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
        if (board.input((*selected)->pos, target)) {
            nextTurn();
        } else {
            select();
        }
    }
}

bool ChessPlusPlusState::waitingForUser() {
    //return false;
    return board.turn() == "Red";
}

void ChessPlusPlusState::aiMove() {
    bool best_is_move = true;
    int best_diff = -1;
    std::pair<board::Board::Position_t, board::Board::Position_t> best_move;
    auto starting_score = board.players().at(board.turn()).score;
    auto turn = board.turn();

    for (auto piece = board.begin(); piece != board.end(); ++piece) {
        if ((*piece)->suit != board.turn())
            continue;

        auto trajectories = board.pieceTrajectory(**piece);
        for (auto const& trajectory : trajectories) {
            if (board.occupied(trajectory.second))
                continue;

            auto from = (*piece)->pos;
            auto to = trajectory.second;
            board::Board board_copy{board};
            if (board_copy.moveQuick(from, to)) {
                auto score = board_copy.players().at(turn).score;
                if (score - starting_score > best_diff) {
                    best_is_move = true;
                    best_diff = score - starting_score;
                    best_move = {from, to};
                }
            }
        }

        auto capturings = board.pieceCapturing(**piece);
        for (auto const& capturing : capturings) {
            auto enemy = board.find(capturing.second);
            if (enemy == board.end() || (*piece)->suit == (*enemy)->suit)
                continue;

            auto capturables = board.pieceCapturable(**enemy);
            auto capturable = std::find_if(capturables.begin(), capturables.end(), [&](auto const& capturable_) {
                return capturable_.first == enemy && capturable_.second == (*enemy)->pos;
            });
            if (capturable == capturables.end())
                continue;

            auto from = (*piece)->pos;
            auto to = capturing.second;
            board::Board board_copy{board};
            if (board_copy.captureQuick(from, to)) {
                auto score = board_copy.players().at(turn).score;
                if (score - starting_score > best_diff) {
                    best_is_move = false;
                    best_diff = score;
                    best_move = {from, to};
                }
            }
        }
    }

    if (best_is_move) {
        board.moveQuick(best_move.first, best_move.second);
    } else {
        board.captureQuick(best_move.first, best_move.second);
    }
    nextTurn();
}

void ChessPlusPlusState::select() {
    selected = board.find(target); //doesn't matter if board.end(), selected won't change then
    if (selected != board.end() && (*selected)->suit != board.turn()) {
        selected = board.end(); //can't select enemy pieces
    }
}

}
}
