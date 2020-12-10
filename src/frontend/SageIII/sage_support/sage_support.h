#ifndef ROSE_SAGESUPPORT_H
#define ROSE_SAGESUPPORT_H

/**
 * \file    sage_support.h
 * \author  Justin Too <too1@llnl.gov>
 * \date    April 4, 2012
 */

/*-----------------------------------------------------------------------------
 *  Dependencies
 *
 *  TODO:
 *  1. Move #includes into .cpp file if they are not needed here.
 *---------------------------------------------------------------------------*/
// tps (01/14/2010) : Switching from rose.h to sage3.
// sage3basic.h should not be included by librose header files, only by .C files per policy. [Robb P. Matzke 2015-01-07]
//#include "sage3basic.h"

#include "rose_paths.h"
#include "astPostProcessing.h"
#include <sys/stat.h>

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
   #include "omp_lowering.h"
#endif

#include "attachPreprocessingInfo.h"

#include "BinaryLoader.h"
#include "sageBuilder.h"

#include "CollectionHelper.h"
#include "IncludeDirective.h"
#include "CompilerOutputParser.h"
#include "IncludingPreprocessingInfosCollector.h"

#ifdef _MSC_VER
//#pragma message ("WARNING: wait.h header file not available in MSVC.")
#else
#include <sys/wait.h>

   #ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
      #include "PHPFrontend.h"
      #include "PythonFrontend.h"
   #endif
#endif

#ifdef _MSC_VER
//#pragma message ("WARNING: libgen.h header file not available in MSVC.")
#else
#include <libgen.h>
#endif

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

#ifdef HAVE_DLADDR
   #include <dlfcn.h>
#endif

#if USING_OLD_EXECUTABLE_FORMAT_SUPPORT
// DQ (8/12/2008): This constructor is implemented in sageSupport.C and
// will be removed later once the new IR nodes are integrated into use.
   #include "ExecELF.h"
#endif

// Needed for boost::filesystem::exists(...)
#include "boost/filesystem.hpp"
#include <stdio.h>

// Liao 10/8/2010, refactored OpenMP related code to ompAstConstruction.C
#include "ompAstConstruction.h"

#ifdef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
   #include "transformationSupport.h"
#endif

namespace Rose {
namespace Frontend {
  int Run(SgProject* project);
  int RunSerial(SgProject* project);
namespace Java {
  int Run(SgProject* project);
namespace Ecj {
//#ifdef ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
  // TOO1 (2/13/2014): Declared in src/frontend/ECJ_ROSE_Connection/openJavaParser_main.C.
  extern ROSE_DLL_API SgProject* Ecj_globalProjectPointer;

  int Run(SgProject* project);
  int RunBatchMode(SgProject* project);

  /**
   * @param argc out variable
   * @param argv out variable
   */
  std::vector<std::string> GetCommandline(
      std::vector<std::string> argv,
      const SgProject* project,
      int& o_argc,
      char*** o_argv);

  std::string GetClasspath(const SgProject* project);
  std::string GetClasspath(const SgProject* project);
  std::string GetSourcepath(const SgProject* project);
  std::string GetSourceVersion(const SgProject* project);
  std::string GetTargetVersion(const SgProject* project);
  std::string GetVerbosity(const SgProject* project);
}// ::Rose::Frontend::Java::Ecj
}// ::Rose::Frontend::Java
}// ::Rose::Frontend

namespace Backend {
namespace Java {
  int CompileBatch(SgProject* project, std::vector<std::string> argv);
  std::string CreateDestdir(SgProject* project);
}// ::Rose::Backend::Java
}// ::Rose::Backend


}// ::Rose

#ifdef ROSE_BUILD_X10_LANGUAGE_SUPPORT
namespace Rose {
namespace Frontend {
namespace X10 {
namespace X10c {
  extern SgSourceFile* X10c_globalFilePointer;
  extern std::list<std::string> classNames;
}// ::Rose::Frontend::X10::X10c
}// ::Rose::Frontend::X10
}// ::Rose::Frontend
}// ::Rose
#endif


#endif // ROSE_SAGESUPPORT_H

