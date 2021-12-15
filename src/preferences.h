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

#include <string>

// Because these aren't really preferences yet, and more of a saved application
// state, it's currently RAII with no mechanism for saving manually.  Thus,
// only one instance of this class should ever exist.
//
// TODO: As soon as this contains multiple things, it should be JSON or XML
class Preferences
{
public:
  Preferences();
  ~Preferences();

  Preferences(const Preferences&) = delete;
  Preferences(Preferences&&) = delete;
  Preferences& operator=(const Preferences&) = delete;
  Preferences& operator=(Preferences&&) = delete;

public:
  std::string mVisitedColor = "blue";

private:
  std::string vPreferencesPath = "/dev/null";
};
