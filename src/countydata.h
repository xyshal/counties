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
  bool writeToFile(const std::string& fileName) const;

  void setSvgColor(const std::string& color);
  bool writeSvg(const std::string& fileName);
  std::string svg();

  size_t size() const { return mCounties.size(); }

  std::pair<size_t, double> numberAndPercentVisited() const;
  // double percentVisited(const State&);
  // double percentStatesCompleted() const;
  // int statesVisited() const;

private:
  std::vector<std::pair<County, bool>>::iterator findCounty(const County& c);
  std::vector<std::pair<County, bool>>::const_iterator findCounty(
      const County& c) const;

  void rebuildSvg();

public:
  std::vector<std::pair<County, bool>> mCounties;

private:
  pugi::xml_document vSvg;
  std::string vVisitedColor = "blue";
  bool vSvgDirty = false;
};

