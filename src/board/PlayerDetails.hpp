#pragma once

#include "config/BoardConfig.hpp"

namespace chesspp
{
class PlayerDetails
{
  public:
    using Suit_t = config::BoardConfig::SuitClass_t;
    using Score_t = config::BoardConfig::Score_t;

    bool alive = true;
    bool checked = false;
    Score_t score = 0;
};
}

