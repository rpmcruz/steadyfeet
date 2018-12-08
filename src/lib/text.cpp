/***************************************************************************
                          font.cpp  -  draws text
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
#include <iostream>

#include "text.h"
#include "screen.h"

// our primitives funcs
void draw_hor_line(int x, int y, int w, int r, int g, int b);
void draw_ver_line(int x, int y, int h, int r, int g, int b);
// hor_dir 1 for right and -1 for left. ver_dir 1 for down and -1 for up.
void draw_diag_line(int x, int y, int w, int hor_dir, int ver_dir, int r, int g, int b);

int my_strlen(const char* str);

/* This enum contains the lines to draw that are then used on each
   characters info. */
enum  DrawLine {
  NO_LINE    = 0x000,  // 
  TOP_HOR    = 0x001,  // -
  BOT_HOR    = 0x002,  // _
  LEFT_VER   = 0x004,  // |
  RIGHT_VER  = 0x008,  //   |
  SLASH_DIAG = 0x010,  //  /
  WINDS_DIAG = 0x020   //
  };

/* Here goes now the information of how to draw each character. */
// we have only characters for letters and numbers, but if you want special characters
// you just need to add them (to the end!), no further coding required
static int char_map[][5] = {
//{ char,top left box,     top right box,     bottom right box, bottom left box },
  { 'A', TOP_HOR|BOT_HOR|LEFT_VER, TOP_HOR|BOT_HOR|RIGHT_VER, RIGHT_VER, LEFT_VER },
  { 'B', TOP_HOR|LEFT_VER|BOT_HOR, TOP_HOR|SLASH_DIAG, BOT_HOR|WINDS_DIAG, BOT_HOR|LEFT_VER },
  { 'C', LEFT_VER|TOP_HOR, TOP_HOR, BOT_HOR, LEFT_VER|BOT_HOR },
//  { 'C', SLASH_DIAG, TOP_HOR, BOT_HOR, WINDS_DIAG },
  { 'D', TOP_HOR|LEFT_VER, TOP_HOR|RIGHT_VER, SLASH_DIAG, BOT_HOR|LEFT_VER },
//  { 'D', TOP_HOR|LEFT_VER, WINDS_DIAG, SLASH_DIAG, BOT_HOR|LEFT_VER },
  { 'E', TOP_HOR|LEFT_VER, TOP_HOR|SLASH_DIAG, BOT_HOR, BOT_HOR|LEFT_VER },
  { 'F', TOP_HOR|LEFT_VER|BOT_HOR, TOP_HOR, NO_LINE, LEFT_VER },
  { 'G', SLASH_DIAG, TOP_HOR, TOP_HOR|BOT_HOR|RIGHT_VER, BOT_HOR|LEFT_VER },
  { 'H', LEFT_VER|BOT_HOR, RIGHT_VER|BOT_HOR, RIGHT_VER, LEFT_VER },
  { 'I', RIGHT_VER, NO_LINE, NO_LINE, RIGHT_VER },
  { 'J', NO_LINE, RIGHT_VER, BOT_HOR|RIGHT_VER, BOT_HOR|LEFT_VER },
  { 'K', LEFT_VER|BOT_HOR, SLASH_DIAG, WINDS_DIAG, LEFT_VER },
  { 'L', LEFT_VER, NO_LINE, BOT_HOR, BOT_HOR|LEFT_VER },
  { 'M', LEFT_VER|WINDS_DIAG, SLASH_DIAG|RIGHT_VER, RIGHT_VER, LEFT_VER  },
  { 'N', LEFT_VER|WINDS_DIAG, RIGHT_VER, RIGHT_VER|WINDS_DIAG, LEFT_VER },
  { 'O', LEFT_VER|TOP_HOR, TOP_HOR|RIGHT_VER, RIGHT_VER|BOT_HOR, LEFT_VER|BOT_HOR },
  { 'P', LEFT_VER|TOP_HOR|BOT_HOR, TOP_HOR|BOT_HOR|RIGHT_VER, NO_LINE, LEFT_VER },
  { 'Q', LEFT_VER|TOP_HOR, TOP_HOR|RIGHT_VER, WINDS_DIAG|RIGHT_VER|BOT_HOR, LEFT_VER|BOT_HOR },
  { 'R', LEFT_VER|TOP_HOR|BOT_HOR, TOP_HOR|BOT_HOR|RIGHT_VER, WINDS_DIAG, LEFT_VER },
  { 'S', LEFT_VER|TOP_HOR|BOT_HOR, TOP_HOR|BOT_HOR, RIGHT_VER|BOT_HOR, BOT_HOR },
  { 'T', TOP_HOR|RIGHT_VER, TOP_HOR, LEFT_VER, NO_LINE },
  { 'U', LEFT_VER, RIGHT_VER, RIGHT_VER|BOT_HOR, BOT_HOR|LEFT_VER },
  { 'V', LEFT_VER, RIGHT_VER, SLASH_DIAG, BOT_HOR|LEFT_VER },
  { 'W', LEFT_VER, RIGHT_VER, RIGHT_VER|WINDS_DIAG, LEFT_VER|SLASH_DIAG },
  { 'X', WINDS_DIAG, SLASH_DIAG, WINDS_DIAG, SLASH_DIAG },
  { 'Y', WINDS_DIAG, SLASH_DIAG, NO_LINE, RIGHT_VER },
  { 'Z', TOP_HOR, TOP_HOR|SLASH_DIAG, BOT_HOR, BOT_HOR|SLASH_DIAG },
  { '0', LEFT_VER|TOP_HOR, TOP_HOR|RIGHT_VER, RIGHT_VER|BOT_HOR, LEFT_VER|BOT_HOR },
  { '1', RIGHT_VER, NO_LINE, NO_LINE, RIGHT_VER },
//  { '1', SLASH_DIAG|RIGHT_VER, NO_LINE, NO_LINE, RIGHT_VER },
  { '2', TOP_HOR|BOT_HOR, TOP_HOR|BOT_HOR|RIGHT_VER, BOT_HOR, BOT_HOR|LEFT_VER },
  { '3', TOP_HOR|BOT_HOR, TOP_HOR|BOT_HOR|RIGHT_VER, BOT_HOR|RIGHT_VER, BOT_HOR },
  { '4', LEFT_VER|BOT_HOR, BOT_HOR|RIGHT_VER, RIGHT_VER, NO_LINE },
  { '5', LEFT_VER|TOP_HOR|BOT_HOR, TOP_HOR|BOT_HOR, RIGHT_VER|BOT_HOR, BOT_HOR },
  { '6', TOP_HOR|BOT_HOR|LEFT_VER, TOP_HOR|BOT_HOR, RIGHT_VER|BOT_HOR, LEFT_VER|BOT_HOR },
  { '7', TOP_HOR, TOP_HOR|RIGHT_VER, RIGHT_VER, NO_LINE },
  { '8', TOP_HOR|BOT_HOR|LEFT_VER, TOP_HOR|BOT_HOR|RIGHT_VER, RIGHT_VER|BOT_HOR, LEFT_VER|BOT_HOR },
  { '9', TOP_HOR|BOT_HOR|LEFT_VER, TOP_HOR|BOT_HOR|RIGHT_VER, RIGHT_VER|BOT_HOR, BOT_HOR },
  { '-', BOT_HOR, BOT_HOR, NO_LINE, NO_LINE },
  { '_', NO_LINE, NO_LINE, BOT_HOR, BOT_HOR },
  { '?', SLASH_DIAG, WINDS_DIAG|BOT_HOR, LEFT_VER, NO_LINE },
  { '\0', NO_LINE, NO_LINE, NO_LINE, NO_LINE }  // last case
  };

int draw_text(const char* text, int x, int y, int size,
              int top_red, int top_green, int top_blue,
              int bot_red, int bot_green, int bot_blue,
              Allignment allign)
  {
  int ori_x = x;

  // let's calculate the allignment
  if(allign != LEFT_ALLIGN)
    {  // no need to calculate for left allignment
    int text_size = my_strlen(text) * (size+(size/4));
    if(allign == CENTER_ALLIGN)
      x -= text_size / 2;
    if(allign == RIGHT_ALLIGN)
      x -= text_size;
    }    

  for(int i = 0; text[i] != '\0'; i++, x += size/4)
    {
    int ch = text[i];

    if(ch == ' ')
      {
      x += size;
      continue;
      }
    if(ch == '\n')
      {
      x = ori_x - size/4;
      y += size + size/2;
      continue;
      }

    // firstly, let's discover where in the table this character is
    int entry = -1;

    // upper-case letters
    if(ch >= 'a' && ch <= 'z')
      ch -= 'a' - 'A';

    if(ch >= 'A' && ch <= 'Z')
      entry = ch - 'A';
    else if(ch >= '0'&& ch <= '9')
      entry = ch-'0' + 'Z'-'A' + 1;
    else
      for(int j = 'Z'-'A' + '9'-'0'; char_map[j][0] != '\0'; j++)
        if(char_map[j][0] == ch)
          entry = j;

    // this character unknown, just give up
    if(entry == -1)
      continue;

    // now that we have found it, draw it!
    int r,g,b;
    r = bot_red; g = bot_green; b = bot_blue;
    int x_ = x, y_ = y+size/2;
    for(int side = 4; side > 0; side--)
      {
      if(side == 3)
        x_ += size/2;
      else if(side == 2)
        { r = top_red; g = top_green; b = top_blue;
          y_ = y; }
      else if(side == 1)
        x_ = x;

      if(char_map[entry][side] & TOP_HOR)
        draw_hor_line(x_, y_, size/2, r,g,b);
      if(char_map[entry][side] & BOT_HOR)
        draw_hor_line(x_, y_+size/2, size/2, r,g,b);
      if(char_map[entry][side] & LEFT_VER)
        draw_ver_line(x_, y_, size/2, r,g,b);
      if(char_map[entry][side] & RIGHT_VER)
        draw_ver_line(x_+size/2, y_, size/2, r,g,b);
      if(char_map[entry][side] & SLASH_DIAG)
        draw_diag_line(x_+size/2, y_, size/2, -1, 1, r,g,b);
      if(char_map[entry][side] & WINDS_DIAG)
        draw_diag_line(x_, y_, size/2, 1, 1, r,g,b);
      }

    // adds the space of this character
//    if(char_map[entry][2] == NO_LINE && char_map[entry][3] == NO_LINE)
//      x += size/2;
//    else
      x += size;
    }
  return x-ori_x;
  }

int my_strlen(const char* str)
  {
  int size = 0;
  int i,j;
  for(i = (j = 0);; i++, j++)
    {
    if(str[i] == '\n' || str[i] == '\0')
      {
      if(size < j)
        size = j;
      j = 0;
      }
    if(str[i] == '\0')
      break;
    }
  return size;
  }

/* Since SDL doesn't come with primitives right from the box, let's
   implement ours. */

// maybe instead of making using of SDL_FillRect(), we should blit the
// pixels manually -- dunno what's faster

void putpixel(SDL_Surface* surface, int x, int y, Uint32 pixel);
void draw_filled_rect(int x, int y, int w, int h, int r, int g, int b);

void draw_ver_line(int x, int y, int h, int r, int g, int b)
  { draw_filled_rect(x, y, 1, h, r, g, b); }

void draw_hor_line(int x, int y, int w, int r, int g, int b)
  { draw_filled_rect(x, y, w, 1, r, g, b); }

void draw_diag_line(int x, int y, int w, int hor_dir, int ver_dir, int r, int g, int b)
  {
  for(int i = 0; i <= w; i++)
    draw_filled_rect(x+(i*hor_dir), y+(i*ver_dir), 1, 1, r,g,b);
  }

void draw_filled_rect(int x, int y, int w, int h, int r, int g, int b)
  {
  screen->fill_rect(x,y,w,h,r,g,b);
  }
