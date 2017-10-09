#include <iostream>
#include "BoardConfig.hpp"

namespace chesspp
{
namespace config
{
BoardConfig::BoardConfig(ResourcesConfig& res)
    : Configuration{"config/" + GAME_MODE + "/board.json"}
      , cell_width{reader()["board"]["cell width"]}
      , cell_height{reader()["board"]["cell height"]} {
    auto player_values = reader()["board"]["players"];
    for (size_t i = 0; i < player_values.length(); ++i) {
        Suit_t suit = player_values[i];
        players_.emplace_back(suit.at(0));
        suits_.emplace(suit.at(0), suit);
    }

    BoardSize_t board_height = reader()["board"]["height"];
    BoardSize_t board_width = reader()["board"]["width"];
    auto pieces = reader()["board"]["pieces"];
    auto suits = reader()["board"]["suits"];
    for (BoardSize_t r = 0; r < board_height; ++r) {
        for (BoardSize_t c = 0; c < board_width; ++c) {
            auto piece = pieces[r][c];
            if (piece.type() == util::JsonReader::Type::Null) {
                continue;
            }

            if (Piece_t(piece) == "Empty") {
                layout_[{c, r}] = std::nullopt;
            } else {
                Suit_t suit = suits[r][c];
                auto it = std::find_if(suits_.begin(), suits_.end(), [&](auto const& found_suit) {
                    return found_suit.second == suit;
                });

                if (it != suits_.end()) {
                    Player_t player = it->first;
                    layout_[{c, r}] = std::make_pair(piece, player);
                }
            }
        }
    }
}
}
}
