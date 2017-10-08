#include "Gamma.hpp"
#include "piece/Piece.hpp"

namespace chesspp
{
namespace board
{
std::optional<Board::Move> Gamma::calc(Board const& board) {
    int best_diff = -1;
    std::optional<Board::Move> best_move;
    auto starting_score = board.players().at(board.turn()).score;
    auto turn = board.turn();

    auto legal_moves = legalMoves(board);

    for (auto const& move : legal_moves) {
        board::Board board_copy{board};
        if (board_copy.inputQuick(move)) {
            auto score = board_copy.players().at(turn).score;
            if (score - starting_score > best_diff) {
                best_diff = score - starting_score;
                best_move = move;
            }
        }
    }

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
            if (enemy == board.end() || (*piece)->suit == (*enemy)->suit)
                continue;

            auto capturables = board.pieceCapturable(**enemy);
            auto capturable = std::find_if(capturables.begin(), capturables.end(), [&](auto const& capturable_) {
                return capturable_.first == enemy && capturable_.second == (*enemy)->pos;
            });
            if (capturable != capturables.end()) {
                moves.emplace_back(Move{(*piece)->pos, capturing.second});
            }
        }
    }
    return moves;
}
}
}