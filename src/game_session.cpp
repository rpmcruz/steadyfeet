/***************************************************************************
                          game_session.cpp  -  responsible for the game handling
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

#include <iostream>

#include "lib/file_access.h"
#include "lib/text.h"
#include "lib/screen.h"
#include "game_session.h"
#include "board.h"
#include "player.h"
#include "background.h"

GameSession* GameSession::current = 0;

GameSession::GameSession(bool border)
  : level_nb(0)
  {
  board = new Board();
  player = new Player();
  background = new Background(border);
  set_levelset("series1.pod");
  }

GameSession::~GameSession()
  {
  delete board;
  delete player;
  delete background;
  }

void GameSession::set_levelset(const std::string& set)
  {
  levelset_filename = set;
  /* First check if the player has already made some change to this levelset. */
  if(file_exists(homedir + "/levels/" + levelset_filename))
    levelset_absolute = homedir + "/levels/" + levelset_filename;
  else
    levelset_absolute = datadir + "/levels/" + levelset_filename;
  }

void GameSession::update(float elapsed_time)
  {
  background->update(elapsed_time);
  player->update();
  }

void GameSession::load_level(int nb)
  {
  if(nb < 0)
    return;
  level_nb = nb;

  state = PLAYING;

  /* Loading a level. */
  background->reset();
  player->reset_level();  // reset variables and stuff
  if(board->load(levelset_absolute, level_nb) == -1)
    state = ABORTED;
  }

void GameSession::load_demo()
  {
  state = PLAYING;
  background->reset();
  player->reset_level();  // reset variables and stuff
  board->load_demo();
  }

void GameSession::draw()
  {
  background->draw();
  board->draw();
  player->draw();
  }

void GameSession::set_level_completed()
  {
  state = COMPLETED;
  player->set_winner();
  background->go_fullspeed();
  }

void GameSession::draw_game_info_msg()
  {
  draw_text("- How to play -", screen->w/2, 30, 15, 0,168,0, 84,246,84, CENTER_ALLIGN);
  draw_text("The goal of the game is to remove all the pieces of the\n"
            " screen - Some of these pieces may require multiple\n"
            " passes - while others have other functions\n",
            14, 75, 9, 160,160,160, 160,160,160);
  draw_text("The different pieces follow", 25, 142, 8,
            255,255,255, 255,255,255);

  int y = 175;
  draw_text("Blue piece", 200, y, 10, 84,84,252, 0,0,168, RIGHT_ALLIGN);
  draw_text("Needs 1 pass to remove", 260, y, 10, 84,84,252, 0,0,168);
  board->draw_icon(ONE_TIME_TILE, 212, y-6);

  y += 35;
  draw_text("Green piece", 200, y, 10, 84,252,84, 0,168,0, RIGHT_ALLIGN);
  draw_text("Needs 2 passes to remove", 260, y, 10, 84,252,84, 0,168,0);
  board->draw_icon(TWO_TIMES_TILE, 212, y-6);

  y += 35;
  draw_text("Cyan piece", 200, y, 10, 84,252,252, 0,168,168, RIGHT_ALLIGN);
  draw_text("Needs 3 passes to remove", 260, y, 10, 84,252,252, 0,168,168);
  board->draw_icon(THREE_TIMES_TILE, 212, y-6);

  y += 45;
  draw_text("Red piece", 200, y, 10, 252,84,84, 168,0,0, RIGHT_ALLIGN);
  draw_text("Permanent - cannot be removed", 260, y, 10, 252,84,84, 168,0,0);
  board->draw_icon(PERMANENT_TILE, 212, y-6);

  y += 35;
  draw_text("Purple piece", 200, y, 10, 252,84,252, 168,0,168, RIGHT_ALLIGN);
  draw_text("Hyperspace piece", 260, y, 10, 252,84,252, 168,0,168);
  board->draw_icon(TELEPORT_TILE, 212, y-6);
            
  draw_text("Use the cursor keys to move your Pod around the network\n"
            "Esc to quit - P to pause the game",
            14, 400, 8, 160,160,160, 160,160,160);
  }

static const char help[] =
  "You may use either the keyboard or the mouse\n"
  "If you use the keyboard - just move the cursor around\n"
  "using the arrow keys - place tiles by pressing 0 to 6\n"
  "numeric keys\n"
  "\n"
  "If you use the mouse - select the tile you want to place\n"
  "from the bottom panel - then just left click in the\n"
  "places you want to put the tile in\n"
  "\n\n"
  "F1 for instructions\n"
  "F3 to save and test the level\n"
  "F4 for just saving";

void GameSession::draw_leveleditor_info_msg()
  {
  draw_text("- How to level editing -", screen->w/2, 30, 15,
            0,168,0, 84,246,84, CENTER_ALLIGN);
  draw_text(help, 14, 85, 9, 160,160,160, 160,160,160);
  }
