/***************************************************************************
                          menu.h  -  responsible for the menu
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

#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>

#include "timer.h"

enum PredefinedIDs {
  // special Ids
  NO_ITEM_SELECTED = -2,
  BACK_ID = -1,
  // ordinary ones
  START_GAME_ID,
  LEVELEDITOR_ID,
  OPTIONS_ID,
  };

class Menu;

bool show_yes_no_dialog(const char* text);

void setup_menus();
void free_menus();

// the different menus:
extern Menu* main_menu;

struct MenuEntry
  {
  MenuEntry(const std::string& str, int id);
  ~MenuEntry();

  void draw(int x, int y, bool selected);

  std::string label;
  int id;
  };

class Menu
  {
  public:
    Menu(const std::string &name);
    ~Menu();

    void add_entry(const std::string& str, int id);
    void add_subentry(const std::string& str, int id);

    bool has_selected();
    int get_selected();
    Menu *get_submenu();
    void reset();

    void show();

    static int idle;

  private:
    void draw(int x);
    void check_events(int x);

    std::vector <MenuEntry> entries;
    int hover_entry;

    std::vector <Menu*> submenus;
    int selected_submenu;
    Menu* parent;  // in case this is a submenu

    // actions:
    bool selected;
    bool cancel;

    unsigned int pos_y, width;
    std::string name;
  };

#endif /*MENU_H*/
