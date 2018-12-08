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

#ifndef SCREEN_H
#define SCREEN_H

struct SDL_Surface;

class Screen
{
  public:
    Screen(int w, int h, int bpp, int flags);
    ~Screen();

    /* Toggle from window to fullscreen mode. */
    void toggle_fullscreen();
    bool is_fullscreen();

    void update();
    void update_part(int x, int y, int w, int h);

    /* Graphical operations. */
    void fill_rect(int x, int y, int w, int h, int r, int g, int b, int a = 255);
    // this draws a semi-transparent rectangle with some lights moving around. ;-)
    void draw_animated_filled_rect(int x, int y, int w, int h, int dots_nb);

    void shrink_fade(int x, int y, int fade_time);

    // more advanced stuff
    void set_clip_area(int x, int y, int w, int h);
    void unset_clip_area();

    int w, h;

    SDL_Surface* sdl_screen; // evil: avoid access to this

  private:
    int set_video_mode(int w, int h, int bpp, int flags);
};

extern Screen* screen;

#endif
