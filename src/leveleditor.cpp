/***************************************************************************
                          leveleditor.cpp  -  built-in leveleditor
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

#include <SDL.h>
#include <iostream>
#include <string>

#include "lib/text.h"
#include "lib/screen.h"
#include "lib/file_access.h"
#include "lib/surface.h"
#include "lib/menu.h"
#include "leveleditor.h"
#include "game_session.h"
#include "gameloop.h"
#include "player.h"
#include "background.h"
#include "board.h"

static SDL_Event event;

void Cursor::draw(GameSession *session)
  {
  screen->draw_animated_filled_rect(x*TILE_W + session->board->board_x, y*TILE_H + session->board->board_y, TILE_W, TILE_H, 6);
  }

Button::Button(const std::string image_file, int id, int x, int y, bool selected)
  : id(id), x(x), y(y), selected(false)
  {
  surface = new Surface(image_file, 255, 0, 255);
  }

Button::~Button()
  {
  delete surface;
  }

void Button::draw()
  {
  if(selected)
    screen->fill_rect(x-1,y-1,surface->w+2,surface->h+2,168,168,168);
  surface->draw(x,y);
  }

bool Button::check_event(int x, int y)
  {
  if(x < this->x || x >= this->x+surface->w || y < this->y || y >= this->y+surface->h)
    {
    selected = false;
    return false;
    }
  selected = true;
  return true;
  }

LevelEditor::LevelEditor(const std::string &levelset, bool user_made)
  {
  game_session = new GameSession(levelset, user_made, true);
  char str[1024];
  for(int i = 0; i <= MAX_TILES+1; i++)
    {
    if(i == 0)
      sprintf(str, "%s/graphics/rubber.bmp", datadir.c_str());
    else if(i == MAX_TILES+1)
      sprintf(str, "%s/graphics/player.bmp", datadir.c_str());
    else
      sprintf(str, "%s/graphics/tile%d.bmp", datadir.c_str(), i);
    button[i] = new Button(str, i-1, 10+i*(TILE_W+30), 448);
    }
  button[1]->set_selected();
  current_tile = 0;

  player = game_session->player;
  board = game_session->board;
  background = game_session->background;
  }

LevelEditor::~LevelEditor()
{
  delete game_session;
  for(int i = 0; i < TOTAL_BUTTONS; i++)
    delete button[i];
}

void LevelEditor::run()
  {
  load_level(1);

  while(game_session->state != ABORTED)
    {
    check_events();

    game_session->draw();
    // draw a grid
      {
        Board *board = game_session->board;
      for(int x = 1; x < board->board_width; x++)
        screen->fill_rect(board->board_x+x*TILE_W, board->board_y, 1, board->board_height*TILE_H,
                          128,128,128);
      for(int y = 1; y < board->board_height; y++)
        screen->fill_rect(board->board_x, board->board_y+y*TILE_H, board->board_width*TILE_W, 1,
                          128,128,128);
      }
    cursor.draw(game_session);
    draw_status_info();

    screen->update();
    }
  if(modified && show_yes_no_dialog("Want to save level changes?"))
    save_level();
  }

void LevelEditor::load_level(int nb)
  {
  if(nb <= 0)
    return;

  game_session->load_level(nb);

  cursor.x = (cursor.y = 0);
  modified = false;
  }

void LevelEditor::save_level()
  {
  board->save(game_session->levelset_absolute,
              //homedir+"/levels/"+game_session->levelset_filename,
              homedir+"/levels/user.pod",
              game_session->level_nb);
  }

void LevelEditor::draw_status_info()
  {
  draw_text("Level Editor", 22, 22, 14, 168, 0, 0, 252, 84, 84);
  draw_text("F1 for help", 430,22, 14, 255,165,0, 255,255,0);

  int sz;
  char str[256];
  // level number
  sprintf(str, "%d", game_session->level_nb);
  sz = draw_text("Level ", 520, 455, 12, 84, 252, 84, 0, 168, 0);
  draw_text(str, 520+sz, 455, 12, 252, 84, 252, 168, 0, 168);

  for(int i = 0; i < TOTAL_BUTTONS; i++)
    {
    button[i]->draw();

    sprintf(str, "%d", i);
    draw_text(str, 10+TILE_W+5+(TILE_W+30)*i, 448+TILE_H/2, 12,
              252, 252, 252, 168, 168, 168);
    }
  }

void LevelEditor::set_selected_button(int nb)
  {
  if(nb < 0 && nb >= TOTAL_BUTTONS)
    return;
  for(int i = 0; i < TOTAL_BUTTONS; i++)
    button[i]->set_selected(false);
  button[nb]->set_selected(true);
  current_tile = nb-1;
  }

void LevelEditor::check_events()
  {
  while(SDL_PollEvent(&event))
    {
    switch(event.type)
      {
      case SDL_KEYDOWN:	// key pressed
        switch(event.key.keysym.sym)
          {
          case SDLK_LEFT:
            cursor.x--;
            if(cursor.x < 0)
              cursor.x = 0;
            break;
          case SDLK_RIGHT:
            cursor.x++;
            if(cursor.x >= game_session->board->board_width)
              cursor.x = game_session->board->board_width-1;
            break;
          case SDLK_UP:
            cursor.y--;
            if(cursor.y < 0)
              cursor.y = 0;
            break;
          case SDLK_DOWN:
            cursor.y++;
            if(cursor.y >= game_session->board->board_height)
              cursor.y = game_session->board->board_height-1;
            break;
          case SDLK_RETURN:
            if(event.key.keysym.mod & KMOD_ALT)
              screen->toggle_fullscreen();
            break;
          // change level when +/- are pressed
          case SDLK_PLUS:
          case SDLK_KP_PLUS:
            load_level(game_session->level_nb+1);
            break;
          case SDLK_MINUS:
          case SDLK_KP_MINUS:
            load_level(game_session->level_nb-1);
            break;
          /* Let's place tiles on request. */
          case SDLK_KP0:
          case SDLK_0:
            modified = true;
            board->set_tile(cursor.x, cursor.y,NO_TILE);
            set_selected_button(0);
            break;
          case SDLK_KP1:
          case SDLK_1:
            modified = true;
            board->set_tile(cursor.x,cursor.y,ONE_TIME_TILE);
            set_selected_button(1);
            break;
          case SDLK_KP2:
          case SDLK_2:
            modified = true;
            board->set_tile(cursor.x,cursor.y,TWO_TIMES_TILE);
            set_selected_button(2);
            break;
          case SDLK_KP3:
          case SDLK_3:
            modified = true;
            board->set_tile(cursor.x,cursor.y,THREE_TIMES_TILE);
            set_selected_button(3);
            break;
          case SDLK_KP4:
          case SDLK_4:
            modified = true;
            board->set_tile(cursor.x,cursor.y,PERMANENT_TILE);
            set_selected_button(4);
            break;
          case SDLK_KP5:
          case SDLK_5:
            modified = true;
            board->set_tile(cursor.x,cursor.y,TELEPORT_TILE);
            set_selected_button(5);
            break;
          case SDLK_KP6:
          case SDLK_6:
            modified = true;
            player->set_pos(cursor.x,cursor.y);
            set_selected_button(6);
            break;
          case SDLK_F3:
            {
            save_level();

            Gameloop gameloop("user.pod", true);
            gameloop.load_level(game_session->level_nb);
            gameloop.run(true);
            }
            break;
          case SDLK_F4:
            save_level();
            break;
          default:
            break;
          }
        break;
      case SDL_KEYUP:	// key released
        switch(event.key.keysym.sym)
          {
          case SDLK_F1:
            pause_timers();
            background->draw();
            game_session->draw_leveleditor_info_msg();
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
      case SDL_MOUSEBUTTONDOWN:
#if 0  //old behavior
        if(event.button.button == SDL_BUTTON_LEFT)
          {
          int x = (event.button.x-BOARD_X) / TILE_W, y = (event.button.y-BOARD_Y) / TILE_H;
          if(x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT)
            {
            cursor.x = x;
            cursor.y = y;
            }
#endif
          // replace the current tile by the current selected button
        if(event.button.button == SDL_BUTTON_LEFT)
          {
          int x = (event.button.x-game_session->board->board_x) / TILE_W, y = (event.button.y-game_session->board->board_y) / TILE_H;
          if(x >= 0 && x < game_session->board->board_width && y >= 0 && y < game_session->board->board_height)
            {
            modified = true;
            cursor.x = x;
            cursor.y = y;
            if(current_tile == MAX_TILES)
              player->set_pos(x,y);
            else
              board->set_tile(x, y, Tile(current_tile));
            }
          else
            for(int i = 0; i < TOTAL_BUTTONS; i++)
              if(button[i]->check_event(event.button.x, event.button.y))
                set_selected_button(i);
          }
        // remove tiles if right button pressed
        else if(event.button.button == SDL_BUTTON_RIGHT)
          {
          int x = (event.button.x-game_session->board->board_x) / TILE_W, y = (event.button.y-game_session->board->board_y) / TILE_H;
          if(x >= 0 && x < game_session->board->board_width && y >= 0 && y < game_session->board->board_height) {
            modified = true;
            board->set_tile(x, y, NO_TILE);
          }
          }
        break;
      case SDL_QUIT:	// window closed
        game_session->state = ABORTED;
        break;
      default:
        break;
      }
    }
  }
