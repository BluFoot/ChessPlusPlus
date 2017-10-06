#include "app/Application.hpp"
#include "app/StartMenuState.hpp"

#include <iostream>
#include <app/ChessPlusPlusState.hpp>

/**
 * \brief
 * Entry point of the application.
 * 
 * Sets up the application window and then passes control to chesspp::app::StartMenuState
 * 
 * \param nargs argument count, not used.
 * \param args arguments, not used.
 */
int main(int nargs, char const* const* args) {
#ifdef CHESSPP_REDIRECT_OUTPUT
    chesspp::enableRedirection();
#endif

    try {
        sf::RenderWindow disp{sf::VideoMode(960, 960), "ChessPlusPlus", sf::Style::Close};
        chesspp::app::Application app{disp};
        app.changeState<chesspp::app::ChessPlusPlusState>(std::ref(app), std::ref(disp));
        return app.execute();
    } catch (std::exception& e) {
        std::clog << typeid(e).name() << " caught in main: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    std::clog << "Exiting normally from main" << std::endl;
}
