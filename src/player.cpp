/***************************************************************************
                          player.cpp  -  player stuff
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

#include "lib/surface.h"
#include "lib/file_access.h"
#include "player.h"
#include "game_session.h"
#include "board.h"
#include "background.h"

/* You can tweak these values to change the behavior. */
#define DEAD_TIME   1400
#define WINNER_TIME 3000
// small timings
#define MOVE_TIME 80
#define WARP_TIME 200

/* These values are related with data stuff. */
#define FALLING_FRAMES 8

Player::Player(GameSession *session)
  : tile_x(0), tile_y(0), session(session)
  {
  surface = new Surface((datadir + "/graphics/player.bmp").c_str(), 255, 0, 255);
  surface_falling = new Surface((datadir + "/graphics/player-falling.bmp").c_str(), 255, 0, 255);

  reset_game();
  }

Player::~Player()
  {
  delete surface;
  delete surface_falling;
  }

void Player::reset_game()
  {
  stats.reset_game();

  reset_level();
  }

void Player::reset_level()
  {
  sequence = NO_SEQ;
  direction = STOP;
  dir_queue.clear();
  }

void Player::draw()
  {
  int x = tile_x*TILE_W + session->board->board_x;
  int y = tile_y*TILE_H + session->board->board_y;

  if(sequence == MOVING_SEQ)
    switch(direction)
      {
      case LEFT:
        x += timer.get_left()*TILE_W/MOVE_TIME;
        break;
      case RIGHT:
        x -= timer.get_left()*TILE_W/MOVE_TIME;
        break;
      case UP:
        y += timer.get_left()*TILE_H/MOVE_TIME;
        break;
      case DOWN:
        y -= timer.get_left()*TILE_H/MOVE_TIME;
        break;
      default:
        break;
      }
  else if(sequence == WARPING_SEQ)
    {
    x = old_tile_x*TILE_W + session->board->board_x;
    y = old_tile_y*TILE_H + session->board->board_y;
    }

  if(sequence == FALLING_SEQ && !timer.check())
    return;
  else if(sequence == FALLING_SEQ)
    {
    surface_falling->draw_frame(x, y,
            timer.get_gone()*FALLING_FRAMES/DEAD_TIME, FALLING_FRAMES);
    }
  else
    surface->draw(x, y);
  }

void Player::draw_icon(int x, int y)
  {
  surface->draw(x, y);
  }

void Player::kill()
  {
  stats.loose_life();

  sequence = FALLING_SEQ;
  timer.start(DEAD_TIME);
//  session->background->freeze();
  }

void Player::set_winner()
  {
std::cerr << "winner!\n";
  sequence = WINNING_SEQ;
  timer.start(WINNER_TIME);

  stats.add_score(10);
  before_win_score = stats.score;
  if(session->level_time.check())
    stats.add_score(session->level_time.get_left()/1000 * 10);
  }

void Player::warp(int x, int y)
  {
  old_tile_x = tile_x;
  old_tile_y = tile_y;
  tile_x = x;
  tile_y = y;

  sequence = WARPING_SEQ;
  timer.start(WARP_TIME);
  }

void Player::force_direction(Direction dir)
  {
  old_tile_x = tile_x;
  old_tile_y = tile_y;

  switch(dir)
    {
    case LEFT:
      tile_x--;
      break;
    case RIGHT:
      tile_x++;
      break;
    case UP:
      tile_y--;
      break;
    case DOWN:
      tile_y++;
      break;
    default:
      break;
    }
  direction = dir;
  }

int Player::get_score()
  {
  if(sequence != WINNING_SEQ)
    return stats.score;
  return timer.get_gone() * (stats.score-before_win_score) / timer.get_total_time()
         + before_win_score;
  }

void Player::update()
  {
  if(sequence)
    {
    if(sequence != FALLING_SEQ && !timer.check())
      {
      sequence = NO_SEQ;
//      if(sequence == WARPING_SEQ || sequence == MOVING_SEQ)
        session->board->check_players_pos();
      }
    else
      return;
    }

  if(!dir_queue.empty())
    {
    force_direction(dir_queue.front());
    dir_queue.pop_front();
    sequence = MOVING_SEQ;
    timer.start(MOVE_TIME);

//    session->board->check_players_pos();
    }
  }
