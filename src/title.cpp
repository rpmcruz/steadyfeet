/***************************************************************************
                          title.cpp  -  the presentation screen
                             -------------------
    begin                : Sat Feb 19 2005
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
#include <SDL.h>

#include "lib/timer.h"
#include "lib/menu.h"
#include "lib/screen.h"
#include "lib/text.h"
#include "title.h"
#include "background.h"
#include "gameloop.h"
#include "leveleditor.h"
#include "game_session.h"
#include "player.h"
#include "highscore.h"

const char info[] =
  "This is free software under the GNU General Public License - You are welcome\n"
  "to redistribute it under certain conditions - see the COPYING file for details";

#define CREDITS_NB 2
const char credits[CREDITS_NB][256] = {
  "Developed by Ricardo Cruz",
  "Clone of Pod from Swordsoft"
  };

#define IDLE_TIME 15000

static GameSession* game_session;
static SDL_Event event;

static Timer demo_timer;
static unsigned int demo_step, sub_demo_step;
static bool demo_seq;

void start_demo()
  {
  demo_seq = true;
  demo_step = 0;
  sub_demo_step = 0;
  demo_timer.start(1000);
  game_session->load_demo(); 
  }

void end_demo()
  {
  demo_seq = false;
  demo_timer.start(IDLE_TIME);
  }

void update_board_demo();
void update_demo()
  {
  if(demo_step != 3 && (demo_step != 0 || sub_demo_step > 1))
    game_session->player->draw_icon(screen->w/2-17, 125);

  if(demo_step > 1 && !demo_timer.check())
    {
    demo_step++;
    demo_timer.start(6000);
    }

  switch(demo_step)
    {
    case 1:
      update_board_demo();
      break;
    case 2:
      print_highscore();
      break;
    case 3:
      game_session->draw_game_info_msg();
      break;
    case 4:
      end_demo();
      break;
    default:
      break;
    }
  }

void update_board_demo()
  {
  /* Print messages according to step. */
  draw_text("Move Pod around network to remove tracks", screen->w/2, 225, 11, 252,252,84, 168,84,0, CENTER_ALLIGN);

  if(sub_demo_step >= 0 && sub_demo_step <= 10)
    {
    draw_text("Blue takes one pass", screen->w/2, 395, 14, 84,84,252, 0,0,168, CENTER_ALLIGN);
    }
  else if(sub_demo_step > 10 && sub_demo_step <= 29)
    draw_text("Green takes two passes", screen->w/2, 395, 14,
              84,252,84, 0,168,0, CENTER_ALLIGN);
  else if(sub_demo_step > 29 && sub_demo_step <= 55)
    draw_text("And Cyan takes three", screen->w/2, 395, 14,
              84,252,252, 0,168,168, CENTER_ALLIGN);
  else if(sub_demo_step == 56 || sub_demo_step == 57)
    draw_text("And Watch out for Hyperspace", screen->w/2, 395, 14,
              252,84,252, 168,0,168, CENTER_ALLIGN);

  if(demo_timer.check())
    return;
  if(sub_demo_step == 58)
    {
    demo_step++;
    demo_timer.start(5000);
    return;
    }

  /* Let's update the step. */
  sub_demo_step++;

  Player* player = game_session->player;
  switch(sub_demo_step)
    {
    case 1: case 2: case 3:
    case 4: case 5: case 6:
    case 7: case 8: case 9:
    case 20: case 21: case 22:
    case 23: case 24: case 25:
    case 26: case 27: case 28:
    case 39: case 40: case 41:
    case 42: case 43: case 44:
    case 45: case 46: case 47:
      player->set_direction(LEFT);
      demo_timer.start(250);
      break;
    case 10:
    case 29:
      player->set_direction(DOWN);
      demo_timer.start(800);
      break;
    case 57:
      player->set_direction(DOWN);
      demo_timer.start(2000);
      break;
    case 11: case 12: case 13:
    case 14: case 15: case 16:
    case 17: case 18: case 19:
    case 30: case 31: case 32:
    case 33: case 34: case 35:
    case 36: case 37: case 38:
    case 48: case 49: case 50:
    case 51: case 52: case 53:
    case 54: case 55: case 56:
      player->set_direction(RIGHT);
      demo_timer.start(250);
      break;
    default:
      break;
    }
  }

void show_title()
  {
  game_session = new GameSession(false);
  game_session->activate();

  start_demo();

  start_framerate();

  demo_timer.start(1000);

  bool done = false;
  while(!done)
    {
    float elapsed_time = get_framerate();
    game_session->update(elapsed_time);

    // drawing stuff
    if(demo_seq && demo_step == 1)
      game_session->draw();
    else if(demo_seq)
      game_session->background->draw();
    else
      {
      game_session->background->draw();
      main_menu->show();
      }

    /* Let's update the demo sequence. */
    if(demo_seq)
      update_demo();
    else if(!demo_timer.check())
      {
      start_demo();
      demo_step = 1;
      }

    if(demo_step == 0 && !demo_timer.check())
      {
      demo_timer.start(1000);
      sub_demo_step++;
      if(sub_demo_step > 3)
        {
        demo_step++;
        sub_demo_step = 0;
        }
      }

    if(!demo_seq || demo_step != 3)
      {
      if(demo_seq && demo_step == 0 && sub_demo_step == 0) ;
      else if(demo_seq && demo_step == 0 && sub_demo_step == 1)
        draw_text("SteadyFeet", screen->w/2, 40,
                  demo_timer.get_gone() * 30 / demo_timer.get_total_time(),
                  84,84,252, 0,0,168, CENTER_ALLIGN);
      else
        draw_text("SteadyFeet", screen->w/2, 40, 30, 84,84,252, 0,0,168, CENTER_ALLIGN);

      if(!demo_seq || demo_step != 0 || sub_demo_step > 1)
        draw_text(credits[(SDL_GetTicks()/4000) % CREDITS_NB], screen->w/2, 90, 12,
                252,84,84, 168,0,0, CENTER_ALLIGN);
      if(!demo_seq)
        draw_text(info, 4, 458, 7, 252,252,252, 168,168,168);
      else if(demo_step != 0 || sub_demo_step > 1)
        draw_text("Press any key to start", screen->w/2, 458, 10,
                  252,252,252, 168,168,168, CENTER_ALLIGN);
      }
    screen->update();

    // with this current approach we can easily add another menu (like an Options)
    if(!demo_seq)
      {
      if(!Menu::idle)
        end_demo();
      switch(main_menu->get_selected())
        {
        case START_GAME_ID:
          {
          Gameloop gameloop;
          gameloop.run();
          }
          break;
        case LEVELEDITOR_ID:
          {
          LevelEditor leveleditor;
          leveleditor.run();
          }
          break;
        case OPTIONS_ID:
          // Not yet implemented
          break;
        case BACK_ID:
          if(main_menu->get_subselected() == 0)
            done = true;
          break;
        case NO_ITEM_SELECTED:
        default:
          break;
        }
      }
    else
      while(SDL_PollEvent(&event))
        if(event.type == SDL_KEYUP || event.type == SDL_MOUSEBUTTONUP ||
           event.type == SDL_QUIT)
          end_demo();
    }
  screen->shrink_fade(screen->w/2, screen->h/2, 220);
  }
