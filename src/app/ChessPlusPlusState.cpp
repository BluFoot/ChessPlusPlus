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
        if (select())
            source = target;
    } else {
        if (board.input(source, target)) {
            nextTurn();
        } else {
            selected = board.end();
        }
    }
}

bool ChessPlusPlusState::waitingForUser() {
    return false;
    //return board.turn() == "Red";
}

void ChessPlusPlusState::aiMove() {
    std::vector<std::vector<config::BoardConfig::BoardSize_t>>
        points_vec{4, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};
    //for (auto& points : points_vec)
    //    std::shuffle(points.begin(), points.end(), gen);

    std::pair<board::Board::Position_t, board::Board::Position_t> best_move;
    int best_score = -1;
    auto starting_score = board.players().at(board.turn()).score;

    for (auto x1 : points_vec[0]) {
        for (auto y1 : points_vec[1]) {
            board::Board::Position_t from{x1, y1};
            if (!board.valid(from))
                continue;
            auto piece = find(from);
            if (piece == board.end() || (*piece)->suit != board.turn())
                continue;

            for (auto x2 : points_vec[2]) {
                for (auto y2 : points_vec[3]) {
                    board::Board::Position_t to{x2, y2};
                    if (!board.valid(to))
                        continue;

                    board::Board board_copy{board};
                    if (board_copy.input(from, to)) {
                        auto score = board_copy.players().at(board_copy.turn()).score;
                        std::clog << (score - starting_score) << std::endl;
                        if (score - starting_score > best_score) {
                            best_move = {source, target};
                        }
                    }
                }
            }
        }
    }

    std::clog << "best move: " << best_move.first << " to " << best_move.second << std::endl;
    board.input(best_move.first, best_move.second);
    nextTurn();
}

bool ChessPlusPlusState::select() {
    if (!board.valid(target))
        return false;

    selected = find(target); //doesn't matter if board.end(), selected won't change then
    if (selected != board.end() && (*selected)->suit != board.turn()) {
        selected = board.end(); //can't select enemy pieces
    }

    return selected != board.end();
}

}
}
