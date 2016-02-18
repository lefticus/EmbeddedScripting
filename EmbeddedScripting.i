%module EmbeddedScripting

%include <std_string.i>

std::string helloworld();

%{
std::string helloworld() {
  return "Hello World";
}

%}


