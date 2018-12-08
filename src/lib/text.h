/***************************************************************************
                          font.h  -  draws text
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
 
#ifndef FONT_H
#define FONT_H

/* Fonts are drawn in a scalable graphics kinda of way.
   It draws the text based in lines defined in the cpp file that are
   stored in the enum on the same file. */

/* Allignment is relative to the X point. */
enum Allignment {
  LEFT_ALLIGN,
  CENTER_ALLIGN,
  RIGHT_ALLIGN
  };

/* Draws "text" into screen.
   Returns the width in pixels. */
int draw_text(const char* text, int x, int y, int size,
              int top_red, int top_green, int top_blue,
              int bottom_red, int bottom_green, int bottom_blue,
              Allignment allignment = LEFT_ALLIGN);

#endif /*FONT_H*/
