#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <limits>
#include <cstdlib>

using namespace std;

constexpr int ROWS = 6;
constexpr int COLS = 7;
constexpr int PLAYER_PIECE = 1;
constexpr int AI_PIECE = 2;

using Board = array<array<int, COLS>, ROWS>;

Board create_board() {
    return {};
}

void drop_piece(Board& board, int row, int col, int piece) {
    board[row][col] = piece;
}

bool is_valid_location(const Board& board, int col) {
    return board[0][col] == 0;
}

int get_next_open_row(const Board& board, int col) {
    for (int r = ROWS - 1; r >= 0; --r) {
        if (board[r][col] == 0) {
            return r;
        }
    }
    return -1;
}

bool winning_move(const Board& board, int piece) {
    // checks vertically
    for (int c = 0; c < COLS - 3; ++c) {
        for (int r = 0; r < ROWS; ++r) {
            if (board[r][c] == piece && board[r][c + 1] == piece && board[r][c + 2] == piece && board[r][c + 3] == piece) {
                return true;
            }
        }
    }

    // checks horiztonally
    for (int c = 0; c < COLS; ++c) {
        for (int r = 0; r < ROWS - 3; ++r) {
            if (board[r][c] == piece && board[r + 1][c] == piece && board[r + 2][c] == piece && board[r + 3][c] == piece) {
                return true;
            }
        }
    }

    // checks diagonally to the top right
    for (int c = 0; c < COLS - 3; ++c) {
        for (int r = 3; r < ROWS; ++r) {
            vector<int> window;
            for (int i = 0; i < 4; ++i) {
                window.push_back(board[r - i][c + i]);
            }
            if (count(window.begin(), window.end(), piece) == 4) {
                return true;
            }
        }
    }

    // checks diagonally to the top left
    for (int c = 3; c < COLS; ++c) {
        for (int r = 3; r < ROWS; ++r) {
            vector<int> window;
            for (int i = 0; i < 4; ++i) {
                window.push_back(board[r - i][c - i]);
            }
            if (count(window.begin(), window.end(), piece) == 4) {
                return true;
            }
        }
    }

    return false;
}

int evaluate_window(const vector<int>& window, int piece) {
    int opponent_piece = (piece == AI_PIECE) ? PLAYER_PIECE : AI_PIECE;

    int score = 0;
    if (count(window.begin(), window.end(), piece) == 4) {
        score += 100;
    }
    else if (count(window.begin(), window.end(), piece) == 3 && count(window.begin(), window.end(), 0) == 1) {
        score += 5;
    }
    else if (count(window.begin(), window.end(), piece) == 2 && count(window.begin(), window.end(), 0) == 2) {
        score += 2;
    }

    if (count(window.begin(), window.end(), opponent_piece) == 3 && count(window.begin(), window.end(), 0) == 1) {
        score -= 4;
    }

    return score;
}

int score_position(const Board& board, int piece) {
    int score = 0;

    vector<int> center_array;
    for (int i = 0; i < ROWS; ++i) {
        center_array.push_back(board[i][COLS / 2]);
    }
    int center_count = count(center_array.begin(), center_array.end(), piece);
    score += center_count * 6;

    // checks horiztonally
    for (int r = 0; r < ROWS; ++r) {
        vector<int> row_array;
        for (int c = 0; c < COLS; ++c) {
            row_array.push_back(board[r][c]);
        }
        for (int c = 0; c < COLS - 3; ++c) {
            vector<int> window(row_array.begin() + c, row_array.begin() + c + 4);
            score += evaluate_window(window, piece);
        }
    }

    // checks vertically
    for (int c = 0; c < COLS; ++c) {
        vector<int> col_array;
        for (int r = 0; r < ROWS; ++r) {
            col_array.push_back(board[r][c]);
        }
        for (int r = 0; r < ROWS - 3; ++r) {
            vector<int> window(col_array.begin() + r, col_array.begin() + r + 4);
            score += evaluate_window(window, piece);
        }
    }

    // checks diagonally to the top right
    for (int r = 3; r < ROWS; ++r) {
        for (int c = 0; c < COLS - 3; ++c) {
            vector<int> window;
            for (int i = 0; i < 4; ++i) {
                window.push_back(board[r - i][c + i]);
            }
            score += evaluate_window(window, piece);
        }
    }

    // checks diagonally to the top left
    for (int r = 3; r < ROWS; ++r) {
        for (int c = 3; c < COLS; ++c) {
            vector<int> window;
            for (int i = 0; i < 4; ++i) {
                window.push_back(board[r - i][c - i]);
            }
            score += evaluate_window(window, piece);
        }
    }

    return score;
}

vector<int> get_valid_locations(const Board& board) {
    vector<int> valid_locations;
    for (int col = 0; col < COLS; ++col) {
        if (is_valid_location(board, col)) {
            valid_locations.push_back(col);
        }
    }
    return valid_locations;
}

bool is_terminal_node(const Board& board) {
    return winning_move(board, PLAYER_PIECE) || winning_move(board, AI_PIECE) || get_valid_locations(board).empty();
}

pair<int, int> minimax(Board board, int depth, int alpha, int beta, bool maximizing_player) {
    vector<int> valid_locations = get_valid_locations(board);
    bool is_terminal = is_terminal_node(board);

    if (depth == 0 || is_terminal) {
        if (is_terminal) {
            if (winning_move(board, AI_PIECE)) {
                return { -1, 10000000 };
            }
            else if (winning_move(board, PLAYER_PIECE)) {
                return { -1, -10000000 };
            }
            else {
                return { -1, 0 };
            }
        }
        else {
            return { -1, score_position(board, AI_PIECE) };
        }
    }

    if (maximizing_player) {
        int value = numeric_limits<int>::min();
        int column = valid_locations[rand() % valid_locations.size()];

        for (int col : valid_locations) {
            int row = get_next_open_row(board, col);
            Board b_copy = board;
            drop_piece(b_copy, row, col, AI_PIECE);
            int new_score = minimax(b_copy, depth - 1, alpha, beta, false).second;
            if (new_score > value) {
                value = new_score;
                column = col;
            }
            alpha = max(value, alpha);
            if (alpha >= beta) {
                break;
            }
        }

        return { column, value };
    }
    else {
        int value = numeric_limits<int>::max();
        int column = valid_locations[rand() % valid_locations.size()];

        for (int col : valid_locations) {
            int row = get_next_open_row(board, col);
            Board b_copy = board;
            drop_piece(b_copy, row, col, PLAYER_PIECE);
            int new_score = minimax(b_copy, depth - 1, alpha, beta, true).second;
            if (new_score < value) {
                value = new_score;
                column = col;
            }
            beta = min(value, beta);
            if (alpha >= beta) {
                break;
            }
        }

        return { column, value };
    }
}

void end_game() {
    cout << "Game Over\n";
}

void display_board(const Board& board) {
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            if (board[r][c] == 0) {
                cout << "- ";
            }
            else if (board[r][c] == PLAYER_PIECE) {
                cout << "X ";
            }
            else if (board[r][c] == AI_PIECE) {
                cout << "O ";
            }
        }
        cout << endl;
    }
    cout << endl;
}

int main() {
    Board board = create_board();
    bool game_over = false;
    int turn = rand() % 2;

    while (!game_over) {
        if (turn == 0) {
            // Player's turn
            display_board(board);
            int col;
            cout << "Player's Turn - Choose column (0-6): ";
            cin >> col;
            if (is_valid_location(board, col)) {
                int row = get_next_open_row(board, col);
                drop_piece(board, row, col, PLAYER_PIECE);
                if (winning_move(board, PLAYER_PIECE)) {
                    display_board(board);
                    cout << "PLAYER WINS!\n";
                    game_over = true;
                }
                turn = 1;
            }
        }
        else {
            // AI's turn
            int col, minimax_score;
            pair<int, int> result = minimax(board, 5, numeric_limits<int>::min(), numeric_limits<int>::max(), true);
            col = result.first;
            minimax_score = result.second;
            if (is_valid_location(board, col)) {
                int row = get_next_open_row(board, col);
                drop_piece(board, row, col, AI_PIECE);
                if (winning_move(board, AI_PIECE)) {
                    display_board(board);
                    cout << "AI WINS!\n";
                    game_over = true;
                }
                turn = 0;
            }
        }
    }

    return 0;
}
