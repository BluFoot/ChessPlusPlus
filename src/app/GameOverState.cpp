#include "GameOverState.hpp"

#include "StartMenuState.hpp"
#include "res/SfmlFileResource.hpp"

namespace chesspp
{
namespace app
{
using Font_res = res::SfmlFileResource<sf::Font>;
using Texture_res = res::SfmlFileResource<sf::Texture>;

GameOverState::GameOverState(Application& app_,
                             sf::RenderWindow& display_,
                             const std::string& winner)
    : AppState(display_) //can't use {}
      , app(app_)          //can't use {}
      , menu_background{app.resourcesConfig().resources().from_config<Texture_res>("menu", "background")}
      , logo{app.resourcesConfig().resources().from_config<Texture_res>("menu", "title")}
      , font(app.resourcesConfig().resources().from_config<Font_res>("menu", "font")) //can't use {}
      , game_over_text{"Game Over!", 75}
      , winner_text{"Winner: " + winner, 75} {
    //Sets position at centered horizontally, down 10% vertically
    logo.setPosition((display.getSize().x / 2) - (logo.getLocalBounds().width / 2), display.getSize().y * 0.10);

    //Set up text
    game_over_text.setPosition(display.getSize().x / 2, display.getSize().y * 0.35);
    game_over_text.setFont(font);

    winner_text.setPosition(display.getSize().x / 2, display.getSize().y * 0.47);
    winner_text.setFont(font);
}

void GameOverState::onRender() {
    display.clear();
    display.draw(menu_background);
    display.draw(logo);
    display.draw(game_over_text);
    display.draw(winner_text);
}

void GameOverState::onKeyPressed(sf::Keyboard::Key key, bool alt, bool control, bool shift, bool system) {
    if (key == sf::Keyboard::Escape) { //Exit to menu screen
        return app.changeState<StartMenuState>(std::ref(app), std::ref(display));
    }
}
}
}
