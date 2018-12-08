/***************************************************************************
                          background.cpp  -  draws the background
                             -------------------
    begin                : Wed Feb 16 2005
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
#include <stdlib.h>
#include <iostream>

#include "background.h"
#include "lib/screen.h"

Background::Background(bool with_border)
  : border(with_border)
  {
  if(with_border)
    {
    area_x = BORDER_X;
    area_y = BORDER_Y;
    area_w = BORDER_W;
    area_h = BORDER_H;
    }
  else
    {
    area_x = (area_y = 0);
    area_w = screen->w;
    area_h = screen->h;
    }
  }

Background::~Background()
  {
  }

#define MAX_VEL 10
#define FULLSPEED_VEL 50

void Background::reset()
  {
  // choose a random speed
  vel_x = ((rand() % (MAX_VEL*20)) / 10.0) - (MAX_VEL/2);
  vel_y = ((rand() % (MAX_VEL*20)) / 10.0) - (MAX_VEL/2);

  // choose some random positions
  for(int i = 0; i < STARS_NB; i++)
    {
    pos_x[i] = rand() % area_w + area_x;
    pos_y[i] = rand() % area_h + area_y;
    }
  }

void Background::go_fullspeed()
  {
  if(vel_x > 0)
    vel_x = FULLSPEED_VEL;
  else if(vel_x < 0)
    vel_x = -FULLSPEED_VEL;
  if(vel_y > 0)
    vel_y = FULLSPEED_VEL;
  else if(vel_y < 0)
    vel_y = -FULLSPEED_VEL;
  }

void Background::freeze()
  {
  vel_x = (vel_y = 0);
  }

void Background::update(float elapsed_time)
  {
  float vx = vel_x, vy = vel_y;
  for(int i = 0; i < STARS_NB; i++)
    {
    if(i == STARS_NB / 2)  // use other speed for half of them
      {
      vx -= vx * 0.10;
      vy -= vy * 0.10;
      }
    pos_x[i] += vx * elapsed_time;
    pos_y[i] += vy * elapsed_time;

    if(pos_x[i] < area_x)
      pos_x[i] += area_w;
    else if(pos_x[i]+area_x > area_w)
      pos_x[i] -= area_w;
    if(pos_y[i] < area_y)
      pos_y[i] += area_h;
    else if(pos_y[i]+area_y > area_h)
      pos_y[i] -= area_h;
    }
  }

static const int border_color[][3] = {
  { 252, 252, 252 },
  { 168, 0, 0 },
  { 168, 168, 168 }
  };

void Background::draw()
  {
  screen->fill_rect(0,0,screen->w,screen->h,0,0,0);

  SDL_Rect rect;
  Uint32 color = SDL_MapRGB(screen->sdl_screen->format,255,255,255);
  for(int i = 0; i < STARS_NB; i++)
    {
    if(i == STARS_NB / 2)  // use other color for half of them
      color = SDL_MapRGB(screen->sdl_screen->format,168,168,168);
    rect.x = (int)pos_x[i]; rect.y = (int)pos_y[i]; rect.w = (rect.h = 1);
    SDL_FillRect(screen->sdl_screen, &rect, color);
    }

  // draw the borders around the screen
  if(border)
    {
    int w, offset = area_x;
    for(int i = 0; i < 4; i++)
      {
      if(i == 1)
        w = 4;
      else
        w = 2;

      screen->fill_rect(offset, offset, BORDER_W-(offset*2), w,
                        border_color[i][0],border_color[i][1],border_color[i][2]);
      screen->fill_rect(BORDER_W-w-offset, offset, w, BORDER_H-(offset*2),
                        border_color[i][0],border_color[i][1],border_color[i][2]);
      screen->fill_rect(offset, BORDER_H-w-offset, BORDER_W-(offset*2), w,
                         border_color[i][0],border_color[i][1],border_color[i][2]);
      screen->fill_rect(offset, offset, w, BORDER_H-(offset*2),
                         border_color[i][0],border_color[i][1],border_color[i][2]);
      offset += w;
      }
    }
  }
