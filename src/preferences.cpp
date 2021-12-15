/*
 * Copyright (C) 2021 Owen Schandle
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.

 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "preferences.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>

static constexpr auto prefFileName = ".qcounties.pref";

Preferences::Preferences()
{
#ifdef _WIN32
  constexpr auto homeEnv = "userprofile";
#else
  constexpr auto homeEnv = "HOME";
#endif

  if (const char* homePath = std::getenv(homeEnv)) {
    vPreferencesPath = std::string(homePath) + "/" + prefFileName;
    vPreferencesPath =
        std::filesystem::path(vPreferencesPath).make_preferred().string();

    if (std::filesystem::exists(vPreferencesPath)) {
      std::ifstream prefFile(vPreferencesPath);
      if (!prefFile.is_open()) {
        std::cout << "Warning: Failed to open existing preferences file\n";
      } else {
        // NOTE: Serious assumption here!
        std::getline(prefFile, mVisitedColor);
      }
    }
  } else {
    std::cout << "Warning: unable to determine where the home directory is\n";
  }
}


Preferences::~Preferences()
{
  std::ofstream prefFile(vPreferencesPath);
  if (!prefFile.is_open()) {
    std::cout << "Warning: unable to write preferences -- color will not be "
                 "persistent!\n";
    std::cout << "  (Tried path '" + vPreferencesPath + "'\n";
  } else {
    prefFile << mVisitedColor;
  }
}
