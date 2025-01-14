/***************************************************************************
                          board.cpp  -  the board
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

#include <iostream>
#include <string>
#include <fstream>
#include <assert.h>
 
#include "lib/surface.h"
#include "lib/file_access.h"
#include "board.h"
#include "game_session.h"
#include "player.h"
#include "background.h"

Board::Board(GameSession *session)
: session(session)
  {
  char str[1024];
  for(int i = 0; i < TOTAL_TILES; i++)
    {
    sprintf(str, "%s/graphics/tile%d.bmp", datadir.c_str(), i+1);
    surface[i] = new Surface(str, 255, 0, 255);
//    surface[i]->set_transparent(128);  // semi-transparency
    }
  }

Board::~Board()
  {
  for(int i = 0; i < TOTAL_TILES; i++)
    delete surface[i];
  }

void Board::load_demo()
  {
  base_x = 1; base_y = 7;
  session->player->set_pos(10, 7);
  total_tiles = 10;

  board_width = board_height = 12;
  board.resize(board_width);
  for (std::vector<std::vector<int> >::iterator it = board.begin(); it != board.end(); it++) {
      it->resize(board_height, 0);
  }

  board_x = (640-board_width*TILE_W)/2;
  board_y = (480-board_height*TILE_H)/2 - 15;

  for(int x = 0; x < board_width; x++)
    for(int y = 0; y < board_height; y++)
      {
      if(y >= 7 && y <= 9 && x >= 1 && x <= 10)
        {
        if(x == 1 || x == 10)
          board[x][y] = PERMANENT_TILE;
        else if(y == 7)
          board[x][y] = ONE_TIME_TILE;
        else if(y == 8)
          board[x][y] = TWO_TIMES_TILE;
        else if(y == 9)
          board[x][y] = THREE_TIMES_TILE;
        else
          board[x][y] = NO_TILE;
        }
      else
        board[x][y] = NO_TILE;
      }
  board[10][10] = TELEPORT_TILE;

  /* Print the board for debugging. */
#if 0
  std::cerr << "Player pos: " << pla_x << ", " << pla_y << std::endl;
  std::cerr << "Level layout:\n";
  for(int h = 0; h < BOARD_HEIGHT; h++)
    {
    for(int w = 0; w < BOARD_WIDTH; w++)
        std::cerr << board[w][h] + 1;
    std::cerr << std::endl;
    }
  std::cerr << std::endl;
#endif
  }

int Board::load(const std::string& filename, int level)
{
  std::ifstream file(filename.c_str());
  if(!file)
    {
    std::cerr << "Error: Could not read board file: " << filename << std::endl;
    return -1;
    }

  // get height of board
  board_width = 12;
  board_height = 12;
  std::string firstLine;
  if(std::getline(file, firstLine)) {
      // Calculate the length of the first line
      std::size_t length = firstLine.length();
      board_width = (length-4) / board_height;
  }
  std::cerr << "board_height: " << board_height << std::endl;
  board_x = (640-board_width*TILE_W)/2;
  board_y = (480-board_height*TILE_H)/2 - 15;

  file.clear();
  file.seekg(0, std::ios::beg);

  board.resize(board_width);
  for (std::vector<std::vector<int> >::iterator it = board.begin(); it != board.end(); it++) {
      it->resize(board_height, 0);
  }

  base_x = (base_y = -1);

  char c;
  /* Let's ignore the file until the line we want */
  for(int i = 0; i < level-1 && !file.eof(); )
    {
    file.get(c);
    if(c == '\n')
      i++;
    }

  if(file.eof() || file.peek() == EOF)
    {
    std::cerr << "Error: level " << level << " doesn't seem to exist in file: "
              << filename << std::endl;
    return -1;
    }

  /* Firstly, we'll read player's pos. */
  int pla_x, pla_y;
  file >> c; pla_x = (c - '0') * 10;
  file >> c; pla_x += (c - '0');
  file >> c; pla_y = (c - '0') * 10;
  file >> c; pla_y += (c - '0');
  session->player->set_pos(pla_x-1, pla_y-1);

  /* Let's now read the file into our board array */
  // POD=12x12, kgryzzles=12x18
  total_tiles = 0;
  for(int x = 0; x < board_width; x++)
    for(int y = 0; y < board_height; y++)
      {
      file >> c;
      if(file.eof())
        {
        std::cerr << "Error: level seems broken in file: " << filename << std::endl;
        return -1;
        }
      switch(c)
        {
        case '0':
          board[x][y] = NO_TILE;
          break;
        case '1':
          board[x][y] = ONE_TIME_TILE;
          total_tiles++;
          break;
        case '2':
          board[x][y] = TWO_TIMES_TILE;
          total_tiles += 2;
          break;
        case '3':
          board[x][y] = THREE_TIMES_TILE;
          total_tiles += 3;;
          break;
        case '4':
          board[x][y] = PERMANENT_TILE;
          if(base_x == -1 || base_y == -1)
            {
            base_x = x;
            base_y = y;
            }
          break;
        case '5':
          board[x][y] = TELEPORT_TILE;
          total_tiles++;
          break;
        default:
          std::cerr << "Warning: Tile not recognized: " << c << std::endl;
          board[x][y] = NO_TILE;
          break;
        }
//      board[w][h] = c -'0' + 1;
      }

  /* Print the board for debugging. */
#if 0
  std::cerr << "Player pos: " << pla_x << ", " << pla_y << std::endl;
  std::cerr << "Level layout:\n";
  for(int h = 0; h < BOARD_HEIGHT; h++)
    {
    for(int w = 0; w < BOARD_WIDTH; w++)
        std::cerr << board[w][h] + 1;
    std::cerr << std::endl;
    }
  std::cerr << std::endl;
#endif

  return 0;
  }

void Board::save(const std::string& input_filename, const std::string& output_filename,
                 int level)
  {
  std::cout << "Saving level to file: " << output_filename << std::endl;

  std::string stream;

  std::ifstream input_file(input_filename.c_str());
  if(!input_file)
    {
    std::cerr << "Error: Could not get read access to file: " << input_filename
              << std::endl;
    return;
    }

  /* Firstly, let's remove this line. */
  char c;
  for(int i = 0; i < level-1 && !input_file.eof(); )
    {
    input_file.get(c);
    if(c == '\n')
      i++;
    stream += c;
    }

  /* Secondly, let's write player's pos. */
  int pla_x = session->player->tile_x;
  int pla_y = session->player->tile_y;
  stream += pla_x / 10 + '0';
  stream += pla_x % 10 + '0' + 1;
  stream += pla_y / 10 + '0';
  stream += pla_y % 10 + '0' + 1;
  
  /* Let's now write the board array into the file. */
  for(unsigned int x = 0; x < board.size(); x++)
    for(unsigned int y = 0; y < board[0].size(); y++)
      switch(board[x][y])
        {
        case NO_TILE:
          stream += "0";
          break;
        case ONE_TIME_TILE:
          stream += "1";
          break;
        case TWO_TIMES_TILE:
          stream += "2";
          break;
        case THREE_TIMES_TILE:
          stream += "3";
          break;
        case PERMANENT_TILE:
          stream += "4";
          break;
        case TELEPORT_TILE:
          stream += "5";
          break;
        default:
          stream += "0";
          std::cerr << "Warning: Tile not recognized: " << board[x][y] << std::endl;
          break;
        }
  stream += '\n';

  /* At last, let's save the rest of the level back to the file. */
  for(int i = level+1;;)
    {
    input_file.get(c);
    if(c == '\n')
      i++;
    if(input_file.eof())
      break;
    stream += c;
    }

  /* Finally, let's put our stream int out file. */
  input_file.close();

  std::ofstream output_file(output_filename.c_str());
  if(!output_file)
    {
    std::cerr << "Error: Could not get write access to file: " << output_filename
              << std::endl;
    return;
    }

  output_file << stream;

  // debug
//  std::cerr << "Saving board:\n" << stream << std::endl;

  /* Done, let's hope it's well done. ;) */

  return;
  }

void Board::draw()
  {
  for(unsigned int x = 0; x < board.size(); x++)
    for(unsigned int y = 0; y < board[0].size(); y++)
      {
      int t = get_tile(x,y);
      if(t != NO_TILE)
        surface[t]->draw(x*TILE_W + board_x, y*TILE_H + board_y);
      }
  }

void Board::draw_icon(Tile tile, int x, int y)
  {
  surface[tile]->draw(x,y);
  }

int Board::get_tile(int x, int y)
  {
  if(x < 0 || x >= (signed)board.size() || y < 0 || y >= (signed)board[0].size())
    return NO_TILE;  // out of field

  int t = board[x][y];
  assert(t >= NO_TILE && t < TOTAL_TILES);
  return t;
  }

void Board::set_tile(int x, int y, Tile tile)
  {
  if(x < 0 || x >= (signed)board.size() || y < 0 || y >= (signed)board[0].size())
    return;  // out of field

  assert(tile >= NO_TILE && tile < TOTAL_TILES);

  board[x][y] = tile;
  }

void Board::check_players_pos()
  {
  Player* player = session->player;

  /* Check player's previous position. */
  switch(get_tile(player->old_tile_x, player->old_tile_y))
    {
    case ONE_TIME_TILE:
    case TWO_TIMES_TILE:
    case THREE_TIMES_TILE:
      /* reduce tile life. */
      board[player->old_tile_x][player->old_tile_y]--;
      player->stats.add_score(10);

      total_tiles--;
      if(total_tiles == 0 &&
         get_tile(player->tile_x, player->tile_y) == PERMANENT_TILE)
        {
std::cerr << "level completed\n";
        /* Level completed!! */
        session->set_level_completed();
        }
      break;
    case TELEPORT_TILE:
std::cerr << "older tile is a teleport, remove it\n";
      board[player->old_tile_x][player->old_tile_y] = NO_TILE;
      total_tiles--;
      break;
    default:
      break;
    }

  /* Check player's current position. */
  switch(get_tile(player->tile_x, player->tile_y))
    {
    case NO_TILE:
std::cerr << "kill the player\n";
      player->kill();
      break;
    case TELEPORT_TILE:
std::cerr << "teleporting player\n";
      player->warp(base_x, base_y);
      break;
    default:
      break;
    }
  }
