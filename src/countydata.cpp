#include "countydata.h"

#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <iostream>

bool CountyData::countyVisited(const County& county) const
{
  auto it = findCounty(county);
  assert(it != mCounties.end());
  return (*it).second;
}


// Performance could be better here, but whatever; it's only 3k things
void CountyData::setCountyVisited(const County& county, const bool visited)
{
  auto it = findCounty(county);
  assert(it != mCounties.end());
  (*it).second = visited;
}


// Fill default county data from the resource
void CountyData::createDefaultCounties()
{
  mCounties.clear();

  QFile f(":/counties.csv");

  if (f.open(QIODevice::ReadOnly)) {
    QTextStream s(&f);
    while (!s.atEnd()) {
      const QStringList line = s.readLine().split(",");
      assert(line.size() == 2);
      const std::string name = line[0].toStdString();
      const std::string stateAbbr = line[1].trimmed().toStdString();
      State state = State::NStates;
      for (State candidateState : AllStates()) {
        if (stateAbbr == AbbreviationForState(candidateState)) {
          state = candidateState;
          break;
        }
      }
      assert(state != State::NStates);

      mCounties.push_back({{name, state}, false});
    }
  }
}


// TODO: There must be a way to avoid this with templates
std::vector<std::pair<County, bool>>::iterator CountyData::findCounty(
    const County& county)
{
  auto it =
      std::find_if(std::begin(mCounties), std::end(mCounties),
                   [&county](const auto& c) { return c.first == county; });

  if (it == mCounties.end()) {
    std::cerr << __func__ << ": no county in the database matches " << county
              << "\n";
    return {};
  }

  return it;
}


std::vector<std::pair<County, bool>>::const_iterator CountyData::findCounty(
    const County& county) const
{
  auto it =
      std::find_if(std::begin(mCounties), std::end(mCounties),
                   [&county](const auto& c) { return c.first == county; });

  if (it == mCounties.end()) {
    std::cerr << __func__ << ": no county in the database matches " << county
              << "\n";
    return {};
  }

  return it;
}

