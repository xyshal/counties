#include "csvfile.h"

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

static std::string CountyToID(const County&);


/*!
 * ReadCsvFile
 *
 * \brief Parses a CSV file with the expected format of, e.g.:
 *        "County Name, State", Visited
 *
 * \return A vector of County that hopefully reflects the state of the CSV
 */
std::vector<County> ReadCsvFile(const std::string& fileName)
{
  std::ifstream f(fileName);
  if (!f.is_open()) {
    std::cerr << "Failed to parse file '" << fileName << "'\n";
    return {};
  }

  std::vector<County> counties;
  std::unordered_set<std::string> ids;

  std::string line;
  unsigned i = 0;
  while (std::getline(f, line)) {
    i++;
    const size_t firstComma = line.find(",");
    const size_t delimiter = line.find(",", firstComma + 1);
    if (delimiter == std::string::npos) {
      std::cerr << "Unexpected format of line " << i << ": " << line << "\n";
      return {};
    }

    const std::string id = line.substr(0, delimiter);

    const bool visited = [&]() -> bool {
      const std::string visited_ = line.substr(delimiter + 1);
      if (visited_ == "0") return false;
      if (visited_ == "1") return true;
      std::cerr << "Warning: unexpected value '" << visited_ << "' at line "
                << i << ".  Assuming 0.\n";
      return false;
    }();

    const auto rv = ids.insert(id);
    if (!rv.second) {
      std::cerr << "Warning: duplicate county detected!\n";
    } else {
      std::cout << "name: " << id << " | visited: " << visited << "\n";
      counties.push_back({.name = id, .state = State::Minnesota});
    }
  }

  std::cout << CountyToID({.name = "Whatever", .state = State::Minnesota});

  return counties;
}


std::string CountyToID(const County& county)
{
  return std::string() + "\"" + county.name + "\", " +
         AbbreviationForState(county.state);
}
