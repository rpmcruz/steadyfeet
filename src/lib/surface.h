/***************************************************************************
                          surface.h  -  stores an "image"
                             -------------------
    begin                : Šun Oct 30 2004
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

#ifndef SURFACE_H
#define SURFACE_H

/* Only BMP are supported. Support for others is easily done, but I
   want to use only SDL. */

struct SDL_Surface;

class Surface
  {
  public:
    Surface(const std::string& filename);
    Surface(const std::string& filename, int red, int green, int blue);
    ~Surface();

    void draw(int x, int y);
    void draw_part(int x, int y, int sx, int sy, int sw, int sh);

    // emulate a Sprite behavior. Will call draw_part().
    void draw_frame(int x, int y, int frame, int total_frames);

    /* Semi-transparent effect. */
    // we are using 16-bit, so this won't work
    void set_transparent(int alpha);

    SDL_Surface* get_sdl_surface()  // avoid this call
      { return sdl_surface; }

    int w, h;

  private:
    void load_image(const std::string& filename);
    void set_colorkey(int red, int green, int blue);

    SDL_Surface* sdl_surface;
  };

#endif /*SURFACE_H*/
