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
#include <experimental/optional>

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
    using PieceClass_t = std::string;
    using SuitClass_t = std::string;
    using Score_t = int;
    using Square_t = std::optional<std::pair<PieceClass_t, SuitClass_t>>;
    using Layout_t = std::map<Position_t, Square_t>;
    using Textures_t = std::map<BoardConfig::SuitClass_t, std::map<BoardConfig::PieceClass_t, std::string>>;
    using Suits_t = std::vector<SuitClass_t>;

  private:
    BoardSize_t board_width, board_height;
    Suits_t players;
    CellSize_t cell_width, cell_height;
    Layout_t layout;

  public:
    /**
     * \brief
     * Loads board configuration from "config/chesspp/board.json"
     *
     * \param The ResourcesConfig to use for resource configuration.
     */
    BoardConfig(ResourcesConfig& res);
    virtual ~BoardConfig() = default;

    BoardSize_t boardWidth() const noexcept { return board_width; }
    BoardSize_t boardHeight() const noexcept { return board_height; }
    Suits_t suits() const noexcept { return players; }
    Layout_t const& initialLayout() const noexcept { return layout; }
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
