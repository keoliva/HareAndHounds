#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#define ROWS 3
#define NUM_PLAYERS 2

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

enum player {
    NO_PLAYER, HOUNDS, HARE
};

enum Win {
    TRAP, ESCAPE, STALL
};
class outcome {
  public:
    virtual std::string outcome_to_string() { return ""; };
};
class Winner : public outcome {
  public:
    player _player;
    Win win;
    Winner (player p, Win w): _player(p), win(w) {};
    std::string outcome_to_string() {
        std::stringstream ss;
        ss << "Winner<" << ((_player == HOUNDS)?"HOUNDS":"HARE") << ">";
        return ss.str();
    };
};

class status {
  public:
    virtual std::string status_to_string() { return ""; };
};
class Over : public status {
  public:
    outcome _outcome;
    Over(outcome o) : _outcome(o) {};
    std::string status_to_string() {
        std::stringstream ss;
        ss << "Over<" << _outcome.outcome_to_string() << ">";
        return ss.str();
    };
};
class In_Play : public status {
  public:
    std::string status_to_string() { return "In_Play"; };
};

struct loc {
    int x, y;
    loc () {};
    loc(int x0, int y0) { x=x0; y=y0; };
    std::string print() {
        std::stringstream ss;
        ss << "loc(" << x << ", " << y << ")";
        return ss.str();
    };
    bool equals(loc l1) {
        return x == l1.x && y == l1.y;
    }
};

class board {
    std::vector<std::vector<player>> vec;
    public:
        board() {};
        board(std::vector<std::vector<player>> v) : vec(v) {};
        player get(loc pos);
        void setPos(loc pos, player p);
};

struct state {
    player _player;
    board _board;
};

class gameMove {};
class MovePlayer : public gameMove {
  public:
    loc from_loc, to_loc;
    MovePlayer(loc from, loc to) : from_loc(from), to_loc(to) {};
};

class HHGame {
    int rounds, hare_score, hounds_score, turns, hounds_vertical_moves; // in a row
    state curr_state;
    void init_state();
    void switch_player();
    std::vector<loc> neighbors(loc player_piece, loc dirs[]);
  public:
    HHGame();
    void restart();
    bool gameOver();
    int get_hare_score();
    int get_hounds_score();
    int get_round();
    player get_player();
    board get_board();
    status get_status();
    loc get_hare();
    loc *get_hounds();
    bool is_legal_move(MovePlayer *m);
    void make_move(MovePlayer *m);
};

#endif // GAME_H_INCLUDED
