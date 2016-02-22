#include <cstdint>
#include <map>
#include <string>

namespace embedded_files { 
  std::map<std::string, std::pair<size_t, const uint8_t *>> files();

  inline bool hasFile(const std::string &t_filename) {
    const auto fs = files();
    return fs.find(t_filename) != fs.end();
  }

  inline std::string getFileAsString(const std::string &t_filename) {
    const auto fs = files();
    const auto f = fs.at(t_filename);
    return std::string(f.second, f.second + f.first);
  }
}
