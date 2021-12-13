#include "countydata.h"

#include <QFile>
#include <QString>
#include <QTextStream>
#include <algorithm>
#include <fstream>
#include <iostream>

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
      const County county = County::fromString(child.attribute("id").value());
      constexpr bool visited = false;
      mCounties.push_back({county, visited});
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

  // Update the color in the SVG.  TODO: Avoid this duplication?
  const pugi::xml_node countyGroup = vSvg.child("svg").child("g");
  for (pugi::xml_node& child : countyGroup.children()) {
    const County candidateCounty = County::fromString(child.attribute("id").value());
    if (candidateCounty == county) {
      const pugi::xml_attribute color = child.attribute("fill");
      if (color.empty()) {
        if (visited) child.append_attribute("fill") = "blue";
      } else {
        if (visited) {
          child.attribute("fill") = "blue";
        } else {
          child.remove_attribute(color);
        }
      }
    }
  }

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


bool CountyData::toSvg(const std::string& fileName) const
{
  // HACK: The reverse of the constructor's situation, but pugixml doesn't mind
  // writing out the DOM without the closing </g> so no changes required
  // here...
  return vSvg.save_file(fileName.c_str());
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

