/***************************************************************************
                          highscore.cpp  -  takes care of handling highscores
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

#include <SDL.h>
#include <iostream>
#include <fstream>

#include "lib/file_access.h"
#include "lib/screen.h"
#include "lib/text.h"
#include "lib/timer.h"
#include "highscore.h"
#include "background.h"

static char high_players[10][3];
static unsigned int high_score[10];
static bool modified;

static SDL_Event event;

inline int power(int b, unsigned int e)
  {
  if(e == 1 || e == 0)
    return 1;
  return b*power(b, e-1);
  }

inline int my_atoi(char* str)
  {
  int n = 0;
  int size;
  for(size = 0; str[size] != '\0'; size++) ;
  for(int i = 0; str[i] != '\0'; i++)
    n += (str[i]-'0') * power(10, size-i);
  return n;
  }

void read_highscores()
  {
  modified = false;

  /* Let's first reset out highscores array. */
  for(int i = 0; i < 10; i++)
    {
    high_score[i] = 0;
    for(int j = 0; j < 3; j++)
      high_players[i][j] = ' ';
    }

  /* Now let's read from file. */
  std::string filename = homedir + "/highscores";
  if(!file_exists(filename))
    return;

  char str[256], c;  // auxiliars
  std::ifstream file(filename.c_str());
  if(!file)
    {
    std::cerr << "Warning: could not get read access highscore file: "
              << filename << std::endl;
    return;
    }
  for(int i = 0; i < 10 && !file.eof(); i++)
    {
    for(int j = 0; j < 3; j++)
      {
      file.get(c);
      while(c == '\n')
        file.get(c);
      high_players[i][j] = c;
      }
    file >> str;
    high_score[i] = my_atoi(str);
    file.ignore();
    }

  /* Let's just print our database for debugging. */
//#if 0
  std::cerr << "Highscores: \n";
  for(int i = 0; i < 10; i++)
    {
    for(int j = 0; j < 3; j++)
      std::cerr << high_players[i][j];
    std::cerr << " - " << high_score[i] << std::endl;
    }
  std::cerr << std::endl;
//#endif
  }

void save_highscores()
  {
  if(!modified)
    return;  // no need to save
  std::string filename = homedir + "/highscores";
std::cerr << "saving highscore: " << filename << std::endl;
  std::ofstream file(filename.c_str());
  if(!file)
    {
    std::cerr << "Warning: Could not get write access to highscore file: "
              << filename << std::endl;
    return;
    }

  for(int i = 0; i < 10; i++)
    {
    for(int j = 0; j < 3; j++)
      file << high_players[i][j];
    file << ' ' << high_score[i] << '\n';
    }
  }

void add_highscore(unsigned int score, Background* background, bool completed_game)
  {
  /* Firstly, let's check if the player got a new highscore. */
  int step = 0;
  int place_nb;
  for(place_nb = 0; place_nb < 10; place_nb++)
    if(high_score[place_nb] < score)
      break;
  if(place_nb == 10)
    return;  // didn't enter in the list
  modified = true;

  /* Let's just prepare the array first. For instance, put all names
     one down and erase the current's place name. */
  for(int i = 9; i > place_nb; i--)
    {
    high_score[i] = high_score[i-1];
    for(int j = 0; j < 3; j++)
      high_players[i][j] = high_players[i-1][j];
    }
  for(int i = 0; i < 3; i++)
    high_players[place_nb][i] = ' ';
  high_score[place_nb] = score;

  start_framerate();
  Timer timer;

  int letter = 0;
  bool done = false;
  while(!done)
    {
    if(background != NULL)
      {
      background->update(get_framerate());
      background->draw();
      }
    else
      screen->fill_rect(0,0,screen->w,screen->h, 0,0,0);
    if(step < 2)
      {
      draw_text("- High Score -", screen->w/2, 180, 18, 255,0,0, 255,0,0, CENTER_ALLIGN);
      if(completed_game)
        draw_text("Congratulations for Victory", screen->w/2, 240, 18, 255,165,0, 255,255,0, CENTER_ALLIGN);
      {
      char str[4];
      for(int i = 0; i < 3; i++)
        {
        if(high_players[place_nb][i] == ' ')
          str[i] = '_';
        else
          str[i] = high_players[place_nb][i];
        }
      str[3] = '\0';
      draw_text(str, screen->w/2, 300, 16, 0,255,0, 0,0,255, CENTER_ALLIGN);
      }
      }

    while(SDL_PollEvent(&event))
      {
      if(event.type == SDL_QUIT && (event.type == SDL_KEYUP &&
        (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_RETURN)))
        step++;
      else if(event.type == SDL_KEYDOWN)
        if((event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9) ||
           (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z))
          {
          if(step == 0 && (event.key.keysym.unicode & 0xFF80) == 0)
            {
            high_players[place_nb][letter] = event.key.keysym.unicode & 0x7F;
            letter++;
            if(letter == 3)
              {
              step++;
              timer.start(1500);
              }
            }
          }
      }

    if(step == 1 && !timer.check())
      {
      step++;
      timer.start(4000);
      }
    else if(step == 2)
      {
      print_highscore();
      if(!timer.check())
        done = true;
      }

    screen->update();
    }
  }

static const int name_color[10][6] = {
  { 84,84,252, 0,0,168 },
  { 84,84,252, 0,0,168 },
  { 84,252,84, 0,168,0 },
  { 84,252,84, 0,168,0 },
  { 84,252,84, 0,168,0 },
  { 84,252,252, 0,168,168 },
  { 84,252,252, 0,168,168 },
  { 84,252,252, 0,168,168 },
  { 252,84,84, 168,0,0 },
  { 252,84,84, 168,0,0 }
  };
#define NAME_COLOR(x)  name_color[x][0], name_color[x][1], name_color[x][2], \
                       name_color[x][3], name_color[x][4], name_color[x][5]

void print_highscore()
  {
  draw_text("Top Scores", screen->w/2,160, 18, 252,252,84, 168,84,0, CENTER_ALLIGN);
  char str[4];
  for(int i = 0; i < 10; i++)
    {
    for(int j = 0; j < 3; j++)
      str[j] = high_players[i][j];
    str[3] = '\0';
    draw_text(str, 200, 200+(i*18), 15, NAME_COLOR(i));
    sprintf(str, "%d", high_score[i]);
    draw_text(str, 440, 200+(i*20), 15, 252,84,252, 168,0,168, RIGHT_ALLIGN);
    }
  }
