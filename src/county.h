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

#pragma once

#include <array>

// TODO: Remove once Github's Linux compiler is modern enough
#if defined(__has_cpp_attribute) 
  #if __has_cpp_attribute(__cpp_lib_three_way_comparison)
    #define HAS_COMPARE
  #endif
#endif

#ifdef HAS_COMPARE
#include <compare>
#endif

#include <string>
#include <vector>

enum struct State {
  Alabama,
  Alaska,
  Arizona,
  Arkansas,
  California,
  Colorado,
  Connecticut,
  Delaware,
  District_Of_Columbia,
  Florida,
  Georgia,
  Hawaii,
  Idaho,
  Illinois,
  Indiana,
  Iowa,
  Kansas,
  Kentucky,
  Louisiana,
  Maine,
  Maryland,
  Massachusetts,
  Michigan,
  Minnesota,
  Mississippi,
  Missouri,
  Montana,
  Nebraska,
  Nevada,
  New_Hampshire,
  New_Jersey,
  New_Mexico,
  New_York,
  North_Carolina,
  North_Dakota,
  Ohio,
  Oklahoma,
  Oregon,
  Pennsylvania,
  Rhode_Island,
  South_Carolina,
  South_Dakota,
  Tennessee,
  Texas,
  Utah,
  Vermont,
  Virginia,
  Washington,
  West_Virginia,
  Wisconsin,
  Wyoming,
  NStates
};


struct County {
  std::string name;
  State state = State::NStates;

#ifdef HAS_COMPARE
  friend auto operator<=>(const County&, const County&) = default;
#else
  friend bool operator<(const County& lhs, const County& rhs)
  {
    if (lhs.state != rhs.state) return lhs.state < rhs.state;
    return lhs.name.compare(rhs.name) < 0;
  }
  friend bool operator==(const County& lhs, const County& rhs)
  {
    return lhs.name == rhs.name && lhs.state == rhs.state;
  }
#endif

  std::string toString() const;
  static County fromString(const std::string&);

  friend std::ostream& operator<<(std::ostream&, const County&);
};


constexpr std::array<State, static_cast<int>(State::NStates)> AllStates()
{
  constexpr std::array<State, static_cast<int>(State::NStates)> states = {
      State::Alabama,        State::Alaska,         State::Arizona,
      State::Arkansas,       State::California,     State::Colorado,
      State::Connecticut,    State::Delaware,       State::District_Of_Columbia,
      State::Florida,        State::Georgia,        State::Hawaii,
      State::Idaho,          State::Illinois,       State::Indiana,
      State::Iowa,           State::Kansas,         State::Kentucky,
      State::Louisiana,      State::Maine,          State::Maryland,
      State::Massachusetts,  State::Michigan,       State::Minnesota,
      State::Mississippi,    State::Missouri,       State::Montana,
      State::Nebraska,       State::Nevada,         State::New_Hampshire,
      State::New_Jersey,     State::New_Mexico,     State::New_York,
      State::North_Carolina, State::North_Dakota,   State::Ohio,
      State::Oklahoma,       State::Oregon,         State::Pennsylvania,
      State::Rhode_Island,   State::South_Carolina, State::South_Dakota,
      State::Tennessee,      State::Texas,          State::Utah,
      State::Vermont,        State::Virginia,       State::Washington,
      State::West_Virginia,  State::Wisconsin,      State::Wyoming};

  return states;
}


constexpr const char* AbbreviationForState(const State state)
{
  switch (state) {
    case State::Alabama:
      return "AL";
    case State::Alaska:
      return "AK";
    case State::Arizona:
      return "AZ";
    case State::Arkansas:
      return "AR";
    case State::California:
      return "CA";
    case State::Colorado:
      return "CO";
    case State::Connecticut:
      return "CT";
    case State::Delaware:
      return "DE";
    case State::District_Of_Columbia:
      return "DC";
    case State::Florida:
      return "FL";
    case State::Georgia:
      return "GA";
    case State::Hawaii:
      return "HI";
    case State::Idaho:
      return "ID";
    case State::Illinois:
      return "IL";
    case State::Indiana:
      return "IN";
    case State::Iowa:
      return "IA";
    case State::Kansas:
      return "KS";
    case State::Kentucky:
      return "KY";
    case State::Louisiana:
      return "LA";
    case State::Maine:
      return "ME";
    case State::Maryland:
      return "MD";
    case State::Massachusetts:
      return "MA";
    case State::Michigan:
      return "MI";
    case State::Minnesota:
      return "MN";
    case State::Mississippi:
      return "MS";
    case State::Missouri:
      return "MO";
    case State::Montana:
      return "MT";
    case State::Nebraska:
      return "NE";
    case State::Nevada:
      return "NV";
    case State::New_Hampshire:
      return "NH";
    case State::New_Jersey:
      return "NJ";
    case State::New_Mexico:
      return "NM";
    case State::New_York:
      return "NY";
    case State::North_Carolina:
      return "NC";
    case State::North_Dakota:
      return "ND";
    case State::Ohio:
      return "OH";
    case State::Oklahoma:
      return "OK";
    case State::Oregon:
      return "OR";
    case State::Pennsylvania:
      return "PA";
    case State::Rhode_Island:
      return "RI";
    case State::South_Carolina:
      return "SC";
    case State::South_Dakota:
      return "SD";
    case State::Tennessee:
      return "TN";
    case State::Texas:
      return "TX";
    case State::Utah:
      return "UT";
    case State::Vermont:
      return "VT";
    case State::Virginia:
      return "VA";
    case State::Washington:
      return "WA";
    case State::West_Virginia:
      return "WV";
    case State::Wisconsin:
      return "WI";
    case State::Wyoming:
      return "WY";
    case State::NStates:
      return "null";
  }
  return "can't happen";
}


State AbbreviationToState(const std::string& abbr);
