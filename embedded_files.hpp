#include <cstdint>
#include <map>
#include <string>
#include <boost/filesystem.hpp>
#include <fstream>

namespace embedded_files { 
  std::map<std::string, std::pair<size_t, const uint8_t *> > files();

  inline bool hasFile(const std::string &t_filename) {
    const auto fs = files();
    return fs.find(t_filename) != fs.end();
  }

  inline std::string getFileAsString(const std::string &t_filename) {
    const auto fs = files();
    const auto f = fs.at(t_filename);
    return std::string(f.second, f.second + f.first);
  }

  inline void extractFile(const std::string &t_filename, const boost::filesystem::path &t_location)
  {
    const auto fs = files();
    auto f = fs.at(t_filename);
    boost::filesystem::path p = t_location / boost::filesystem::path(t_filename);
    boost::filesystem::create_directories(p.parent_path());
    std::ofstream ofs(p.string().c_str());
    ofs.write(reinterpret_cast<const char *>(f.second), f.first);
    std::cout << "***** Extracted " << t_filename << " to: " << p.string() << " *****\n";
  }


}
