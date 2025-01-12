/***************************************************************************
                          gameloop.cpp  -  the usual game's loop
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

#include <SDL.h>
#include <iostream>
#include <string>

#include "lib/timer.h"
#include "lib/text.h"
#include "lib/screen.h"
#include "lib/file_access.h"
#include "gameloop.h"
#include "game_session.h"
#include "board.h"
#include "player.h"
#include "background.h"
#include "highscore.h"

static SDL_Event event;

Gameloop::Gameloop()
  {
  game_session = new GameSession();
  player = game_session->player;
  board = game_session->board;
  background = game_session->background;
  }

Gameloop::~Gameloop()
  {
  delete game_session;
  }

void Gameloop::load_levelset(const std::string& file)
  {
  game_session->set_levelset(file);
  }

void Gameloop::run(bool test_mode)
  {
  if(game_session->level_nb == 0)
    load_level(1);

  start_framerate();

  while(game_session->state != ABORTED)
    {
    check_events();

    float elapsed_time = get_framerate();
    game_session->update(elapsed_time);

    /* Do the ordinary gameplay stuff. */
    if(player->is_dead())
      {
std::cerr << "player dead\n";
      if(test_mode)
        break;
      else
        {
        game_session->state = LOST;
        if(player->stats.lives_nb == 0)
          {
std::cerr << "game end\n";
          /* Game end. */
          // FIXME: game end should also happen on last level
          add_highscore(player->stats.score, background);
          break;
          }
        else  // just reload the level
{
std::cerr << "let's re-load this level\n";
          load_level(game_session->level_nb);
}
        }
      }
    if(game_session->state == COMPLETED && !player->is_animating())
      load_level(game_session->level_nb+1);

    // time for drawing
    game_session->draw();
    draw_status_info();
    if(test_mode)
      draw_text("Level Editor - Testing level", 22, 22, 14, 168, 0, 0, 252, 84, 84);

    screen->update();
    }
  }

void Gameloop::load_level(int nb)
  {
  if(nb < 0)
    return;

std::cerr << "level: " << nb << std::endl;
  game_session->load_level(nb);

  clean_events();  // to ignore key's pressed during loading
  game_session->level_time.start(61*1000);
  }

void Gameloop::draw_status_info()
  {
  int sz;
  char str[256];
  // score
  sprintf(str, "%4d", player->get_score());
  sz = draw_text("Score ", 170, 455, 12, 84, 252, 84, 0, 168, 0);
  draw_text(str, 170+sz, 455, 12, 252, 84, 252, 168, 0, 168);
  // level number
  sprintf(str, "%d", game_session->level_nb);
  sz = draw_text("Level ", 360, 455, 12, 84, 252, 84, 0, 168, 0);
  draw_text(str, 360+sz, 455, 12, 252, 84, 252, 168, 0, 168);
  // time
  int time = game_session->level_time.get_left() / 1000;
  if(time < 0)
    {
    time = 0;
    sprintf(str, "00");
    }
  else if(time < 10)
    sprintf(str, "0%d", time);
  else
    sprintf(str, "%d", time);
  sz = draw_text("Time ", 500, 455, 12, 84, 252, 84, 0, 168, 0);
  draw_text(str, 500+sz, 455, 12, 252, 84, 252, 168, 0, 168);
  // life icons
  for(int i = 1; i < player->stats.lives_nb; i++)
    player->draw_icon(5+((i-1)*(TILE_W+5)), 450);
  }

void Gameloop::clean_events()
  {
  while(SDL_PollEvent(&event))
    ;
  mouse_pressed = false;
  }

#define EVENT_PRESSING_DELAY 270

void Gameloop::check_events()
  {
  while(SDL_PollEvent(&event))
    {
    switch(event.type)
      {
      case SDL_KEYDOWN:	// key pressed
        switch(event.key.keysym.sym)
          {
          case SDLK_LEFT:
            player->set_direction(LEFT);
            break;
          case SDLK_RIGHT:
            player->set_direction(RIGHT);
            break;
          case SDLK_UP:
            player->set_direction(UP);
            break;
          case SDLK_DOWN:
            player->set_direction(DOWN);
            break;
          case SDLK_RETURN:
            if(event.key.keysym.mod & KMOD_ALT)
              screen->toggle_fullscreen();
            break;
          // debug stuff
          case SDLK_PLUS:
          case SDLK_KP_PLUS:
            load_level(game_session->level_nb+1);
            break;
          case SDLK_MINUS:
          case SDLK_KP_MINUS:
            load_level(game_session->level_nb-1);
            break;
          default:
            break;
          }
        break;
      case SDL_KEYUP:	// key released
        switch(event.key.keysym.sym)
          {
          case SDLK_p:
            pause_timers();

            background->draw();
            draw_text("Game Paused", screen->w/2, 180, 16,
                      0, 168, 168, 84, 252, 252, CENTER_ALLIGN);
            draw_text("Press Any Key to Resume", screen->w/2, 260, 12,
                      168, 0, 0, 252, 84, 84, CENTER_ALLIGN);
            draw_status_info();
            screen->update();

            {
            bool game_paused = true;
            while(game_paused)
              {
              SDL_Delay(250);  // dont waste all cpu on this
              while(SDL_PollEvent(&event))
                if(event.type == SDL_KEYUP || event.type == SDL_QUIT)
                  game_paused = false;
                }
            }
            resume_timers();
            break;
          case SDLK_F1:
            pause_timers();
            background->draw();
            game_session->draw_game_info_msg();
            screen->update();
            {
            bool game_paused = true;
            while(game_paused)
              {
              SDL_Delay(250);  // dont waste all cpu on this
              while(SDL_PollEvent(&event))
                if(event.type == SDL_KEYUP || event.type == SDL_QUIT)
                  game_paused = false;
              }
            }
            resume_timers();
            break;
          case SDLK_ESCAPE:
            game_session->state = ABORTED;
            break;
          default:
            break;
          }
        break;
      /* Mouse support. */
      case SDL_MOUSEBUTTONDOWN:
        if(event.button.button == SDL_BUTTON_LEFT)
          mouse_pressed = true;
        break;
      case SDL_MOUSEBUTTONUP:
        mouse_pressed = false;
        event_timer.stop();
        break;
#if 0
// disable cause I don't think joystick is suitable for the game
      /* Joystick support. */
      case SDL_JOYAXISMOTION:  /* Handle Joystick Motion */
        if((event.jaxis.value < -4096) || (event.jaxis.value > 4096))
          {
          /* Let's ignore events if they persist. */
          if(event_timer.check())
            break;
          event_timer.start(EVENT_PRESSING_DELAY);

          if(event.jaxis.axis == 0)  // Left-right movement code goes here
            {
            if(event.jaxis.value < 0)
              player->set_direction(LEFT);
            else
              player->set_direction(RIGHT);
            }
          else if(event.jaxis.axis == 1)   // Up-Down movement code goes here
            {
            if(event.jaxis.value < 0)
              player->set_direction(UP);
            else
              player->set_direction(DOWN);
            }
          }
        break;
      case SDL_JOYHATMOTION:  /* Handle Hat Motion */
        /* If two movements are made at the same time, let's just
           ignore them both. */
        if(event.jhat.value & SDL_HAT_RIGHTUP || event.jhat.value & SDL_HAT_RIGHTDOWN ||
           event.jhat.value & SDL_HAT_LEFTUP || event.jhat.value & SDL_HAT_LEFTDOWN)
           break;

        /* Let's ignore events if they persist. */
        if(event_timer.check())
          break;
        event_timer.start(EVENT_PRESSING_DELAY);
        if(event.jhat.value & SDL_HAT_LEFT)
          player->set_direction(LEFT);
        else if(event.jhat.value & SDL_HAT_RIGHT)
          player->set_direction(RIGHT);
        else if(event.jhat.value & SDL_HAT_UP)
          player->set_direction(UP);
        else if(event.jhat.value & SDL_HAT_DOWN)
          player->set_direction(DOWN);
        break;
#endif
      case SDL_QUIT:	// window closed
        game_session->state = ABORTED;
        break;
      default:
        break;
      }
    }

  /* Check for keep pressed mouse or gamepad button. */
  /* This code is so that you can hold the button and player keep
     moving. We don't need it for keyboard, since we have already setup it
     in main.cpp . */
  if(mouse_pressed && !event_timer.check())
    {
    int x = (event.button.x-BOARD_X) / TILE_W, y = (event.button.y-BOARD_Y) / TILE_H;
    if(x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT)
      if(player->tile_x == x || player->tile_y == y)
        {
        event_timer.start(EVENT_PRESSING_DELAY);
        if(x < player->tile_x)
          player->set_direction(LEFT);
        else if(x > player->tile_x)
          player->set_direction(RIGHT);
        else if(y < player->tile_y)
          player->set_direction(UP);
        else if(y > player->tile_y)
          player->set_direction(DOWN);
        }
    }
  }
