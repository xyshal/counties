#include "countydata.h"

#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <algorithm>
#include <fstream>
#include <iostream>

// Fills the data from the known-accurate resource
CountyData::CountyData()
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
      const State state = AbbreviationToState(stateAbbr);
      assert(state != State::NStates);

      mCounties.push_back({{name, state}, false});
    }
  }
}


bool CountyData::countyVisited(const County& county) const
{
  auto it = findCounty(county);
  assert(it != mCounties.end());
  return (*it).second;
}


// Performance could be better here, but whatever; it's only 3k things
bool CountyData::setCountyVisited(const County& county, const bool visited)
{
  auto it = findCounty(county);
  if (it == mCounties.end()) return false;
  (*it).second = visited;
  return true;
}


/*!
 * CountyData::readFromFile
 *
 * \brief Parses a CSV file with the expected format of, e.g.:
 *        "County Name, State", Visited
 *
 *  NOTE: This does not have to be a complete file.  We only process records we
 *        understand, and otherwise rely on the default construction.
 *
 *  \returns Whether the read avoided any serious problems
 *
 */
bool CountyData::readFromFile(const std::string& fileName)
{
  std::ifstream f(fileName);
  if (!f.is_open()) {
    std::cerr << "Failed to open file '" << fileName << "'\n";
    return false;
  }

  bool ok = true;

  std::string line;
  unsigned i = 0;
  while (std::getline(f, line)) {
    i++;

    // Split the county identifier and whether it was visited
    const size_t firstComma = line.find(",");
    const size_t delimiter = line.find(",", firstComma + 1);
    if (delimiter == std::string::npos) {
      std::cerr << "Warning: Skipping line " << i
                << ", which had unexpected format: " << line << "\n";
      ok = false;
      continue;
    }

    std::string id = line.substr(0, delimiter);

    const County county = County::fromString(id);
    if (county.state == State::NStates) {
      std::cerr << "Warning: Skipping line " << i
                << ", which had unrecognized data " << line << "\n";
      ok = false;
      continue;
    }

    const bool visited = [&]() -> bool {
      const std::string visited_ = line.substr(delimiter + 1);
      if (visited_ == "0") return false;
      if (visited_ == "1") return true;
      std::cerr << "Warning: unexpected value '" << visited_ << "' at line "
                << i << ".  Assuming 0.\n";
      return false;
    }();

    const bool setOk = setCountyVisited(county, visited);
    if (!setOk) {
      std::cout << "Warning: Ignoring unrecognized county " << county << "\n";
      ok = false;
    }
  }
  return ok;
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
  }

  return it;
}

