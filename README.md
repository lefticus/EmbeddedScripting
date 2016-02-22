# Goals

 * Provide a single executable that contains an embedded ruby interpreter and all requirements
 * Expose C++ functions using [SWIG](http://swig.org/) that defined inside of this executable to ruby
 * Support all major (MacOS, Windows, Linux) platforms equally
 * Attempt to reduce the number of temporary files created during program execution
 
# Challenges

 * Embedding ruby into an executable
 * Loading SWIG exposed functions into embedded ruby
 * Embedding files into a C++ application
 * Loading embedded files as ruby modules
 
# Solutions

## Embedding Ruby Into An Executable

There are many nuances for properly initializing an embedding ruby. Fortunately, these nuances have been worked out previously
in OpenStudio applications, so we have been able to borrow those solutions for this project. Specifically, the 
[RubyInterpreter.hpp](/RubyInterpreter.hpp) file has been cleaned up and adapted for use.

## Loading SWIG Exposed Functions Into Embedded Ruby

SWIG does not generate a header designed for manually initializing a generated module. However, the function name that is
created by the generator is well defined because it is the name that is expected by Ruby.

To avoid `#include`ing large files or having to manipulate generated files, we simply declare and call the function
that we expect to exist.

That is:

```cpp
extern "C" {
// declare expected initialization function
void Init_EmbeddedScripting(void);
}

int main()
{
  // create and initialize ruby iterpreter 
  
  // ...
  
  Init_EmbeddedScripting();
}
```

(Note that the name 'EmbeddedScripting' is the name of the module as defined in the SWIG.i file.
 See [main.cpp](/main.cpp) and [EmbeddedScripting.i](/EmbeddedScripting.i#L1) for full details)

The declared function is found by the linker at link time when the SWIG generated module is compiled and linked
into this executable as any other .cpp file would be.

## Embedding Files Into A C++ Application

### Options

#### Qt Resource System

The [Qt Resource System](http://doc.qt.io/qt-5/resources.html) is a common tool for this situation. However, Qt is a large
dependency and inappropriate for this situation.

#### xxd

[xxd](http://linux.die.net/man/1/xxd) is a UNIX command line tool that can be used to dump binary data as hex strings designed for 
embedding into a C application (using the `-i` parameter). This tool is also available on recent releases of the git 
shell installer on Windows. Since [Git for Windows](http://www.git-scm.com/download/win) is largely a requirement for building the OpenStudio suite already,
this was the presumed solution initially. 

However, xxd's output does *not* give us:
  
  * Easy capability for embedding multiple files
  * Storage or customization of the saved file's name
  
#### Pure CMake Solution

Using the [file](https://cmake.org/cmake/help/v3.0/command/file.html) function in CMake allows us to load a file as a string
of hexidecimal bytes.

```cmake
file(READ "filename" outputvariable HEX)
```

This string can then be split up and dumped into a byte array in a generated cxx file. Allowing us to later access all of the
data and resulting location of the file.

### Chosen Solution

The "Pure CMake Solution" was chosen as it provide the most flexibility and portability. The current implementation is not
efficient because it does not generate an actual target, and needs to be run each time cmake is run. 

See [CMakeLists.txt](/CMakeLists.txt) for the implementation details.

## Loading Embedded Files as Ruby Modules

### Options

#### Implement Virtual Filesystem

##### Concept

Intercept C standard library function calls that are trying to access some predefined location (e.g. `myvirtualfs/`)
and return back some well formed value that allows the application to access files stored inside of the current
executable without having to actually extract any of those values.

##### Implementation

This can be accomplished with a relatively simple concept of just defining our own versions of C library functions
then allowing the linker to choose our versions over others provided by the standard library.

Example: 

```cpp
#include <iostream>
#include <dlfcn.h>

// Note that there is no legal way in C++ to cast a `void*` to a function pointer,
// so we must either use a c-style cast or a trick that involves 
int (*origopen)(const char *, int) = nullptr;

extern "C" {
  int open(const char *path, int flags) {
    const auto id = origopen(path, flags);
    std::cout << "(" << id << ") open: " << path << '\n';
    return id;
  }
}

int main()
{
  origopen = (int (*)(const char *, int))(dlsym(RTLD_NEXT, "open"));
}
```

or similarly on Winddows:

```
#pragma comment(linker, "/FORCE")

#include <string>
#include <iostream>
#include <windows.h>

#ifdef _DEBUG
FILE *(*origfopen)(const char *, const char *) = (FILE *(*)(const char*, const char *))GetProcAddress(GetModuleHandle(TEXT("ucrtbased.dll")), "fopen");
#else
FILE *(*origfopen)(const char *, const char *) = (FILE *(*)(const char*, const char *))GetProcAddress(GetModuleHandle(TEXT("ucrtbase.dll")), "fopen");
#endif

FILE *fopen(const char *name, const char *mode)
{
  std::cout << "fopen called: " << name << '\n';
  return origfopen(name, mode);
}

int main()
{
}
```

##### Issues

While the UNIX method is a fairly normal way of intercepting C function calls, it's not entirely clear if the Windows version would
work as expected.

Particularly note that we had to enable `/FORCE` linker flags to get it to link due to multiply defined symbols.

*A more complete and general solution for intercepting function calls in Windows is available on [CodeProject](http://www.codeproject.com/Articles/34237/A-C-Style-of-Intercepting-Functions)*

More generally, however, we have the issue of needing to intercept many different function calls on two different
platforms (UNIX/Windows) to provide the consistent behavior that Ruby expects.

That is, we need to convince ruby:

 * Our virtual directory exists
 * Our virtual directory is readable with current permissions
 * Our virtual directory is in fact a directory (as opposed to a file)
 * The virtual files it wants to read exist
 * The virtual files are readable with current permissions

We also need to be able to indicate:
 
 * File size
 * End of file status
 * Error conditions
   * Attempt to write to file
   * Attempt to open file in write or append mode

It's estimated that approximately 10 system calls would need to be intercepted and our own thread safe `FILE *`, `DIR *` and 
filedescriptor tracking system would need to be implemented.

#### Intercept Ruby `require` Method

The concept is to implement our own method that replaces the ruby `Kernel::require` method. 

 1. Our version would intercept require calls that are looking in a path that exists in our virtual filesystem.
 2. If the file exists in our internal filesystem the file will be read and then evaluated
 3. If not, the require call is passed to the built in require method

##### Implementation

The idea is sound, as it's exactly what rubygems does. A sample implementation is:

```ruby
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
    # Actually do something here
    return gem_original_require path
  end
end
```

This version would likely need to be executed before the `rubygem` module is loaded so that the load for it
can be intercepted as well.

###### Issues

 * What to do if the found file is a dynamic module? The implementation would need to somehow implement the mechanism of `LoadLibrary` on Windows and `dlopen` on UNIX to be able to load a shared object from inside of the existing executable.
 * This code can only execute after the ruby interpreter has been initialized, so it cannot be used to load the ruby gems modules, for instance

