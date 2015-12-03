#include "include/Game.h"

using namespace std;

const int num_rounds = 3;
loc hounds_dirs[] = {
    loc(1, 0), loc(-1, 0), // vertical
    loc(0, 1), // forward
    loc(-1, 1), loc(1, 1), // diagonally (left to right)
};
loc hare_dirs[] = {
    loc(-1, 0), loc(1, 0), // vertical
    loc(0, -1), loc(0, 1), // horizontal
    loc(-1, 1), loc(1, 1), // diagonally (left to right)
    loc(-1, -1), loc(1, -1) // diagonally (right to left)
};

player board::get(loc pos) {
    return vec[pos.x][pos.y];
};
void board::setPos(loc pos, player p) {
    vec[pos.x][pos.y] = p;
}

HHGame::HHGame() {
    rounds = 0;
    roundIsOver = false;
    hounds_score = 0;
    hare_score = 0;
    turns = 0;
    hounds_vertical_moves = 0;
    init_state();
}
// new round or new game
void HHGame::restart(void)
{
    rounds++;
    roundIsOver = false;
    turns = 0;
    hounds_vertical_moves = 0;
    if (gameOver()) {
        rounds %= 3; // since gameOver() is true, rounds == 3 so rounds % 3 == 0
        hounds_score = 0;
        hare_score = 0;
    }
    init_state();
}
void HHGame::init_state()
{
    vector<vector<player>> vec = {
        { HOUNDS, NO_PLAYER, NO_PLAYER },
        { HOUNDS, NO_PLAYER, NO_PLAYER, NO_PLAYER, HARE },
        { HOUNDS, NO_PLAYER, NO_PLAYER }
    };

    curr_state._board = board(vec);
    curr_state._player = HOUNDS; // they always start
}
void HHGame::switch_player()
{
    curr_state._player = (get_player() == HOUNDS)?HARE:HOUNDS;
}

player HHGame::get_player()
{
    return curr_state._player;
}

board HHGame::get_board()
{
    // check if the hounds trap the Hare
    return curr_state._board;
}
bool HHGame::roundOver() {
    return roundIsOver;
}
bool HHGame::gameOver() {
    return roundIsOver && rounds == 2;
}
int HHGame::get_hare_score() {
    return hare_score;
}
int HHGame::get_hounds_score() {
    return hounds_score;
}
int HHGame::get_round() {
    return rounds + 1; // since 0 <= rounds
}

loc HHGame::get_hare() {
    board b = get_board();
    int rows = 3, cols;
    loc pos;
    player p;
    for (int r = 0; r < rows; r++) {
        if (r % 2 == 0) cols = 3;
        else cols = 5;
        for (int c = 0; c < cols; c++) {
            pos = loc(r, c);
            p = b.get(pos);
            if (p == HARE) {
                return pos;
            }
        }
    }
}
loc *HHGame::get_hounds()
{
    board b = get_board();
    int hounds_found=0, rows = 3, cols;
    loc pos, *hounds = new loc[3];
    player p;
    for (int r = 0; r < rows; r++) {
        if (r % 2 == 0) cols = 3;
        else cols = 5;
        for (int c = 0; c < cols; c++) {
            pos = loc(r, c);
            p = b.get(pos);
            if (p == HOUNDS) {
                hounds[hounds_found++] = pos;
                if (hounds_found == 3) break;
            }
        }
    }
    return hounds;
}
status *HHGame::get_status()
{
    // check if hare wins because Hounds stalled
    if (hounds_vertical_moves == 10) {
        Over *over = new Over(new Winner(HARE, STALL));
        return over;
    }
    // check if hare escapes (gets to the left of all the Hounds)
    bool hare_escapes = true;
    loc hare = get_hare();
    if (hare.x % 2 == 0) hare.y++;
    loc *hounds = get_hounds();
    for (int i = 0; i < 3; i++) {
        if (hounds[i].x % 2 == 0) hounds[i].y++;
        if (hare.y > hounds[i].y) {
            hare_escapes = false;
            break;
        }
    }
    delete[] hounds;
    if (hare.x % 2 == 0) hare.y--; // reset the y coordinate
    if (hare_escapes) {
        Over *over = new Over(new Winner(HARE, ESCAPE));
        return over;
    }
    // hounds win if hare cannot escape
    vector<loc> hare_possible_moves = neighbors(HARE, hare, hare_dirs);
    if (hare_possible_moves.size() == 0) {
        Over *over = new Over(new Winner(HOUNDS, TRAP));
        return over;
    }
    In_Play *in_play = new In_Play();
    return in_play;
}

bool in_range(loc pos)
{
    int x = pos.x;
    int y = pos.y;
    // since there are three rows in board
    if (0 <= x && x < 3) {
        // this represents the three middle nodes
        // in the 3x5 board that are seen
        if (x % 2 == 0) { return (1 <= y && y < 4); }
        else { return (0 <= y && y < 5); } // middle row has 5 columns
    }
    return false;
}

vector<loc> HHGame::neighbors(player p, loc player_piece, loc dirs[])
{
    int x = player_piece.x;
    int y = player_piece.y;
    int dirX, dirY, _size;
    loc dir, pos;
    board b = get_board();
    vector<loc> vec;
    if (p == HOUNDS) {
        // on the board, players can only move diagonally on even
        // columns
        _size = (y % 2 == 0)?5:3;
    } else {
        _size = (y % 2 == 0)?8:4;
    }
    // map from first and third row coords to the coords if the board was a
    // 3x5 shap
    // (0,0)->(0, 1) (0, 1)->(0, 2) (0, 2)->(0, 3)
    // (2, 0)->(2, 1) (2, 1)->(2, 2) (2, 2)->(2, 3)
    if (x % 2 == 0) y++;
    for (int i=0; i < _size; i++) {
        dir = dirs[i];
        dirX = dir.x;
        dirY = dir.y;
        pos = loc(x + dirX, y + dirY);
        if (in_range(pos)) {
            if (pos.x % 2 == 0) pos.y--;
            if (b.get(pos) == NO_PLAYER) {
                vec.push_back(pos);
            }
        }
    }
    return vec;
}

bool HHGame::is_legal_move(MovePlayer *m)
{
    loc from = m->from_loc;
    loc to = m->to_loc;

    board b = curr_state._board;
    player p = curr_state._player;
    // check if the move's "from" location holds the current player
    if (b.get(from) == p) {
        // check the move's "to" location is a neighbor of the player at the "from" location
        vector<loc> possible_moves = neighbors(p, from, (p == HOUNDS)?hounds_dirs:hare_dirs);
        int _size = possible_moves.size();
        for (int i = 0; i < _size; i++)
            if (possible_moves[i].equals(to)) return true;
    }
    return false;
}

void HHGame::make_move(MovePlayer *m)
{
    if (!is_legal_move(m)) { throw IllegalMoveError(); return; }
    loc from = m->from_loc;
    loc to = m->to_loc;
    int dirX = to.x - from.x;
    int dirY = ((to.x % 2 == 0)?(to.y + 1):to.y) -
                ((from.x % 2 == 0)?(from.y + 1):from.y);
    if (curr_state._player == HOUNDS) {
        if ((dirX == -1 || dirX == 1) && dirY == 0) {
            hounds_vertical_moves += 1;
        } else {
            hounds_vertical_moves = 0;
        }
    }
    board b = get_board();
    b.setPos(from, NO_PLAYER);
    b.setPos(to, get_player());
    curr_state._board = b;

    // update game
    status *curr_status = get_status();
    if (curr_status->status_to_string().substr(0, 4) == "Over") {
        Over *over = (Over *)(curr_status);
        Winner *winner = (Winner *)(over->_outcome);
        if (winner->_player == HARE) {
            hare_score++;
        } else {
            hounds_score++;
        }
        roundIsOver = true;
    } else { // In_play
        switch_player();
    }
    delete curr_status;
}
