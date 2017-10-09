#include "Gamma.hpp"

#include <chrono>

#include "piece/Piece.hpp"

namespace chesspp
{
namespace board
{
std::optional<Gamma::Move> Gamma::chooseMove(Board const& board) {
    auto start = std::chrono::system_clock::now();
    player = board.turn();

    std::optional<Move> best_move;
    int best_score = std::numeric_limits<int>::min();

    for (auto const& move : legalMoves(board)) {
        board::Board board_copy{board};
        board_copy.inputQuick(move);
        auto score = calc(board_copy, 2);
        //std::clog << move << " = " << score << std::endl;
        if (score > best_score) {
            best_score = score;
            best_move = move;
        }
    }

    auto duration = std::chrono::system_clock::now() - start;
    std::clog << "calc() " << board.turn() << ": " << best_move.value() << " "
              << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
    return best_move;
}

int Gamma::calc(Board const& board, size_t depth) {
    if (depth == 0) {
        return evalBoard(board, player);
    }

    int best_score = std::numeric_limits<int>::min();

    for (auto const& move : legalMoves(board)) {
        board::Board board_copy{board};
        board_copy.inputQuick(move);
        auto score = calc(board_copy, depth - 1);
        if (score > best_score) {
            best_score = score;
        }
    }

    return best_score;
}

std::vector<Gamma::Move> Gamma::legalMoves(Board const& board) {
    std::vector<Move> moves;
    for (auto const& piece : board.pieces()) {
        if (piece->suit != board.turn())
            continue;

        for (auto const& trajectory : board.pieceTrajectory(piece.get())) {
            if (board.empty(trajectory.second)) {
                moves.emplace_back(Move{piece->pos, trajectory.second});
            }
        }

        for (auto const& capturing : board.pieceCapturing(piece.get())) {
            auto enemy = board.find(capturing.second);
            if (enemy && piece->suit != enemy.value()->suit) {
                moves.emplace_back(Move{piece->pos, capturing.second});
            }
        }
    }
    return moves;
}

int Gamma::evalBoard(const Board& board, const Suit_t& turn) {
    int score = 0;
    for (auto const& player : board.players()) {
        score += player.second.score * (player.first == turn ? 1 : -1);
    }
    for (auto const& piece : board.pieces()) {
        if (piece->suit == turn) {
            score += piece->value;
        }
    }
    return score;
}
}
}