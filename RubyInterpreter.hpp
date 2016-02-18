/**********************************************************************
 *  Copyright (c) 2008-2016, Alliance for Sustainable Energy.
 *  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 **********************************************************************/

#ifndef UTILITIES_CORE_RUBYINTERPRETER_HPP
#define UTILITIES_CORE_RUBYINTERPRETER_HPP

#include "RubyException.hpp"

#include <vector>
#include <string>
#include <map>
#include <typeinfo>
#include <sstream>


// SWIGRubyRuntime.hxx includes ruby.h which includes ruby/win32.h, which has some brain damaged notions of 
// what standard errno values should be. 

#ifdef _MSC_VER
#ifdef EWOULDBLOCK
#pragma push_macro("EWOULDBLOCK")
#undef EWOULDBLOCK
#endif

#ifdef EINPROGRESS
#pragma push_macro("EINPROGRESS")
#undef EINPROGRESS
#endif

#ifdef EALREADY
#pragma push_macro("EALREADY")
#undef EALREADY
#endif

#ifdef ENOTSOCK
#pragma push_macro("ENOTSOCK")
#undef ENOTSOCK
#endif

#ifdef EDESTADDRREQ
#pragma push_macro("EDESTADDRREQ")
#undef EDESTADDRREQ
#endif

#ifdef EMSGSIZE
#pragma push_macro("EMSGSIZE")
#undef EMSGSIZE
#endif

#ifdef EPROTOTYPE
#pragma push_macro("EPROTOTYPE")
#undef EPROTOTYPE
#endif

#ifdef ENOPROTOOPT
#pragma push_macro("ENOPROTOOPT")
#undef ENOPROTOOPT
#endif

#ifdef EPROTONOSUPPORT
#pragma push_macro("EPROTONOSUPPORT")
#undef EPROTONOSUPPORT
#endif

#ifdef EOPNOTSUPP
#pragma push_macro("EOPNOTSUPP")
#undef EOPNOTSUPP
#endif

#ifdef EAFNOSUPPORT
#pragma push_macro("EAFNOSUPPORT")
#undef EAFNOSUPPORT
#endif

#ifdef EADDRINUSE
#pragma push_macro("EADDRINUSE")
#undef EADDRINUSE
#endif

#ifdef EADDRNOTAVAIL
#pragma push_macro("EADDRNOTAVAIL")
#undef EADDRNOTAVAIL
#endif

#ifdef ENETDOWN
#pragma push_macro("ENETDOWN")
#undef ENETDOWN
#endif

#ifdef ENETUNREACH
#pragma push_macro("ENETUNREACH")
#undef ENETUNREACH
#endif

#ifdef ENETRESET
#pragma push_macro("ENETRESET")
#undef ENETRESET
#endif

#ifdef ECONNABORTED
#pragma push_macro("ECONNABORTED")
#undef ECONNABORTED
#endif

#ifdef ECONNRESET
#pragma push_macro("ECONNRESET")
#undef ECONNRESET
#endif

#ifdef ENOBUFS
#pragma push_macro("ENOBUFS")
#undef ENOBUFS
#endif

#ifdef EISCONN
#pragma push_macro("EISCONN")
#undef EISCONN
#endif

#ifdef ENOTCONN
#pragma push_macro("ENOTCONN")
#undef ENOTCONN
#endif

#ifdef ETIMEDOUT
#pragma push_macro("ETIMEDOUT")
#undef ETIMEDOUT
#endif

#ifdef ECONNREFUSED
#pragma push_macro("ECONNREFUSED")
#undef ECONNREFUSED
#endif

#ifdef ELOOP
#pragma push_macro("ELOOP")
#undef ELOOP
#endif

#ifdef EHOSTUNREACH
#pragma push_macro("EHOSTUNREACH")
#undef EHOSTUNREACH
#endif

#ifdef try
#pragma push_macro("try")
#undef try
#endif

#ifdef except
#pragma push_macro("except")
#undef except
#endif

#ifdef finally
#pragma push_macro("finally")
#undef finally
#endif

#ifdef leave
#pragma push_macro("leave")
#undef leave
#endif

#ifdef OpenFile
#pragma push_macro("OpenFile")
#undef OpenFile
#endif

#ifdef CharNext
#pragma push_macro("CharNext")
#undef CharNext
#endif


#ifdef rb_w32_iswinnt
#pragma push_macro("rb_w32_iswinnt")
#undef rb_w32_iswinnt
#endif

#ifdef rb_w32_iswin95
#pragma push_macro("rb_w32_iswin95")
#undef rb_w32_iswin95
#endif

#ifdef getc
#pragma push_macro("getc")
#undef getc
#endif

#ifdef putc
#pragma push_macro("putc")
#undef putc
#endif

#ifdef fgetc
#pragma push_macro("fgetc")
#undef fgetc
#endif

#ifdef fputc
#pragma push_macro("fputc")
#undef fputc
#endif

#ifdef getchar
#pragma push_macro("getchar")
#undef getchar
#endif

#ifdef putchar
#pragma push_macro("putchar")
#undef putchar
#endif

#ifdef fgetchar
#pragma push_macro("fgetchar")
#undef fgetchar
#endif

#ifdef fputchar
#pragma push_macro("fputchar")
#undef fputchar
#endif

#ifdef utime
#pragma push_macro("utime")
#undef utime
#endif

#ifdef strcasecmp
#pragma push_macro("strcasecmp")
#undef strcasecmp
#endif

#ifdef strncasecmp
#pragma push_macro("strncasecmp")
#undef strncasecmp
#endif

#ifdef close
#pragma push_macro("close")
#undef close
#endif

#ifdef fclose
#pragma push_macro("fclose")
#undef fclose
#endif

#ifdef read
#pragma push_macro("read")
#undef read
#endif

#ifdef write
#pragma push_macro("write")
#undef write
#endif

#ifdef getpid
#pragma push_macro("getpid")
#undef getpid
#endif

#ifdef sleep
#pragma push_macro("sleep")
#undef sleep
#endif

#ifdef creat
#pragma push_macro("creat")
#undef creat
#endif

#ifdef eof
#pragma push_macro("eof")
#undef eof
#endif

#ifdef filelength
#pragma push_macro("filelength")
#undef filelength
#endif

#ifdef mktemp
#pragma push_macro("mktemp")
#undef mktemp
#endif

#ifdef tell
#pragma push_macro("tell")
#undef tell
#endif

#ifdef unlink
#pragma push_macro("unlink")
#undef unlink
#endif

#ifdef _open
#pragma push_macro("_open")
#undef _open
#endif

#ifdef sopen
#pragma push_macro("sopen")
#undef sopen
#endif

#ifdef fopen
#pragma push_macro("fopen")
#undef fopen
#endif

#ifdef fdopen
#pragma push_macro("fdopen")
#undef fdopen
#endif

#ifdef fsopen
#pragma push_macro("fsopen")
#undef fsopen
#endif

#ifdef fsync
#pragma push_macro("fsync")
#undef fsync
#endif

#ifdef stat
#pragma push_macro("stat")
#undef stat
#endif

#ifdef execv
#pragma push_macro("execv")
#undef execv
#endif

#ifdef isatty
#pragma push_macro("isatty")
#undef isatty
#endif

#ifdef mkdir
#pragma push_macro("mkdir")
#undef mkdir
#endif

#ifdef rmdir
#pragma push_macro("rmdir")
#undef rmdir
#endif

#ifdef isascii
#pragma push_macro("isascii")
#undef isascii
#endif

#ifdef vsnprintf
#pragma push_macro("vsnprintf")
#undef vsnprintf
#endif

#ifdef snprintf
#pragma push_macro("snprintf")
#undef snprintf
#endif

#ifdef isnan
#pragma push_macro("isnan")
#undef isnan
#endif

#ifdef finite
#pragma push_macro("finite")
#undef finite
#endif

#ifdef copysign
#pragma push_macro("copysign")
#undef copysign
#endif

#ifdef scalb
#pragma push_macro("scalb")
#undef scalb
#endif

#ifdef accept
#pragma push_macro("accept")
#undef accept
#endif

#ifdef bind
#pragma push_macro("bind")
#undef bind
#endif

#ifdef connect
#pragma push_macro("connect")
#undef connect
#endif

#ifdef FD_SET
#pragma push_macro("FD_SET")
#undef FD_SET
#endif

#ifdef FD_CLR
#pragma push_macro("FD_CLR")
#undef FD_CLR
#endif

#ifdef FD_ISSET
#pragma push_macro("FD_ISSET")
#undef FD_ISSET
#endif

#ifdef select
#pragma push_macro("select")
#undef select
#endif

#ifdef getpeername
#pragma push_macro("getpeername")
#undef getpeername
#endif

#ifdef getsockname
#pragma push_macro("getsockname")
#undef getsockname
#endif

#ifdef getsockopt
#pragma push_macro("getsockopt")
#undef getsockopt
#endif

#ifdef ioctlsocket
#pragma push_macro("ioctlsocket")
#undef ioctlsocket
#endif

#ifdef listen
#pragma push_macro("listen")
#undef listen
#endif

#ifdef recv
#pragma push_macro("recv")
#undef recv
#endif

#ifdef recvfrom
#pragma push_macro("recvfrom")
#undef recvfrom
#endif

#ifdef send
#pragma push_macro("send")
#undef send
#endif

#ifdef sendto
#pragma push_macro("sendto")
#undef sendto
#endif

#ifdef setsockopt
#pragma push_macro("setsockopt")
#undef setsockopt
#endif

#ifdef shutdown
#pragma push_macro("shutdown")
#undef shutdown
#endif

#ifdef socket
#pragma push_macro("socket")
#undef socket
#endif

#ifdef gethostbyaddr
#pragma push_macro("gethostbyaddr")
#undef gethostbyaddr
#endif

#ifdef gethostbyname
#pragma push_macro("gethostbyname")
#undef gethostbyname
#endif

#ifdef gethostname
#pragma push_macro("gethostname")
#undef gethostname
#endif

#ifdef getprotobyname
#pragma push_macro("getprotobyname")
#undef getprotobyname
#endif

#ifdef getprotobynumber
#pragma push_macro("getprotobynumber")
#undef getprotobynumber
#endif

#ifdef getservbyname
#pragma push_macro("getservbyname")
#undef getservbyname
#endif

#ifdef getservbyport
#pragma push_macro("getservbyport")
#undef getservbyport
#endif

#ifdef get_osfhandle
#define OLD_get_osfhandle get_osfhandle
#undef get_osfhandle
#endif

#ifdef getcwd
#pragma push_macro("getcwd")
#undef getcwd
#endif

#ifdef getenv
#pragma push_macro("getenv")
#undef getenv
#endif

#ifdef rename
#pragma push_macro("rename")
#undef rename
#endif

#ifdef times
#pragma push_macro("times")
#undef times
#endif

#ifdef Sleep
#pragma push_macro("Sleep")
#undef Sleep
#endif
#endif


#include "SWIGRubyRuntime.hxx"



class RubyInterpreter
{
  private:
    // dummy arguments passed to ruby_options
    int m_argc;
    char** m_argv;

    static void addIncludePath(std::vector<std::string>& includePaths, const std::string& includePath)
    {
      includePaths.push_back("-I");
      includePaths.push_back(includePath);
    }

  public:
    RubyInterpreter()
    {


      // set load paths
      std::vector<std::string> rubyArgs;

      rubyArgs.emplace_back("-EUTF-8");


      // and now give the interpreter something to parse, so that it doesn't sit
      // waiting on stdin from us
      rubyArgs.emplace_back("-e");
      rubyArgs.emplace_back("");


      m_argc = static_cast<int>(rubyArgs.size());
      m_argv = new char*[m_argc];

      for (int i = 0; i < m_argc; ++i){
        m_argv[i] = new char[rubyArgs[i].size() + 1];
        strcpy(m_argv[i], rubyArgs[i].c_str());
      }

      // the return value of ruby_options is the parsed node of our "script"
      // from the -e "" we passed in. This could be used to actually parse / eval something if we wanted
      ruby_options(m_argc, m_argv);


      // register some default types that we want to pass in / out of the ruby system
      registerType<int>("int");
      registerType<long>("long");
      registerType<size_t>("size_t");
      registerType<double>("double");
      registerType<std::string>("std::string");
      registerType<float>("float");

      // set the script name if desired
      //ruby_script("script_name");
    }

    ~RubyInterpreter()
    {
      for (int i = 0; i < m_argc; i++){
        delete[] m_argv[i];
      }
      delete[] m_argv;
    }

    /// Register a type along with its vector versions so that it can be used 
    /// from within the Ruby wrapper. These are necessary because there's no way to divine 
    /// the unmangled type name.
    template<typename Type>
      void registerType(const std::string &t_name)
      {
        m_types[typeid(Type).name()] = t_name;
        m_types[typeid(Type *).name()] = t_name + " *";
        m_types[typeid(std::vector<Type>).name()] = "std::vector<" + t_name + ">";
        m_types[typeid(std::vector<Type> *).name()] = "std::vector<" + t_name + "> *";
      }

    // evaluate a ruby statement with no return value. If a ruby exception is raised
    // the description is translated into a C++ exception, which is thrown as an openstudio::RubyException.
    void evalString(const std::string &t_str)
    {

      VALUE val = rb_str_new2(t_str.c_str());
      int error;

      // save and restore the current working directory in case the call to ruby upsets it
//      QDir cwd = QDir::current();
      rb_protect(evaluateSimpleImpl,val,&error);
//      QDir::setCurrent(cwd.dirName());


      if (error != 0)
      {
        VALUE errval = rb_eval_string("$!.to_s");
        char *str = StringValuePtr(errval);
        std::string err(str);

        VALUE locval = rb_eval_string("$@.to_s");
        str = StringValuePtr(locval);
        std::string loc(str);

        throw RubyException(err, loc);
      } 
    }

    /// Execute a function by name with 0 parameters and no return value
    template<typename ReturnType>
      void exec(const std::string &t_functionName)
      {
        std::vector<VALUE> params;

        makeProtectedCall(t_functionName, params);
      }

    /// Execute a function by name with 1 parameter and no return value
    template<typename ReturnType, typename Param1>
      void exec(
          const std::string &t_functionName, 
          Param1 t_param1)
      {
        std::vector<VALUE> params;
        params.push_back(newPointerObj(&t_param1));

        makeProtectedCall(t_functionName, params);
      }

    /// Execute a function by name with 2 parameters and no return value
    template<typename ReturnType, typename Param1, typename Param2>
      ReturnType exec(
          const std::string &t_functionName, 
          Param1 t_param1,
          Param2 t_param2)
      {
        std::vector<VALUE> params;
        params.push_back(newPointerObj(&t_param1));
        params.push_back(newPointerObj(&t_param2));

        makeProtectedCall(t_functionName, params);
      }

    /// Execute a function by name with 3 parameters and no return value
    template<typename ReturnType, typename Param1, typename Param2, typename Param3>
      ReturnType exec(
          const std::string &t_functionName,
          Param1 t_param1,
          Param2 t_param2,
          Param3 t_param3)
      {
        std::vector<VALUE> params;
        params.push_back(newPointerObj(&t_param1));
        params.push_back(newPointerObj(&t_param2));
        params.push_back(newPointerObj(&t_param3));

        makeProtectedCall(t_functionName, params);
      }

    /// Execute a function by name with 0 parameters and a return value
    template<typename ReturnType>
      ReturnType execWithReturn(const std::string &t_functionName)
      {
        std::vector<VALUE> params;

        VALUE retval = makeProtectedCall(t_functionName, params);
        return asType<ReturnType>(retval);
      }

    /// Execute a function by name with 1 parameter and a return value
    template<typename ReturnType, typename Param1>
      ReturnType execWithReturn(
          const std::string &t_functionName, 
          Param1 t_param1)
      {
        std::vector<VALUE> params;
        params.push_back(newPointerObj(&t_param1));

        VALUE retval = makeProtectedCall(t_functionName, params);
        return asType<ReturnType>(retval);
      }

    /// Execute a function by name with 2 parameters and a return value
    template<typename ReturnType, typename Param1, typename Param2>
      ReturnType execWithReturn(
          const std::string &t_functionName, 
          Param1 t_param1,
          Param2 t_param2)
      {
        std::vector<VALUE> params;
        params.push_back(newPointerObj(&t_param1));
        params.push_back(newPointerObj(&t_param2));

        VALUE retval = makeProtectedCall(t_functionName, params);
        return asType<ReturnType>(retval);
      }

    /// Execute a function by name with 3 parameters and a return value
    template<typename ReturnType, typename Param1, typename Param2, typename Param3>
      ReturnType execWithReturn(
          const std::string &t_functionName,
          Param1 t_param1,
          Param2 t_param2,
          Param3 t_param3)
      {
        std::vector<VALUE> params;
        params.push_back(newPointerObj(&t_param1));
        params.push_back(newPointerObj(&t_param2));
        params.push_back(newPointerObj(&t_param3));

        VALUE retval = makeProtectedCall(t_functionName, params);
        return asType<ReturnType>(retval);
      }


    /// Add a named global object to the ruby runtime
    template<typename ObjectType>
      void addObject(const std::string &t_objectName, ObjectType *t_obj)
      {
        VALUE r_obj = SWIG_NewPointerObj(t_obj, getTypeInfo<ObjectType *>(), 0);
        rb_define_variable(t_objectName.c_str(), &r_obj);
      }


  private:
    RubyInterpreter &operator=(const RubyInterpreter &) = delete;
    RubyInterpreter(const RubyInterpreter &) = delete;

    std::map<std::string, std::string> m_types;

    // Used for our rb_protect calls.
    static VALUE evaluateSimpleImpl(VALUE arg) 
    {
      return rb_eval_string(StringValuePtr(arg));
    }

    // Returns the swig_type_info for the templated type if it's been registered previously
    // with registerType
    template<typename Type>
      swig_type_info *getTypeInfo() const
      {
        auto itr = m_types.find(typeid(Type).name());

        if (itr == m_types.end())
        {
          throw std::runtime_error(std::string("Type has not been registered: ") + typeid(Type).name());
        }

        std::string t_typeName = itr->second;

        swig_type_info *sti = SWIG_TypeQuery(t_typeName.c_str());

        if (sti == nullptr) {
          throw std::runtime_error("Unable to lookup type info for type: " + t_typeName);
        }

        return sti;
      }

    VALUE makeProtectedCall(const std::string &t_functionName, std::vector<VALUE> &t_params)
    {
      // we go through all of this because we cannot rb_protect a call to
      // rb_funcall. At least not in any way I can find.
      std::stringstream params;

      for (size_t i = 0; i < t_params.size(); ++i)
      {
        std::stringstream ss;
        ss << "$embedded_ruby_param_" << i;

        rb_define_variable(ss.str().c_str(), &t_params[i]);

        params << ss.str();
        if (i < t_params.size() - 1)
        {
          params << ", ";
        }
      }

      std::string funcall = "$embedded_ruby_return = " + t_functionName + "(" + params.str() + ")";
      evalString(funcall);

      VALUE retval = rb_gv_get("$embedded_ruby_return");
      return retval;
    } 

    template<typename Param>
      VALUE newPointerObj(Param t_param)
      {
        swig_type_info *ti = getTypeInfo<Param>();
        return SWIG_NewPointerObj(t_param, ti, 0);
      }

    VALUE newPointerObj(double *d)
    {
      return rb_float_new(*d);
    }

    VALUE newPointerObj(float *d)
    {
      return rb_float_new(*d);
    }

    VALUE newPointerObj(int *d)
    {
      return rb_to_int(rb_float_new(*d));
    }

    VALUE newPointerObj(long *d)
    {
      return rb_to_int(rb_float_new(*d));
    }

    VALUE newPointerObj(std::string *s)
    {
      return rb_str_new2(s->c_str());
    }


    template<typename Type>
      Type asType(VALUE v)
      {
        Type *ptr = nullptr;

        int res = SWIG_ConvertPtr(v, reinterpret_cast<void **>(&ptr), getTypeInfo<Type *>(), 0);
        if (SWIG_IsOK(res))
        {
          if (!ptr)
          {
            throw std::runtime_error(std::string("Result is null, trying to get type: ") + typeid(Type).name());
          }

          if (SWIG_IsNewObj(res)) {
            Type obj = *ptr;
            delete ptr;
            return obj;
          } else {
            return *ptr;
          }
        } else {
          throw std::runtime_error(std::string("Unable to convert object to type: ") + typeid(Type).name());
        }
      }

};


#ifdef _MSC_VER
#pragma pop_macro("EWOULDBLOCK")
#pragma pop_macro("EINPROGRESS")
#pragma pop_macro("EALREADY")
#pragma pop_macro("ENOTSOCK")
#pragma pop_macro("EDESTADDRREQ")
#pragma pop_macro("EMSGSIZE")
#pragma pop_macro("EPROTOTYPE")
#pragma pop_macro("ENOPROTOOPT")
#pragma pop_macro("EPROTONOSUPPORT")
#pragma pop_macro("EOPNOTSUPP")
#pragma pop_macro("EAFNOSUPPORT")
#pragma pop_macro("EADDRINUSE")
#pragma pop_macro("EADDRNOTAVAIL")
#pragma pop_macro("ENETDOWN")
#pragma pop_macro("ENETUNREACH")
#pragma pop_macro("ENETRESET")
#pragma pop_macro("ECONNABORTED")
#pragma pop_macro("ECONNRESET")
#pragma pop_macro("ENOBUFS")
#pragma pop_macro("EISCONN")
#pragma pop_macro("ENOTCONN")
#pragma pop_macro("ETIMEDOUT")
#pragma pop_macro("ECONNREFUSED")
#pragma pop_macro("ELOOP")
#pragma pop_macro("EHOSTUNREACH")
#pragma pop_macro("try")
#pragma pop_macro("except")
#pragma pop_macro("finally")
#pragma pop_macro("leave")
#pragma pop_macro("OpenFile")
#pragma pop_macro("CharNext")
#pragma pop_macro("rb_w32_iswinnt")
#pragma pop_macro("rb_w32_iswin95")
#pragma pop_macro("rb_w32_iswinnt")
#pragma pop_macro("rb_w32_iswin95")
#pragma pop_macro("getc")
#pragma pop_macro("putc")
#pragma pop_macro("fgetc")
#pragma pop_macro("fputc")
#pragma pop_macro("getchar")
#pragma pop_macro("putchar")
#pragma pop_macro("fgetchar")
#pragma pop_macro("fputchar")
#pragma pop_macro("utime")
#pragma pop_macro("strcasecmp")
#pragma pop_macro("strncasecmp")
#pragma pop_macro("close")
#pragma pop_macro("fclose")
#pragma pop_macro("read")
#pragma pop_macro("write")
#pragma pop_macro("getpid")
#pragma pop_macro("sleep")
#pragma pop_macro("creat")
#pragma pop_macro("eof")
#pragma pop_macro("filelength")
#pragma pop_macro("mktemp")
#pragma pop_macro("tell")
#pragma pop_macro("_open")
#pragma pop_macro("sopen")
#pragma pop_macro("fopen")
#pragma pop_macro("fdopen")
#pragma pop_macro("fsopen")
#pragma pop_macro("fsync")
#pragma pop_macro("stat")
#pragma pop_macro("execv")
#pragma pop_macro("isatty")
#pragma pop_macro("mkdir")
#pragma pop_macro("rmdir")
#pragma pop_macro("unlink")
#pragma pop_macro("isascii")
#pragma pop_macro("vsnprintf")
#pragma pop_macro("snprintf")
#pragma pop_macro("isnan")
#pragma pop_macro("finite")
#pragma pop_macro("copysign")
#pragma pop_macro("scalb")
#pragma pop_macro("accept")
#pragma pop_macro("bind")
#pragma pop_macro("connect")
#pragma pop_macro("FD_SET")
#pragma pop_macro("FD_CLR")
#pragma pop_macro("FD_ISSET")
#pragma pop_macro("select")
#pragma pop_macro("getpeername")
#pragma pop_macro("getsockname")
#pragma pop_macro("getsockopt")
#pragma pop_macro("ioctlsocket")
#pragma pop_macro("listen")
#pragma pop_macro("recv")
#pragma pop_macro("recvfrom")
#pragma pop_macro("send")
#pragma pop_macro("sendto")
#pragma pop_macro("setsockopt")
#pragma pop_macro("shutdown")
#pragma pop_macro("socket")
#pragma pop_macro("gethostbyaddr")
#pragma pop_macro("gethostbyname")
#pragma pop_macro("gethostname")
#pragma pop_macro("getprotobyname")
#pragma pop_macro("getprotobynumber")
#pragma pop_macro("getservbyname")
#pragma pop_macro("getservbyport")
#pragma pop_macro("get_osfhandle")
#pragma pop_macro("getcwd")
#pragma pop_macro("getenv")
#pragma pop_macro("rename")
#pragma pop_macro("times")
#pragma pop_macro("Sleep")
#endif


#endif // UTILITIES_CORE_RUBYINTERPRETER_HPP
