#include "RubyInterpreter.hpp"

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
}
