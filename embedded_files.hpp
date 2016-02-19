#include <cstdint>
#include <map>
#include <string>

namespace embedded_files { 
  std::map<std::string, std::pair<size_t, const uint8_t *>> files();

}
