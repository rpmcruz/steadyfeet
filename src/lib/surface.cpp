/***************************************************************************
                          surface.cpp  -  stores an "image"
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

#include "surface.h"
#include "screen.h"

Surface::Surface(const std::string& filename)
  {
  load_image(filename);
  }

Surface::Surface(const std::string& filename, int red, int green, int blue)
  {
  load_image(filename);
  set_colorkey(red, green, blue);
  }

Surface::~Surface()
  {
  SDL_FreeSurface(sdl_surface);
  }

void Surface::load_image(const std::string& filename)
  {
  sdl_surface = SDL_LoadBMP(filename.c_str());
  if(sdl_surface == NULL)
    {
    std::cerr << "Error: Couldn't load image " << filename << std::endl
              << "Possible reason: " << SDL_GetError() << std::endl;
    return;
    }
  w = sdl_surface->w;
  h = sdl_surface->h;
  }

void Surface::set_colorkey(int red, int green, int blue)
  {
  if(SDL_SetColorKey(sdl_surface, SDL_SRCCOLORKEY | SDL_RLEACCEL,
                     SDL_MapRGB(sdl_surface->format, red, green, blue)) == -1)
    std::cerr << "Error: could not set colorkey.\n"
                 "Possible reason: " << SDL_GetError() << std::endl;
  }

void Surface::draw(int x, int y)
  {
  if(x + w < 0 || x > screen->w || y + h < 0 || y > screen->h)
    return;   // drawing out of screen range

  SDL_Rect rect;
  rect.x = x; rect.y = y;
  SDL_BlitSurface(sdl_surface, NULL, screen->sdl_screen, &rect);
  }

void Surface::draw_part(int x, int y, int sx, int sy, int sw, int sh)
  {
  if(sx > w || sx + sw > w || sy > h || sy + sh > h)
    std::cerr << "Warning: part of image to draw bigger than the image itself.\n";

  if(x + sw < 0 || x > screen->w || y + sh < 0 || y > screen->h)
    return;   // drawing out of screen range

  SDL_Rect src = { sx, sy, sw, sh };
  SDL_Rect dst;
  dst.x = x; dst.y = y;
  SDL_BlitSurface(sdl_surface, &src, screen->sdl_screen, &dst);
  }

void Surface::draw_frame(int x, int y, int frame, int total_frames)
  {
  draw_part(x, y, frame * (w / total_frames), 0, w/total_frames, h);
  }

void Surface::set_transparent(int alpha)
  {
  SDL_SetAlpha(sdl_surface, SDL_SRCALPHA, alpha);
  }
