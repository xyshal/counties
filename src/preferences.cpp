#include "preferences.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>

static constexpr auto prefFileName = ".qcounties.pref";

Preferences::Preferences()
{
#ifdef _WIN32
  constexpr auto homeEnv = "userprofile";
#else
  constexpr auto homeEnv = "HOME";
#endif

  if (const char* homePath = std::getenv(homeEnv)) {
    vPreferencesPath = std::string(homePath) + "/" + prefFileName;
    vPreferencesPath =
        std::filesystem::path(vPreferencesPath).make_preferred().string();

    if (std::filesystem::exists(vPreferencesPath)) {
      std::ifstream prefFile(vPreferencesPath);
      if (!prefFile.is_open()) {
        std::cout << "Warning: Failed to open existing preferences file\n";
      } else {
        // NOTE: Serious assumption here!
        std::getline(prefFile, mVisitedColor);
      }
    }
  } else {
    std::cout << "Warning: unable to determine where the home directory is\n";
  }
}


Preferences::~Preferences()
{
  std::ofstream prefFile(vPreferencesPath);
  if (!prefFile.is_open()) {
    std::cout << "Warning: unable to write preferences -- color will not be "
                 "persistent!\n";
    std::cout << "  (Tried path '" + vPreferencesPath + "'\n";
  } else {
    prefFile << mVisitedColor;
  }
}
