#pragma once

#include <pugixml.hpp>
#include <utility>
#include <vector>

#include "county.h"

class CountyData
{
public:
  CountyData();
  ~CountyData() = default;
  CountyData(const CountyData&) = delete;
  CountyData(CountyData&&) = delete;
  CountyData& operator=(const CountyData&) = delete;
  CountyData& operator=(CountyData&&) = delete;

  bool countyVisited(const County& c) const;
  bool setCountyVisited(const County& c, const bool visited);

  bool readFromFile(const std::string& fileName);

  bool toSvg(const std::string& fileName) const;

  size_t size() const { return mCounties.size(); }



private:
  std::vector<std::pair<County, bool>>::iterator findCounty(const County& c);
  std::vector<std::pair<County, bool>>::const_iterator findCounty(
      const County& c) const;

public:
  std::vector<std::pair<County, bool>> mCounties;

private:
  pugi::xml_document vSvg;
};

