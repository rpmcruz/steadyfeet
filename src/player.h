/***************************************************************************
                          player.h  -  player stuff
                             -------------------
    begin                : Tue Feb 15 2005
    copyright            : (C) 2005 by Ricardo Cruz
    email                : rick2@aeiou.pt
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PLAYER_H
#define PLAYER_H

#include <list>

#include "lib/timer.h"

class Surface;
class GameSession;

enum Direction { STOP, LEFT, RIGHT, UP, DOWN };
enum AnimationSequence { NO_SEQ, FALLING_SEQ, WINNING_SEQ, WARPING_SEQ, MOVING_SEQ };

/* Stats takes care of the information score, life, etc. */
struct Stats
  {
    Stats() { reset(); };
    ~Stats() { };

    void reset()
      { score = 0; };
    int get_score() { return score; }
     void add_score(int nb)
      { score += nb; }

    int score;
  };

class Player
  {
  public:
    Player(GameSession *session);
    ~Player();

    /* Should be called on level start. */
    void reset_level();
    void reset_game();

    void set_pos(int x, int y)
      { tile_x = x; tile_y = y;
        old_tile_x = tile_x; old_tile_y = tile_y; }

    void draw();
    void update();

    /* Used to draw the lives. */
    void draw_icon(int x, int y);

    void set_direction(Direction dir)
      { dir_queue.push_back(dir); }
    void force_direction(Direction dir);

    /* Kill player. */
    void kill();
    bool is_dead()
      { return sequence == FALLING_SEQ && !timer.check(); }
    void set_winner();
    void warp(int x, int y);

    /* Check if the player isn't animating. */
    bool is_animating()
      { return sequence && timer.check(); }

    int get_score();  // hack to show score increasing at end
    int get_high_score();  // get the score at the end of the game

    /* Let's put this public to avoid the get_X() funcs. */
    int tile_x, tile_y, old_tile_x, old_tile_y;
    bool has_moved;

  private:
    friend class Board;
    GameSession *session;
    Surface *surface, *surface_falling;

    Direction direction;  // next direction

    Timer timer;
    AnimationSequence sequence;
    std::list <Direction> dir_queue;

    int before_win_score;  // score before the end level bonus
    Stats global_stats, level_stats;
  };

#endif /*PLAYER_H*/
