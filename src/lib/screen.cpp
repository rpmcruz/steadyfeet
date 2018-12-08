/***************************************************************************
                          ini_parser.h  -  to be included in all cpp files
                             -------------------
    begin                : un Oct 30 2004
    copyright            : (C) 2004 by Ricardo Cruz
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

#include "screen.h"

Screen* screen = 0;

Screen::Screen(int w, int h, int bpp, int flags)
  {
  set_video_mode(w, h, bpp, flags);
  }

Screen::~Screen()
  {
  SDL_FreeSurface(sdl_screen);
  }

int Screen::set_video_mode(int w_, int h_, int bpp, int flags)
  {
  sdl_screen = SDL_SetVideoMode(w_, h_, bpp, flags);
  if(sdl_screen == NULL)
    {
    std::cerr << "Error: Couldn't set video mode " << w_ << "x" << h_ << "x" << bpp
                                                   << std::endl <<
                 "Possible reason: " << SDL_GetError() << std::endl;
    return -1;
    }

  w = w_; h = h_;
  return 0;
  }

void Screen::toggle_fullscreen()
  {
  // unfortunately, this command doesn't work for non-X11 platforms:
  //SDL_WM_ToggleFullScreen(sdl_screen);

  Uint32 flags = sdl_screen->flags;
  flags ^= SDL_FULLSCREEN;
  Uint8 bpp = sdl_screen->format->BitsPerPixel;
  SDL_FreeSurface(sdl_screen);
  sdl_screen = SDL_SetVideoMode(w, h, bpp, flags);
  }

bool Screen::is_fullscreen()
  {
  return sdl_screen->flags & SDL_FULLSCREEN;
  }

void Screen::update()
  {
  SDL_Flip(sdl_screen);
  }


void Screen::update_part(int x, int y, int w, int h)
  {
  SDL_UpdateRect(sdl_screen, x, y, w, h);
  }

void Screen::fill_rect(int x, int y, int w, int h, int r, int g, int b, int a)
  {
  if(a == 255)
    {
    SDL_Rect rect = { x, y, w, h };
    SDL_FillRect(sdl_screen, &rect, SDL_MapRGB(sdl_screen->format, r, g, b));
    }
  else
    {  // color with alpha component
    SDL_Surface* temp = SDL_CreateRGBSurface(sdl_screen->flags, w, h,
               sdl_screen->format->BitsPerPixel,
               sdl_screen->format->Rmask, sdl_screen->format->Gmask,
               sdl_screen->format->Bmask, sdl_screen->format->Amask);

    SDL_Rect src;
    src.x = (src.y = 0); src.w = w; src.h = h;
    SDL_FillRect(temp, &src, SDL_MapRGB(sdl_screen->format, r, g, b));
    SDL_SetAlpha(temp, SDL_SRCALPHA, a);

    src.x = x; src.y = y; src.w = w; src.h = h;
    SDL_BlitSurface(temp, NULL, sdl_screen, &src);

    SDL_FreeSurface(temp);
    }
  }

void Screen::set_clip_area(int x, int y, int w, int h)
  {
  SDL_Rect r;
  r.x = x; r.y = y; r.w = w; r.h = h;
  SDL_SetClipRect(sdl_screen, &r);
  }

void Screen::unset_clip_area()
  {
  SDL_SetClipRect(sdl_screen, NULL);
  }

#define DOTS_SPEED 1/35
#define BOX_CLR  168,168,168
#define DOTS_CLR 255,255,0

void Screen::draw_animated_filled_rect(int x, int y, int w, int h, int dots_nb)
  {
  // draw a filled semi-transparent rectangle
  fill_rect(x, y, w, h, 128, 128, 0, 128);

  // draw a box around it with some white pixels going around animation
  fill_rect(x, y, w, 1, BOX_CLR);
  fill_rect(x + w, y, 1, h, BOX_CLR);
  fill_rect(x, y + h, w, 1, BOX_CLR);
  fill_rect(x, y, 1, h, BOX_CLR);

  // drawing the dots for the animation
  for(int i = 0; i < dots_nb; i++)
    {
    int pos = ((SDL_GetTicks()*DOTS_SPEED)%(2*w+2*h)) + i*((2*w+2*h)/dots_nb);
    if(pos > 2*w+2*h)
      pos -= 2*w+2*h;

    int pos_x = 0, pos_y = 0;
    if(pos > 0 && pos < w)
      pos_x = pos;
    else if(pos >= w && pos < w+h)
      {
      pos_x = w;
      pos_y = pos - w;
      }
    else if(pos >= w+h && pos < 2*w+h)
      {
      pos_x = w - pos + (w+h);
      pos_y = h;
      }
    else if(pos >= 2*w+h && pos < 2*w+2*h)
      pos_y = h - pos + (2*w+h);
    
    fill_rect(x + pos_x, y + pos_y, 1, 1,DOTS_CLR);
    }
  }

#define LOOP_DELAY 20.0

void Screen::shrink_fade(int x, int y, int fade_time)
  {
  float left_inc  = x / ((float)fade_time / LOOP_DELAY);
  float right_inc = (screen->w - x) / ((float)fade_time / LOOP_DELAY);
  float up_inc    = y / ((float)fade_time / LOOP_DELAY);
  float down_inc  = (screen->h - y) / ((float)fade_time / LOOP_DELAY);
                                                                                
  float left_cor = 0, right_cor = 0, up_cor = 0, down_cor = 0;
                                                                                
  while(left_cor < x && right_cor < screen->w - x &&
      up_cor < y && down_cor < screen->h - y)
    {
    left_cor  += left_inc;
    right_cor += right_inc;
    up_cor    += up_inc;
    down_cor  += down_inc;

    fill_rect(0, 0, (int)left_cor, h, 0,0,0);  // left side
    fill_rect(w - (int)right_cor, 0, (int)right_cor, h, 0,0,0);  // right side
    fill_rect(0, 0, w, (int)up_cor, 0,0,0);  // up side
    fill_rect(0, h - (int)down_cor, w, (int)down_cor+1, 0,0,0);  // down side
    update();

    SDL_Delay(int(LOOP_DELAY));
    }
  }
