#include "RubyInterpreter.hpp"
#include "embedded_files.hpp"

#include <iostream>
#include <dlfcn.h>
#include <dirent.h>


FILE *(*origfopen)(const char *, const char *);
int (*origopen)(const char *, int);
ssize_t (*origread)(int fd, void *, size_t);
int (*origstat)(const char *, struct stat *);
DIR* (*origopendir)(const char *);

extern "C" {
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

  DIR *opendir(const char *path) {
    std::cout << "opendir: " << path << '\n';
    return origopendir(path);
  }

  void Init_EmbeddedScripting(void);
}


int main(int argc, char *argv[])
{
  std::cout << "***** Initializing file function pointers *****\n";
  origfopen = (FILE *(*)(const char *, const char *))(dlsym(RTLD_NEXT, "fopen"));
  origopen = (int (*)(const char *, int))(dlsym(RTLD_NEXT, "open"));
  origread = (ssize_t (*)(int, void*, size_t))(dlsym(RTLD_NEXT, "read"));
  origstat = (int (*)(const char *, struct stat *))(dlsym(RTLD_NEXT, "stat"));
  origopendir = (DIR* (*)(const char *))(dlsym(RTLD_NEXT, "opendir"));

  std::cout << "***** Initializing ruby *****\n";
  ruby_sysinit(&argc, &argv);
  {
    RUBY_INIT_STACK;
    ruby_init();
  }

  std::cout << "***** Initializing RubyInterpreter Wrapper *****\n";
  RubyInterpreter rubyInterpreter("vfs");

  std::cout << "***** Initializing Embedded Ruby Module *****\n";
  Init_EmbeddedScripting();


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
    return gem_original_require path
  end
end
)");


  std::cout << "***** Calling Embedded Module Function *****\n";
  rubyInterpreter.evalString("puts(EmbeddedScripting::helloworld())");

  std::cout << "***** Requiring JSON *****\n";
  rubyInterpreter.evalString("require 'json'");




  const auto files = embedded_files::files();

  std::cout << "***** Listing embedded files *****\n";
  for (const auto &f : files) {
    std::cout << "Embedded file: '" << f.first << "': " << f.second.first << " bytes\n";
  }


}
