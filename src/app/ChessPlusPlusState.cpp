#include "ChessPlusPlusState.hpp"

#include "StartMenuState.hpp"
#include "GameOverState.hpp"

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

void ChessPlusPlusState::onRender() {
    graphics.drawBoard(board);
    if (selected) {
        graphics.drawTrajectory(selected.value());
    }
    auto piece = board.find(target);
    if (piece) {
        graphics.drawTrajectory(piece.value(), piece.value()->suit != board.turn());
    }
}

void ChessPlusPlusState::onKeyPressed(sf::Keyboard::Key key, bool /*alt*/, bool /*control*/, bool /*shift*/, bool /*system*/) {
    if (key == sf::Keyboard::Escape) { //Exit to menu screen
        return app.changeState<StartMenuState>(std::ref(app), std::ref(display));
    }
}
void ChessPlusPlusState::onMouseMoved(int x, int y) {
    target.x = static_cast<std::uint8_t>(x / board.config.cellWidth());
    target.y = static_cast<std::uint8_t>(y / board.config.cellHeight());
}
void ChessPlusPlusState::onLButtonPressed(int x, int y) {
}
void ChessPlusPlusState::onLButtonReleased(int x, int y) {
    if (!selected) {
        select();
    } else {
        if (board.input({selected.value()->pos, target})) {
            nextTurn();
        } else {
            select();
        }
    }
}

bool ChessPlusPlusState::waitingForUser() {
    //return true;
    //return false;
    return board.turn() == "Red";
}

void ChessPlusPlusState::aiMove() {
    auto move = gamma.chooseMove(board);
    if (move)
        board.input(move.value());
    nextTurn();
}

void ChessPlusPlusState::select() {
    selected = board.find(target);
    if (selected && selected.value()->suit != board.turn()) {
        selected = std::nullopt; //can't select enemy pieces
    }
}

void ChessPlusPlusState::nextTurn() {
    selected.reset();
    target = {127, 127};
    auto winner = board.winner();
    if (winner) {
        app.changeState<GameOverState>(app, display, winner.value()->first, std::to_string(winner.value()->second.score));
    }
}

}
}
