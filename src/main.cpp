/***************************************************************************
                          main.cpp  -  the usual init/end code
                             -------------------
    begin                : Tue Feb 15 2005
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

#include "lib/screen.h"
#include "lib/file_access.h"
#include "lib/menu.h"
#include "gameloop.h"
#include "leveleditor.h"
#include "title.h"
#include "highscore.h"

#ifndef DATA_PREFIX
  #define DATA_PREFIX "./data/"
    #warning DATA_PREFIX should have been declared in Makefile.
    #warning If installed, the game will not work!
#endif

#define VERSION "0.0.2"

/* Check if directory exists. */
bool dir_exists(const std::string& dirname);

int main(int argc, char* argv[])
  {
  /* Parsing command-line arguments. */
  bool fullscreen_flag = true;
  bool leveleditor_flag = false;
  std::string levelfile_passed;
  for(int i = 1; i < argc; i++)
    {
    if(!strcmp(argv[i], "--help"))
      {
      std::cout << "Usage: " << argv[0] << " [OPTIONS] [LEVEL_FILE]\n\n"
                   "Options:\n"
                   "\t--windowed\tRuns the game in window mode.\n"
                   "\t--leveleditor\tGoes directly to the level editor.\n"
                   "\t--use-joy NUMBER\tUses the joystick NUMBER.\n"
                   "\t--version, -v\tPrints the game version.\n"
                   "\t--help\tPrints this help text.\n"
                   "If you want to load a custom level file, just copy it into\n"
                   "the levels folder (either of your home or system) and pass\n"
                   "the level's filename as an argument.\n\n";
      return 0;
      }
    if(!strcmp(argv[i], "--version") || !strcmp(argv[i], "-v"))
      {
      std::cout << "Steadyfeet version " VERSION "\n\n";
      return 0;
      }
    if(!strcmp(argv[i], "--windowed"))
      fullscreen_flag = false;
    else if(!strcmp(argv[i], "--leveleditor"))
      leveleditor_flag = true;
#if 0
    else if(!strcmp(argv[i], "--use-joy"))
      {
      i++;
      if(argv[i][0] >= '0' && argv[i][0] <= '9')
        joy_nb = argv[i][0] - '0';
      else
        std::cerr << "Number was not feed when used --use-joy: " << argv[i] << std::endl;
      }
#endif
    else if(argv[i][0] == '-')
      {
      std::cout << "Unknown argument passed: " << argv[i] << std::endl;
      return 1;
      }
    else
      {
      levelfile_passed = argv[i];
      std::cout << "File passed: " << levelfile_passed << std::endl;
/*      if(!file_exists(levelfile_passed))
        {
        std::cerr << "Error: file passed \"" << levelfile_passed << "\" was not found.\n";
        return 1;
        }*/
      }
    }

  /* Do some magic to find data dir */
  if(find_datadir(argv[0]) == false)
    {
    std::cerr << "Error: could not find data directory. Bailing out...\n";
    return 1;
    }
  std::cout << "Using data dir: " << datadir << std::endl;

  /* Initializations */
  if(SDL_Init(SDL_INIT_VIDEO) == -1)
    {
    std::cerr << "Error: Could not initializate video.\n"
                 "SDL error: " << SDL_GetError() << std::endl;
    return 1;
    }

  screen = new Screen(640, 480, 16, SDL_HWSURFACE | SDL_FULLSCREEN*fullscreen_flag);

  SDL_WM_SetCaption("SteadyFeet", "SteadyFeet");

#if 0
  /* Initializating gamepads. */
  if(SDL_InitSubSystem(SDL_INIT_JOYSTICK) == -1)
    std::cerr << "Warning: Could not initializate joystick.\n"
                 "SDL error: " << SDL_GetError() << std::endl;
  else if(SDL_NumJoysticks() >= joy_nb+1)
    {
    std::cout << "Using joystick number " << joy_nb << std::endl;
    SDL_JoystickEventState(SDL_ENABLE);
    joystick = SDL_JoystickOpen(joy_nb);
    }
#endif
  // feed random number generator
  srand(SDL_GetTicks());

  // allow player to hold a key:
//  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
  // enables Unicode so that we can read keys name (ie. for highscores):
  SDL_EnableUNICODE(1);

  /* Load some stuff. */
  setup_menus();
  read_highscores();

  /* Running the game */
  if(leveleditor_flag)
    {
    LevelEditor leveleditor;
    if(!levelfile_passed.empty())
      leveleditor.load_levelset(levelfile_passed);
    leveleditor.run();
    }
  else if(!levelfile_passed.empty())
    {
    Gameloop gameloop;
    gameloop.load_levelset(levelfile_passed);
    gameloop.run();
    }
  else
    show_title();



  /* Finalizations */
  save_highscores();
  free_menus();
#if 0
  if(SDL_NumJoysticks())
    SDL_JoystickClose(joystick);
#endif
  delete screen;
  SDL_Quit();

  return 0;
  }
