/***************************************************************************
                          game_session.h  -  responsible for the game handling
                             -------------------
    begin                : Fri Feb 18 2005
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
 
#ifndef GAME_SESSION_H
#define GAME_SESSION_H

#include <string>

#include "lib/timer.h"

class Board;
class Player;
class Background;

enum GameState { PLAYING, COMPLETED, LOST, ABORTED };

/* This is a small class that encapsulates the game stuff to be accessed
   by both Gameloop and LevelEditor. */

class GameSession
  {
  friend class Gameloop;
  friend class LevelEditor;

  public:
    GameSession(bool background_border = true);
    ~GameSession();

    void activate()
      { current = this; };

    void set_levelset(const std::string& file);

    void load_level(int nb);
    void load_demo();  // used for the title demo

    void update(float elapsed_time);
    void draw();

    void set_level_completed();

    void draw_game_info_msg();
    void draw_leveleditor_info_msg();

    /* These stuff is here so that Board and Player can communicate with
       each other. For instance, GameSession::current->player->set_pos(0,0); */
    static GameSession* current;
    Board* board;
    Player* player;
    Background* background;

    Timer level_time;

  private:
    GameState state;

    /* Information regard the current level. */
    // don't edit this directly, use set_levelset()
    std::string levelset_filename;  // this only got the filename
    std::string levelset_absolute;  // this got the absolute path to the file

    int level_nb;
  };

#endif /*GAME_SESSION_H*/
