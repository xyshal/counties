#include "countydata.h"

#include <csv.h>

#include <QFile>
#include <QString>
#include <QTextStream>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

// Fills the data from the known-accurate resource
CountyData::CountyData()
{
  mCounties.clear();

  QFile f(":/Usa_counties_large.svg");

  // TODO: There's probably a better way to do this.
  QString fileContents;

  if (!f.open(QIODevice::ReadOnly)) {
    assert(false);
  } else {
    QTextStream s(&f);
    while (!s.atEnd()) {
      const QString line = s.readLine();

      // KLUDGE: Boy, what a hack.  pugixml can't deal with the lack of </g> and
      // refuses to parse the svg, but the svg standard requires it to not
      // exist so the resource can't be modified.
      // TODO: Brief internet searching suggests that ending <g> is totally
      // valid SVG, so is something wrong with the QSvgWidget?
      if (line.contains("</svg>")) fileContents += QString(" </g>");

      fileContents += line;
    }
    assert(!fileContents.isEmpty());

    // Pull all the <path>s into the data
    const pugi::xml_parse_result result =
        vSvg.load_string(fileContents.toStdString().c_str());
    assert(result == pugi::status_ok);
    const pugi::xml_node countyGroup = vSvg.child("svg").child("g");

    size_t nPaths = 0;
    for (const pugi::xml_node& child : countyGroup.children()) {
      // Counties
      const County county = County::fromString(child.attribute("id").value());
      constexpr bool visited = false;
      mCounties.push_back({county, visited});

      // States
      if (vCountiesPerState.contains(county.state)) {
        vCountiesPerState[county.state]++;
      } else {
        vCountiesPerState[county.state] = 1;
      }

      nPaths++;
    }
    assert(mCounties.size() == 3143U);

    // Because the SVG doesn't sort the counties, we have to.
    // TODO: Just sort the SVG's path elements and resave it; it shouldn't
    // matter, right?
    std::sort(mCounties.begin(), mCounties.end(), [](auto& lhs_, auto& rhs_) {
      const County& lhs = lhs_.first;
      const County& rhs = rhs_.first;
      if (lhs.state != rhs.state) return lhs.state < rhs.state;
      return lhs.name < rhs.name;
    });
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

  vSvgDirty = true;

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
  io::CSVReader<2, io::trim_chars<' '>, io::double_quote_escape<',', '"'>> reader(fileName);
  reader.read_header(io::ignore_extra_column, "county", "visited");
 
  bool ok = true;
  unsigned i = 0;
  std::string county_;
  std::string visited_;
  while (reader.read_row(county_, visited_)) {
    i++;

    const County county = County::fromString(county_);
    if (county.state == State::NStates) {
      std::cerr << "Warning: Skipping line " << i
                << ", which had unrecognized data " << county_ << "\n";
      ok = false;
      continue;
    }

    const bool visited = [&]() -> bool {
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


bool CountyData::writeToFile(const std::string& fileName) const
{
  std::ofstream f;
  f.open(fileName);
  if (!f.is_open()) return false;

  f << "county,visited\n";
  for (const auto& countyPair : mCounties) {
    f << countyPair.first.toString() << ",";
    f << (countyPair.second ? "1" : "0");
    f << "\n";
  }

  return true;
}


/*!
 * CountyData::setSvgColor
 *
 * \brief Sets the SVG color from its hex value (e.g. '#000000') or name (e.g.
 * 'blue')
 *
 */
void CountyData::setSvgColor(const std::string& color)
{
  assert(!color.empty());
  vVisitedColor = color;

  // Re-color the SVG for anything with a fill
  const pugi::xml_node countyGroup = vSvg.child("svg").child("g");
  for (pugi::xml_node& child : countyGroup.children()) {
    if (!child.attribute("fill").empty()) {
      child.attribute("fill") = vVisitedColor.c_str();
    }
  }
}


bool CountyData::writeSvg(const std::string& fileName)
{
  if (vSvgDirty) rebuildSvg();

  // HACK: The reverse of the constructor's situation, but pugixml doesn't
  // mind writing out the DOM without the closing </g> so no changes required
  // here...
  return vSvg.save_file(fileName.c_str());
}


std::string CountyData::svg()
{
  if (vSvgDirty) rebuildSvg();

  std::stringstream ss;
  vSvg.save(ss);
  return ss.str();
}


Statistics CountyData::statistics() const
{
  assert(mCounties.size() > 0U);

  size_t totalVisitedCounties = 0U;
  std::unordered_map<State, size_t> visitedCountiesPerState;

  // Gather county-level data
  for (const auto& pair : mCounties) {
    const County& county = pair.first;
    const bool& visited = pair.second;
    if (visited) {
      totalVisitedCounties++;

      if (visitedCountiesPerState.contains(county.state)) {
        visitedCountiesPerState[county.state]++;
      } else {
        visitedCountiesPerState[county.state] = 1;
      }
    }
  }

  // Gather state-level data
  size_t totalStatesCompleted = 0U;
  for (const State& state : AllStates()) {
    // DC is its own entity in the SVG and in the tree view, but it isn't
    // really a state, so we need to exclude its State enum equivalent here.
    if (state == State::District_Of_Columbia) continue;
    assert(vCountiesPerState.contains(state));
    if (visitedCountiesPerState.contains(state) &&
        visitedCountiesPerState.at(state) == vCountiesPerState.at(state)) {
      totalStatesCompleted++;
    }
  }

  // Calculate percentages (which are rounding to 0.1)
  const double percentCountiesVisited =
      std::round(static_cast<double>(totalVisitedCounties) / mCounties.size() *
                 100.0 * 10.0) /
      10.0;

  const double percentStatesCompleted =
      std::round(static_cast<double>(totalStatesCompleted) /
                 (AllStates().size() - 1) * 100.0 * 10.0) /
      10.0;


  Statistics stats;
  stats.countiesVisited = totalVisitedCounties;
  stats.percentCountiesVisited = percentCountiesVisited;
  stats.statesCompleted = totalStatesCompleted;
  stats.percentStatesCompleted = percentStatesCompleted;

  for (const State& state : AllStates()) {
    if (!visitedCountiesPerState.contains(state)) {
      stats.countiesAndPercentCompletePerState[state] = {0U, 0.0};
    } else {
      const size_t countiesVisitedInState = visitedCountiesPerState.at(state);
      const double percentOfState =
          std::round(static_cast<double>(countiesVisitedInState) /
                     vCountiesPerState.at(state) * 100.0 * 10.0) /
          10.0;
      stats.countiesAndPercentCompletePerState[state] = {countiesVisitedInState,
                                                         percentOfState};
    }
  }

  return stats;
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


// Updates the colors in the SVG.  TODO: Avoid this duplication?
void CountyData::rebuildSvg()
{
  const pugi::xml_node countyGroup = vSvg.child("svg").child("g");
  for (pugi::xml_node& child : countyGroup.children()) {
    const County county = County::fromString(child.attribute("id").value());
    const bool visited = findCounty(county)->second;
    const pugi::xml_attribute color = child.attribute("fill");
    if (color.empty()) {
      if (visited) child.append_attribute("fill") = vVisitedColor.c_str();
    } else {
      if (visited) {
        child.attribute("fill") = vVisitedColor.c_str();
      } else {
        child.remove_attribute(color);
      }
    }
  }
  vSvgDirty = false;
}
