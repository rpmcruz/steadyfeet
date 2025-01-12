/***************************************************************************
                          leveleditor.h  -  built-in leveleditor
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

#ifndef LEVELEDITOR_H
#define LEVELEDITOR_H

#include <string>

class Player;
class Background;
class Board;
class GameSession;
class Surface;

/* Cursor shown on the level editor. */

struct Cursor
  {
  void draw(GameSession *session);
  int x, y;
  };

/* Button which the user can press to choose a tile. */

#define MAX_TILES 5
#define TOTAL_BUTTONS (MAX_TILES+2)

class Button
  {
  public:
    Button(const std::string image_filename, int id, int x, int y, bool selected = false);
    ~Button();

    void draw();
    bool check_event(int x, int y);
    void set_selected(bool flag = true)
      { selected = flag; }

    int id;

  private:
    int x, y;
    bool selected;
    Surface* surface;
  };

/* Built-in level editor. */

class LevelEditor
  {
  public:
    LevelEditor(const std::string& levelset, bool user_made);
    ~LevelEditor();

    /* Let's rock! */
    void run();

    void load_level(int nb);

  private:
    void check_events();
    void draw_status_info();
    void set_selected_button(int nb);

    /* Saves current level. */
    void save_level();

    /* The core game is in this object: */
    GameSession* game_session;

    /* These are just pointers to GameSession ones to facilitate work. */

    Board* board;
    Player* player;
    Background* background;

    Cursor cursor;
    Button* button[TOTAL_BUTTONS];
    int current_tile;
    bool modified;
  };

#endif /*LEVELEDITOR_H*/
