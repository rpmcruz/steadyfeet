/***************************************************************************
                          gameloop.h  -  the usual game's loop
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

#ifndef GAMELOOP_H
#define GAMELOOP_H

#include <string>

#include "lib/timer.h"

class Board;
class Player;
class Background;
class GameSession;

class Gameloop
  {
  public:
    Gameloop();
    ~Gameloop();

    /* Let's rock! */
    void run(bool testing_mode = false);

    void load_levelset(const std::string& file);
    void load_level(int nb);

  private:
    void check_events();
    void clean_events();
    void draw_status_info();
 
    /* The core game is in this object: */
    GameSession* game_session;

    /* These are just pointers to GameSession ones to facilitate work. */
    Player* player;
    Board* board;
    Background* background;

    // this timer is used, so that you can keep pressing the mouse or gamepad and
    // it will keep going...
    Timer event_timer;
    bool mouse_pressed;
  };

#endif /*GAMELOOP_H*/
