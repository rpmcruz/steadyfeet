/***************************************************************************
                          highscore.h  -  takes care of handling highscores
                             -------------------
    begin                : Thu Feb 24 2005
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
 
#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#include <string>

class Background;

void read_highscores();
void save_highscores();

void add_highscore(unsigned int score, Background* background);
void print_highscore();

#endif /*HIGHSCORE_H*/
