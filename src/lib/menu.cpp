/***************************************************************************
                          menu.cpp  -  responsible for the menu
                             -------------------
    begin                : Sat Feb 19 2005
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
#include <assert.h>

#include "menu.h"
#include "text.h"
#include "screen.h"
#include "surface.h"
#include "file_access.h"

#define ENTRY_SPACE 2
//#define ENTRY_FONT 16
//#define ENTRY_SIZE (ENTRY_FONT+15)
//#define SUBENTRY_FONT 13
//#define SUBENTRY_SIZE (SUBENTRY_FONT+8)

static SDL_Event event;

int Menu::idle = false;

bool show_yes_no_dialog(const char* text)
  {
  Menu menu("yesno");
  menu.add_entry("Yes", 1, 16);
  menu.add_entry("No", 0, 16);
  while(true)
    {
    screen->fill_rect(0,0,screen->w,screen->h, 0,0,0);
    draw_text(text, screen->w/2, 100, 12, 80,80,80, 168,168,168, CENTER_ALLIGN);
    menu.show();
    screen->update();
    if(menu.has_selected())
      break;
    }
  int sel = menu.get_selected();
  if(sel == BACK_ID)
    return false;
  return sel;
  }

MenuEntry::MenuEntry(const std::string& str, int id, int font_size)
  : label(str), id(id), font_size(font_size)
  { }

MenuEntry::~MenuEntry()
  { }

void MenuEntry::draw(int x, int y, bool selected)
  {
  if(selected)
    draw_text(label.c_str(), x, y, font_size, 252,84,84, 168,0,0, CENTER_ALLIGN);
  else
    draw_text(label.c_str(), x, y, font_size, 252,252,252, 168,168,168, CENTER_ALLIGN);
  }

Menu::Menu(const std::string &name)
  : parent(NULL), pos_y(screen->h/2), width(0), name(name)
  {
  reset();
  }

Menu::~Menu()
  {
  for(unsigned int i = 0; i < submenus.size(); i++)
    if(submenus[i])
      delete submenus[i];
  }

void Menu::reset()
  {
  hover_entry = 0;
  selected = (cancel = false);
  selected_submenu = -1;
  }

void Menu::add_entry(const std::string& str, int id, int font_size)
  {
  entries.push_back(MenuEntry(str, id, font_size));
  submenus.push_back(NULL);
  pos_y -= font_size / 2;
  width = std::max(width, (unsigned int)(str.size()*(font_size*1.25)));
  }

void Menu::add_subentry(const std::string& str, int id, int font_size)
  {
  assert(!submenus.empty());
  if(submenus.back() == NULL)
    {
    submenus.back() = new Menu("submenu");
    submenus.back()->parent = this;
    }
  submenus.back()->add_entry(str, id, font_size);
  }

bool Menu::has_selected()
  {
  return cancel || selected;
  }

int Menu::get_selected()
  {
  if(cancel)
    return BACK_ID;
  if(!selected) {
    return NO_ITEM_SELECTED;
  }
  int sel = entries[hover_entry].id;
  //reset();
  return sel;
  }

Menu *Menu::get_submenu()
{
  if(selected_submenu < 0)
    return NULL;
  return submenus[selected_submenu];
}

#define MAINMENU_X (screen->w/2)
#define SUBMENU_X  (screen->w-100)

void Menu::show()
  {
  draw(MAINMENU_X);
  if(selected_submenu >= 0 && submenus[selected_submenu]) {
    submenus[selected_submenu]->draw(SUBMENU_X);
}

  if(selected_submenu != -1)
    submenus[selected_submenu]->check_events(SUBMENU_X);
  else
    this->check_events(MAINMENU_X);
  }

void Menu::draw(int x)
{
  int dy = 0;
  for(unsigned int i = 0; i < entries.size(); i++) {
    if(hover_entry == (int)i)
      screen->draw_animated_filled_rect(x-(width/2), pos_y + dy,
        width, entries[i].font_size, 20);
    entries[i].draw(x, pos_y + dy, hover_entry == (int)i);
    dy += entries[i].font_size * ENTRY_SPACE;
  }
}

void Menu::check_events(int pos_x)
  {
  idle = true;
  while(SDL_PollEvent(&event))
    {
    idle = false;
    switch(event.type)
      {
      case SDL_KEYDOWN:	// key pressed
        switch(event.key.keysym.sym)
          {
          case SDLK_UP:
            hover_entry--;
            if(hover_entry < 0)
              hover_entry = entries.size()-1;
            break;
          case SDLK_DOWN:
            hover_entry++;
            if(hover_entry >= (int)entries.size())
              hover_entry = 0;
            break;
          case SDLK_RETURN:
            if(event.key.keysym.mod & KMOD_ALT)
              {
              screen->toggle_fullscreen();
              break;
              }
          case SDLK_SPACE:
            if(submenus[hover_entry] != NULL)
              selected_submenu = hover_entry;
            selected = true;
            break;
          default:
            break;
          }
        break;
      case SDL_KEYUP:	// key released
        if(event.key.keysym.sym == SDLK_ESCAPE)
          cancel = true;
        break;
      /* Mouse support. */
      case SDL_MOUSEMOTION:
        {
        unsigned int x = event.motion.x, y = event.motion.y;
        if(x < pos_x-(width/2) || x > pos_x-(width/2) + width)
          break;

        int dy = 0;
        for(unsigned int i = 0; i < entries.size(); i++) {
          if(y >= pos_y+dy && y < pos_y+dy+entries[i].font_size)
            hover_entry = i;
          dy += entries[i].font_size*ENTRY_SPACE;
        }
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
        if(event.button.button == SDL_BUTTON_LEFT)
          {
          unsigned int x = event.motion.x, y = event.motion.y;
          if(x < pos_x-(width/2) || x > pos_x-(width/2) + width)
            break;

          int dy = 0;
          for(unsigned int i = 0; i < entries.size(); i++) {
            if(y >= pos_y+dy && y < pos_y+dy+entries[i].font_size && hover_entry == (signed)i) {
              selected = true;
              if(submenus[hover_entry] != NULL)
                selected_submenu = hover_entry;
            }
            dy += entries[i].font_size*ENTRY_SPACE;
          }
          }
        break;
      case SDL_QUIT:	// window closed
        cancel = true;
        break;
      default:
        break;
      }
    }
  }
