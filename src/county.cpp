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

#include "county.h"

#include <iostream>

std::string County::toString() const
{
  return std::string() + "\"" + name + ", " + AbbreviationForState(state) +
         "\"";
}

County County::fromString(const std::string& id_)
{
  std::string id = id_;
  std::erase(id, '\"');

  const size_t firstComma = id.find(",");
  const std::string countyName = id.substr(0, firstComma);
  const std::string state_ = id.substr(firstComma + 2);

  const State state = AbbreviationToState(state_);
  if (state == State::NStates) {
    std::cerr << "Warning: unrecognized state [" << state_ << "]\n";
  }

  return {countyName, state};
}


std::ostream& operator<<(std::ostream& s, const County& c)
{
  s << std::string("{ ") << c.toString() << std::string(" }");
  return s;
}


State AbbreviationToState(const std::string& abbr)
{
  State state = State::NStates;
  for (State candidateState : AllStates()) {
    if (abbr == AbbreviationForState(candidateState)) {
      state = candidateState;
      break;
    }
  }
  return state;
}

