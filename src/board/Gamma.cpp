#include "Gamma.hpp"

#include <chrono>

#include "piece/Piece.hpp"

namespace chesspp
{
namespace board
{
Gamma::Move Gamma::chooseMove(Board board) {
    auto start = std::chrono::system_clock::now();
    player = board.turn();

    auto best_move = calc(board, 3).first;

    auto duration = std::chrono::system_clock::now() - start;
    std::clog << "calc() " << board.turn() << ": " << best_move << " "
              << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
    return best_move;
}

std::pair<Gamma::Move, int> Gamma::calc(Board const& board, size_t depth) {
    if (depth == 0) {
        return {Move{}, evalBoard(board, player)};
    }

    Move best_move;
    int best_score = std::numeric_limits<int>::min();

    for (auto const& piece : board.pieces()) {
        if (piece->player != board.turn()) {
            continue;
        }

        auto const& from = piece->pos;
        for (auto const& targets : {piece->trajectories, piece->capturings}) {
            for (auto const& to : targets) {
                Move move{from, to};
                Board board_copy{board};
                board_copy.inputQuick(move);
                auto score = calc(board_copy, depth - 1).second;
                if (score > best_score) {
                    best_score = score;
                    best_move = move;
                }
            }
        }
    }

    return {best_move, best_score};
}

int Gamma::evalBoard(const Board& board, const Suit_t& turn) {
    int score = 0;
    for (auto const& player : board.playersDetails()) {
        score += player.second.score * (player.first == turn ? 1 : -1);
    }
    for (auto const& piece : board.pieces()) {
        if (piece->player == turn) {
            score += piece->value();
        }
    }
    return score;
}
}
}