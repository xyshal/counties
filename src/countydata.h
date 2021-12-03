#pragma once

#include <utility>
#include <vector>

#include "county.h"

// This class only exists so that the model and the main window both have
// access to the same data and can operate on it with an easier interface
// TODO: As we're using a QStandardItemModel, that's no longer true -- should
// this class just go away?

class CountyData
{
public:
  bool countyVisited(const County& c) const;
  void setCountyVisited(const County& c, const bool visited);

  void createDefaultCounties();
  size_t size() const { return mCounties.size(); }

private:
  std::vector<std::pair<County, bool>>::iterator findCounty(const County& c);
  std::vector<std::pair<County, bool>>::const_iterator findCounty(
      const County& c) const;

public:
  std::vector<std::pair<County, bool>> mCounties;
};

