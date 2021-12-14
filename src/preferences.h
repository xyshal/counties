#include <string>

// Because these aren't really preferences yet, and more of a saved application
// state, it's currently RAII with no mechanism for saving manually.  Thus,
// only one instance of this class should ever exist.
//
// TODO: As soon as this contains multiple things, it should be JSON or XML
class Preferences
{
public:
  Preferences();
  ~Preferences();

  Preferences(const Preferences&) = delete;
  Preferences(Preferences&&) = delete;
  Preferences& operator=(const Preferences&) = delete;
  Preferences& operator=(Preferences&&) = delete;

public:
  std::string mVisitedColor = "blue";

private:
  std::string vPreferencesPath = "/dev/null";
};

