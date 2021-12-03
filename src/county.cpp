#include "county.h"

#include <iostream>

std::string County::toString() const
{
  return std::string() + "\"" + name + "\", " + AbbreviationForState(state);
}

County County::fromString(const std::string& id_)
{
  std::string id = id_;
  id.erase(std::remove(id.begin(), id.end(), '\"'), id.end());

  const size_t firstComma = id.find(",");
  const std::string countyName = id.substr(0, firstComma);
  const std::string state_ = id.substr(firstComma + 2);

  const State state = AbbreviationToState(state_);
  if (state == State::NStates) {
    std::cerr << "Warning: unrecognized state [" << state_ << "]\n";
  }

  return County(countyName, state);
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

