#pragma once

#include <array>
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
