#include "BoardConfig.hpp"

namespace chesspp
{
namespace config
{
BoardConfig::BoardConfig(ResourcesConfig& res)
    : Configuration{"config/" + GAME_MODE + "/board.json"},
      board_width{reader()["board"]["width"]},
      board_height{reader()["board"]["height"]},
      cell_width{reader()["board"]["cell width"]},
      cell_height{reader()["board"]["cell height"]} {
    auto player_values = reader()["board"]["players"];
    for (size_t i = 0; i < player_values.length(); ++i)
        players.emplace_back(player_values[i]);

    auto pieces = reader()["board"]["pieces"];
    auto suits = reader()["board"]["suits"];
    for (BoardSize_t r = 0; r < board_height; ++r) {
        for (BoardSize_t c = 0; c < board_width; ++c) {
            auto piece = pieces[r][c];
            auto suit = suits[r][c];
            if (piece.type() != util::JsonReader::Type::Null) //it is OK if suit is null
            {
                layout[{c, r}] = std::make_pair<PieceClass_t, SuitClass_t>(piece, suit);
            }
        }
    }
}
}
}
