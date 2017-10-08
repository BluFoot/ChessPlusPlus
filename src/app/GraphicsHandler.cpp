#include "GraphicsHandler.hpp"

#include "res/SfmlFileResource.hpp"
#include "AppState.hpp"

namespace chesspp
{
namespace app
{
GraphicsHandler::GraphicsHandler(sf::RenderWindow& disp, config::ResourcesConfig& resc, config::BoardConfig& bc)
    : display(disp)         //can't use {}
      , res_config(resc)      //can't use {}
      , board_config(bc)      //can't use {}
      , res(resc.resources()) //can't use {}
      , board{res.from_config<Texture_res>("board", "board")}
      , valid_move{res.from_config<Texture_res>("board", "valid move")}
      , enemy_move{res.from_config<Texture_res>("board", "enemy move")}
      , valid_capture{res.from_config<Texture_res>("board", "valid capture")}
      , enemy_capture{res.from_config<Texture_res>("board", "enemy capture")}
      , font{res.from_config<Font_res>("menu", "font")} {
    scores.setCharacterSize(35);
    scores.setStyle(sf::Text::Bold);
    scores.setPosition(0, 0);
    scores.setFont(font);
    scores.setColor(sf::Color::White);
}

void GraphicsHandler::drawBackground() {
    display.draw(board);
}
void GraphicsHandler::drawSpriteAtCell(sf::Sprite& s, std::size_t x, std::size_t y) {
    s.setPosition(x * board_config.cellWidth(), y * board_config.cellHeight());
    display.draw(s);
}
void GraphicsHandler::drawPiece(piece::Piece const& p) {
    sf::Sprite piece{res.from_config<Texture_res>("board", "pieces", p.suit, p.pclass)};
    drawSpriteAtCell(piece, p.pos.x, p.pos.y);
}
void GraphicsHandler::drawPieceAt(piece::Piece const& p, sf::Vector2i const& pos) {
    sf::Sprite piece{res.from_config<Texture_res>("board", "pieces", p.suit, p.pclass)};
    piece.setPosition(pos.x - (board_config.cellWidth() / 2), pos.y - (board_config.cellHeight() / 2));
    display.draw(piece);
}
void GraphicsHandler::drawTrajectory(piece::Piece const& p, bool enemy) {
    {
        auto& sprite = (enemy ? enemy_move : valid_move);
        for (auto const& it : p.board.pieceTrajectory(p)) {
            if (!p.board.occupied(it.second)) {
                drawSpriteAtCell(sprite, it.second.x, it.second.y);
            }
        }
    }
    {
        auto& sprite = (enemy ? enemy_capture : valid_capture);
        for (auto const& it : p.board.pieceCapturing(p)) {
            auto enemy_piece = p.board.find(it.second);
            if (enemy_piece != p.board.end() && (*enemy_piece)->suit != p.suit) {
                drawSpriteAtCell(sprite, it.second.x, it.second.y);
                auto jt = std::find_if(p.board.pieceTrajectories().begin(), p.board.pieceTrajectories().end(),
                                       [&](board::Board::Movements_t::value_type const& m) {
                                           return (*m.first)->pos == it.second;
                                       });
                if (jt != p.board.pieceTrajectories().end()) {
                    drawPiece(**(jt->first)); //redraw
                }
                break;
            }
        }
    }
}
void GraphicsHandler::drawBoard(board::Board const& b) {
    display.clear();

    drawBackground();

    for (auto const& pp : b) {
        drawPiece(*pp);
    }

    std::string scores_text;
    for (const auto& suit : b.player_order()) {
        scores_text += suit;
        scores_text += ": ";
        scores_text += std::to_string(b.players().at(suit).score);
        scores_text += "\n";
    }
    scores.setString(scores_text);
    display.draw(scores);
}
}
}
