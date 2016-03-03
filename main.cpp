#include "RubyInterpreter.hpp"
#include "embedded_files.hpp"

#include <iostream>

#ifndef _MSC_VER
#include <dlfcn.h>
#include <dirent.h>
#endif


FILE *(*origfopen)(const char *, const char *);
int (*origopen)(const char *, int);
ssize_t (*origread)(int fd, void *, size_t);
int (*origstat)(const char *, struct stat *);

extern "C" {

#ifndef _MSC_VER
  int open(const char *path, int flags) {
    const auto id = origopen(path, flags);
    std::cout << "(" << id << ") open: " << path << '\n';
    return id;
  }

  int stat(const char *path, struct stat *buf) {
    std::cout << "stat: " << path << '\n';
    return origstat(path, buf);
  }

  FILE *fopen(const char *path, const char *mode) {
    std::cout << "fopen: " << path << '\n';
    return origfopen(path, mode);
  }

  ssize_t read(int fd, void *buf, size_t count) {
    std::cout << "(" << fd << ") read: " << count << '\n';
    return origread(fd, buf, count);
  }
#endif

  void Init_EmbeddedScripting(void);
}

class Temp_Dir
{
  public:
    Temp_Dir()
    {
      boost::filesystem::create_directories(dirpath);
    }

    ~Temp_Dir()
    {
      boost::filesystem::remove_all(boost::filesystem::path(dirpath));
    }

    boost::filesystem::path dir() const {
      return boost::filesystem::path(dirpath);
    }


  private:
    boost::filesystem::path dirpath = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
};

inline std::unique_ptr<Temp_Dir> extractAll() {
  auto d = std::unique_ptr<Temp_Dir>(new Temp_Dir());

  const auto fs = embedded_files::files();
  for (const auto &f : fs) {
    embedded_files::extractFile(f.first, d->dir());
  }

  return d;
}

int main(int argc, char *argv[])
{
  std::cout << "***** Initializing file function pointers *****\n";

#ifndef _MSC_VER
  origfopen = (FILE *(*)(const char *, const char *))(dlsym(RTLD_NEXT, "fopen"));
  origopen = (int (*)(const char *, int))(dlsym(RTLD_NEXT, "open"));
  origread = (ssize_t (*)(int, void*, size_t))(dlsym(RTLD_NEXT, "read"));
  origstat = (int (*)(const char *, struct stat *))(dlsym(RTLD_NEXT, "stat"));
#endif

  std::cout << "***** Initializing ruby *****\n";
  ruby_sysinit(&argc, &argv);
  {
    RUBY_INIT_STACK;
    ruby_init();
  }


  std::cout << "***** Extracting Files *****\n";
  const auto filepath = extractAll();


  std::cout << "***** Initializing RubyInterpreter Wrapper *****\n";
  RubyInterpreter rubyInterpreter({filepath->dir().string()});

  std::cout << "***** Exercising our search path *****\n";
  rubyInterpreter.evalString(R"(require 'extracted/test3.rb')");


  std::cout << "***** Initializing Embedded Ruby Module *****\n";
  Init_EmbeddedScripting();

  std::cout << "***** Calling Embedded Module Function *****\n";
  rubyInterpreter.evalString("puts(EmbeddedScripting::helloworld())");

  std::cout << "***** Shimming Our Kernel::require method *****\n";
  rubyInterpreter.evalString(R"(
module Kernel
  if defined?(gem_original_require) then
    # Ruby ships with a custom_require, override its require
    remove_method :require
  else
    ##
    alias gem_original_require require
    private :gem_original_require
  end

  def require path
    puts "Requiring #{path}"
    if EmbeddedScripting::hasFile(path) then
      puts "It's an embedded file!"
      return eval(EmbeddedScripting::getFileAsString(path))
    else
      return gem_original_require path
    end
  end
end
)");



  std::cout << "***** Requiring JSON *****\n";
  rubyInterpreter.evalString("require 'json'");




  const auto files = embedded_files::files();

  std::cout << "***** Listing embedded files *****\n";
  for (const auto &f : files) {
    std::cout << "Embedded file: '" << f.first << "': " << f.second.first << " bytes\n";
  }

  std::cout << "***** Exercising our require method *****\n";
  rubyInterpreter.evalString(R"(require 'myvfs/test.rb')");


}
