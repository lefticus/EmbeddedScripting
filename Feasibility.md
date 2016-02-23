# Feasibility Report

Start by reading the [full report](/README.md).

It is possible to create a C++ application that contains an embedded ruby interpreter and all necessary files.

In general Ruby presents several challenges for embedding and is not a very good candidate for it.

Specifically, the following issues will have to be addressed to ensure good crossplatform capability

 1. On Windows a custom build of the ruby .lib will need to be made that does not export common C library functions. These exports
    can and will interfer with common operations in the rest of the C++ application. Specifically, this code in the Ruby
    build process will need to be changed: https://github.com/ruby/ruby/blob/trunk/win32/mkexports.rb#L38-L52
  
 2. All standard modules for Ruby will need to be statically compiled into the libruby.a or packaged with the executable on all 
    platforms where there is no system installed ruby available.
    
    
Additionally, the following recommendations are made

 1. Update Linux baseline to 16.04 when it is releasted in April, 2016
 2. Use system ruby when possible (Linux/MacOS)
 3. Embed and extract required files to a [temp folder](/README.md#extract-embedded-files-to-actual-filesystem)
 
 
