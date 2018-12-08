/***************************************************************************
                          file_access.h  -  handles path, etc
                             -------------------
    begin                : Mon Nov 01 2004
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

#ifndef FILE_ACCESS_H
#define FILE_ACCESS_H

#include <string>
#include <set>

extern std::string datadir;  // the system data stuff
extern std::string homedir;  // here are stores personal level files and config

/* Guesses the data directory.
   DATA_PREFIX should be specified in Makefile to be used to
   get the directory, in case it is installed in the system. */

bool find_datadir(const std::string& invocation);

bool file_exists(const std::string& filename);
bool dir_exists(const std::string& dirname);

// get all files from a folder
std::set<std::string> get_files_in_dir(const std::string& path);

#endif
