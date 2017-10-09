#ifndef chesspp_config_BoardConfig_HeaderPlusPlus
#define chesspp_config_BoardConfig_HeaderPlusPlus

#include "Configuration.hpp"
#include "ResourcesConfig.hpp"
#include "util/Position.hpp"

#include <string>
#include <cstdint>
#include <utility>
#include <map>
#include <vector>
#include <optional>

namespace chesspp
{
namespace config
{
/**
 * \brief
 * Holds configuration values for the board (layout, metadata, etc)
 */
class BoardConfig: public Configuration
{
  public:
    using BoardSize_t = std::uint8_t;
    using CellSize_t = std::uint16_t;
    using Position_t = util::Position;
    using Piece_t = std::string;
    using Player_t = char;
    using Suit_t = std::string;
    using Score_t = int;

    using Players_t = std::vector<Player_t>;
    using Suits_t = std::unordered_map<Player_t, Suit_t>;
    using Square_t = std::optional<std::pair<Piece_t, Player_t>>;
    using Layout_t = std::map<Position_t, Square_t>;

  private:
    Players_t players_;
    Suits_t suits_;
    Layout_t layout_;
    CellSize_t cell_width, cell_height;

  public:
    /**
     * \brief
     * Loads board configuration from "config/chesspp/board.json"
     *
     * \param The ResourcesConfig to use for resource configuration.
     */
    explicit BoardConfig(ResourcesConfig& res);
    ~BoardConfig() override = default;

    Players_t const& players() const noexcept { return players_; }
    Suits_t const& suits() const noexcept { return suits_; }
    Layout_t const& layout() const noexcept { return layout_; }
    CellSize_t cellWidth() const noexcept { return cell_width; }
    CellSize_t cellHeight() const noexcept { return cell_height; }

    /**
     * \brief
     * Get metadata for the board pieces.
     *
     * Equivalent to calling `setting("board", "metadata", ...)`
     * \see setting()
     */
    template<typename... Args>
    util::JsonReader::NestedValue metadata(Args const& ... path) const {
        return reader.navigate("board", "metadata", path...);
    }
};
}
}

#endif
