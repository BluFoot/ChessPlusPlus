#ifndef chesspp_util_Position_HeaderPlusPlus
#define chesspp_util_Position_HeaderPlusPlus

#include "Utilities.hpp"
#include "Direction.hpp"

#include <type_traits>
#include <tuple>
#include <ostream>
#include <cstdint>

#include <boost/functional/hash.hpp>

namespace chesspp
{
namespace util
{
/**
 * Represents a two-dimensional point or position.
 * \tparam T coordinate type, must be a scalar type.
 */
class Position final
{
  public:
    std::uint8_t x, y; //intentionally public

    Position(std::uint8_t x_ = 0, std::uint8_t y_ = 0) noexcept
        : x{x_}
          , y{y_} {
    }

    Position& move(std::uint8_t xoff, std::uint8_t yoff) noexcept {
        x += xoff;
        y += yoff;
        return *this;
    }

    Position& move(Direction const& d, signed times = 1) noexcept {
        using D = Direction;
        for (signed i = 0; i < times; ++i) {
            //move forward
            switch (d) {
                case D::North:
                    --y;
                    break;
                case D::NorthEast:
                    ++x;
                    --y;
                    break;
                case D::East:
                    ++x;
                    break;
                case D::SouthEast:
                    ++x;
                    ++y;
                    break;
                case D::South:
                    ++y;
                    break;
                case D::SouthWest:
                    --x;
                    ++y;
                    break;
                case D::West:
                    --x;
                    break;
                case D::NorthWest:
                    --x;
                    --y;
                    break;
                default:
                    break;
            }
        }
        if (times < 0) {
            //move backward
            Position t = Position(0, 0).move(d, -times);
            move(-t.x, -t.y);
        }
        return *this;
    }

    friend bool operator==(Position const& a, Position const& b) noexcept {
        return a.x == b.x && a.y == b.y;
    }

    friend bool operator<(Position const& a, Position const& b) noexcept {
        return (a.x < b.x) || (a.x == b.x && a.y < b.y);
    }

    friend std::size_t hash_value(Position const& p) {
        std::size_t seed = 0;
        boost::hash_combine(seed, p.x);
        boost::hash_combine(seed, p.y);

        return seed;
    }
};

inline std::ostream& operator<<(std::ostream& os, Position const& p) noexcept {
    return os << '(' << static_cast<int>(p.x) << ", " << static_cast<int>(p.y) << ')';
}
}
}

namespace std
{
template<>
struct hash<chesspp::util::Position>
{
    std::size_t operator()(const chesspp::util::Position& p) const {
        boost::hash<chesspp::util::Position> hasher;
        return hasher(p);
    }
};
}

#endif
