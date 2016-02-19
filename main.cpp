#include "RubyInterpreter.hpp"
#include "embedded_files.hpp"
#include <iostream>

extern "C" {
void Init_EmbeddedScripting(void);
}


int main(int argc, char *argv[])
{

  ruby_sysinit(&argc, &argv);
  {
    RUBY_INIT_STACK;
    ruby_init();
  }

  RubyInterpreter rubyInterpreter;

  Init_EmbeddedScripting();

  rubyInterpreter.evalString("puts(EmbeddedScripting::helloworld())");

  const auto files = embedded_files::files();

  for (const auto &f : files) {
    std::cout << "Embedded file: '" << f.first << "': " << f.second.first << " bytes\n";
  }
}
