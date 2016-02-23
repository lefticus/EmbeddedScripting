#include <cstdint>
#include <map>
#include <string>
#include <boost/filesystem.hpp>
#include <fstream>

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

  class Temp_Dir
  {
    public:
    Temp_Dir()
    {
      // Note this will need to be ported to other operating systems
      // it is only tested on linux at the moment due to the use of
      // mkdtemp
      mkdtemp(&dirname.front());
    }

    ~Temp_Dir()
    {
      boost::filesystem::remove_all(boost::filesystem::path(dirname));
    }

    boost::filesystem::path dir() const {
      return boost::filesystem::path(dirname);
    }


    private:
      std::string dirname = "/tmp/embeddedXXXXXX";
  };

  inline std::unique_ptr<Temp_Dir> extractAll() {
    auto d = std::unique_ptr<Temp_Dir>(new Temp_Dir());

    const auto fs = files();
    for (const auto &f : fs) {
      extractFile(f.first, d->dir());
    }

    return d;
  }
}
