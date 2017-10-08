#include "Gamma.hpp"

#include <chrono>

#include "piece/Piece.hpp"

namespace chesspp
{
namespace board
{
std::optional<Board::Move> Gamma::calc(Board const& board) {
    auto start = std::chrono::system_clock::now();
    std::optional<Board::Move> best_move;

    auto turn = board.turn();
    auto starting_score = evalBoard(board, turn);
    int best_diff = std::numeric_limits<int>::min();

    for (auto const& move : legalMoves(board)) {
        board::Board board_copy{board};
        if (board_copy.inputQuick(move)) {
            auto score = evalBoard(board_copy, turn);
            if (score - starting_score > best_diff) {
                best_diff = score - starting_score;
                best_move = move;
            }
        }
    }

    auto duration = std::chrono::system_clock::now() - start;
    std::clog << "calc() for " << turn << " took " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()
              << "ms" << std::endl;
    return best_move;
}

std::vector<Gamma::Move> Gamma::legalMoves(Board const& board) {
    std::vector<Move> moves;
    for (auto piece = board.begin(); piece != board.end(); ++piece) {
        if ((*piece)->suit != board.turn())
            continue;

        auto trajectories = board.pieceTrajectory(**piece);
        for (auto const& trajectory : trajectories) {
            if (!board.occupied(trajectory.second)) {
                moves.emplace_back(Move{(*piece)->pos, trajectory.second});
            }
        }

        auto capturings = board.pieceCapturing(**piece);
        for (auto const& capturing : capturings) {
            auto enemy = board.find(capturing.second);
            if (enemy != board.end() && (*piece)->suit != (*enemy)->suit) {
                moves.emplace_back(Move{(*piece)->pos, capturing.second});
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
    for (auto const& piece : board) {
        if (piece->suit == turn) {
            score += piece->value;
        }
    }
    return score;
}
}
}