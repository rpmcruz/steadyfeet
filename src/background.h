/***************************************************************************
                          background.h  -  draws the background
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
 
#ifndef BACKGROUND_H
#define BACKGROUND_H

#define STARS_NB 80

#define BORDER_X 2
#define BORDER_Y 2
#define BORDER_W 640
#define BORDER_H 445

class Background
  {
  public:
    Background(bool with_border);
    ~Background();

    /* Sets stars pos and velocity. */
    void reset();

    void update(float elapsed_time);
    void draw();

    /* Used for special ocasions like level finished. */
    void freeze();
    void go_fullspeed();

  private:
    float pos_x[STARS_NB], pos_y[STARS_NB];
    float vel_x, vel_y;

    int area_x, area_y, area_w, area_h;

    bool border;
  };

#endif /*BACKGROUND_H*/
