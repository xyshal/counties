#pragma once

#include <string>
#include <vector>

#include "county.h"

std::vector<County> ReadCsvFile(const std::string& fileName);
