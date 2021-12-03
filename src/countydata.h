#pragma once

#include <utility>
#include <vector>

#include "county.h"

class CountyData
{
public:
  CountyData();
  ~CountyData() = default;
  CountyData(const CountyData&) = default;
  CountyData(CountyData&&) = default;
  CountyData& operator=(const CountyData&) = default;
  CountyData& operator=(CountyData&&) = default;

  bool countyVisited(const County& c) const;
  bool setCountyVisited(const County& c, const bool visited);

  bool readFromFile(const std::string& fileName);

  size_t size() const { return mCounties.size(); }

private:
  std::vector<std::pair<County, bool>>::iterator findCounty(const County& c);
  std::vector<std::pair<County, bool>>::const_iterator findCounty(
      const County& c) const;

public:
  std::vector<std::pair<County, bool>> mCounties;
};

