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
    turns = 0;
    hounds_vertical_moves = 0;
    if (gameOver()) {
        rounds %= 3; // since gameOver() is true, rounds % 3 == 0
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
    cout << "switching player...";
    curr_state._player = (get_player() == HOUNDS)?HARE:HOUNDS;
    cout << "current player is now... " << ((get_player() == HOUNDS)?"HOUNDS":"HARE")<<endl;
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

bool HHGame::gameOver() {
    return (rounds % 3) == 3;
}
int HHGame::get_hare_score() {
    return hare_score;
}
int HHGame::get_hounds_score() {
    return hounds_score;
}
int HHGame::get_round() {
    return rounds;
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
    loc pos, hounds[3];
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
status HHGame::get_status()
{
    // check if hare wins because Hounds stalled
    if (hounds_vertical_moves == 10) {
        return Over(Winner(HARE, STALL));
    }
    // check if hare escapes (gets to the left of all the Hounds)
    bool hare_escapes = true;
    loc hare = get_hare();
    loc *hounds = get_hounds();
    for (int i = 0; i < 3; i++) {
        if (hare.x > hounds[i].x) {
            hare_escapes = false;
            break;
        }
    }
    if (hare_escapes) {
        return Over(Winner(HARE, ESCAPE));
    }
    // hounds win if hare cannot escape
    vector<loc> hare_possible_moves = neighbors(hare, hare_dirs);
    if (hare_possible_moves.size() == 0) {
        return Over(Winner(HOUNDS, TRAP));
    }
    return In_Play();
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

vector<loc> HHGame::neighbors(loc player_piece, loc dirs[])
{
    int x = player_piece.x;
    int y = player_piece.y;
    int dirX, dirY, _size;
    loc dir, pos;
    board b = get_board();
    vector<loc> vec;
    cout << "getting neighbors..." << endl;
    if (get_player() == HOUNDS) {
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
        cout << "dir: " << dir.print() << " pos: " << pos.print();
        if (in_range(pos)) {
            cout << " (in range)" << endl;
            if (pos.x % 2 == 0) pos.y--;
            cout << " actual pos: " << pos.print() << endl;
            if (b.get(pos) == NO_PLAYER)
                vec.push_back(pos);
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
    cout << "from loc: " << from.print() << endl;
    if (b.get(from) == p) {
        // check the move's "to" location is a neighbor of the player at the "from" location
        vector<loc> possible_moves = neighbors(from, (p == HOUNDS)?hounds_dirs:hare_dirs);
        int _size = possible_moves.size();
        for (int i = 0; i < _size; i++)
            if (possible_moves[i].equals(to)) return true;
    }
    return false;
}

void HHGame::make_move(MovePlayer *m)
{
    if (!is_legal_move(m)) return;
    loc from = m->from_loc;
    loc to = m->to_loc;
    int dirX = to.x - from.x;
    int dirY = to.y - from.y;
    if (dirX == 0 && (dirY == -1 || dirY == 1)) {
        hounds_vertical_moves++;
    } else {
        hounds_vertical_moves = 0;
    }

    board b = get_board();
    b.setPos(from, NO_PLAYER);
    b.setPos(to, get_player());
    curr_state._board = b;

    // update game
    status s = get_status();
    status *curr_status = &s;

    if (dynamic_cast<Over*>(curr_status)) {
        cout << "in make_move. Game is Over."<<endl;
        Over *over = (Over*)curr_status;
        Winner *winner = (Winner*)&(over->_outcome);
        if (winner->_player == HARE) {
            hare_score++;
        } else {
            hounds_score++;
        }
    } else { // In_play
        cout << "in make_move. Game is In_Play. SWITCHED PLAYER!" << endl;
        switch_player();
    }
}
