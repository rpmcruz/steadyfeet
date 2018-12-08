/***************************************************************************
                          board.h  -  the board
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
 
#ifndef BOARD_H
#define BOARD_H

#include <string>

#define BOARD_X ((640-BOARD_WIDTH*TILE_W)/2)
#define BOARD_Y ((480-BOARD_HEIGHT*TILE_H)/2)

#define BOARD_WIDTH  12
#define BOARD_HEIGHT 12

// the size of each tile (width and height is the same)
#define TILE_W 34
#define TILE_H 24

class Surface;

enum Tile {
  NO_TILE = -1,
  ONE_TIME_TILE,
  TWO_TIMES_TILE,
  THREE_TIMES_TILE,
  PERMANENT_TILE,
  TELEPORT_TILE,
  CLEANUP_TILE,
  TOTAL_TILES
  };

class Board
  {
  friend class GameSession;

  public:
    Board();
    ~Board();

    /* Loads level file. Returns -1 if error, 0 otherwise. */
    int load(const std::string& filename, int level);
    void load_demo();  // for title screen

    /* Same as load, but the otherway around. */
    void save(const std::string& input_filename, const std::string& output_filename,
              int level);

    void draw();

    void draw_icon(Tile tile, int x, int y);

    /* When player moves, it will trigger this call to check
       what's his current and tile what was his previous. */
    void check_players_pos();

    int get_tile(int x, int y);
    void set_tile(int x, int y, Tile tile);

  private:
    Surface* surface[TOTAL_TILES];

    int board[BOARD_WIDTH][BOARD_HEIGHT];
    int total_tiles;
    int base_x, base_y;
  };

#endif /*BOARD_H*/ 
