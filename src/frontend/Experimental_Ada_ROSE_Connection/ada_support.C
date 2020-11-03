// C++ code calling an Ada function.

// #include <rose_paths.h>
// #include <rose.h>

// DQ (11/13/2017): This is a violation, sage3basic.h must be the first file included.
// #include "rose_config.h"

#include "sage3basic.h"
#include "rose_config.h"

#include <boost/filesystem.hpp>

#include "ada_support.h"

// using namespace std;

// #include <stdio.h>
#include "a_nodes.h"
#include "tool_2_wrapper.h"

// extern "C" void dot_asisinit (void);
// extern "C" void dot_asisfinal (void);

namespace boostfs = boost::filesystem;

namespace Ada_ROSE_Translation
{
  Sawyer::Message::Facility mlog;
}


#ifdef BUILD_EXECUTABLE
int main(int argc, char** argv)
#else
  int ada_main(int argc, char** argv, SgSourceFile* file)
#endif
   {
     using Ada_ROSE_Translation::mlog;

     ROSE_INITIALIZE;

     int status = 0;
     const char *prefix = "call_asis_tool_2.main";

     mlog = Sawyer::Message::Facility("Ada2ROSE", Rose::Diagnostics::destination);
     mprintf ("In ada_support.C: In ada_main(): calling ada support for file = %s \n",file->getFileName().c_str());

  // char *target_file = "../test_units/unit_2.adb";
  // const char *target_file = file->getFileName().c_str();

  // char *gnat_home   = "/usr/workspace/wsb/charles/bin/adacore/gnat-gpl-2017-x86_64-linux";
     const char *gnat_home   = std::getenv("GNAT_HOME");

     if (!gnat_home) gnat_home = "/home/quinlan1/ROSE/ADA/x86_64-linux/adagpl-2017/gnatgpl/gnat-gpl-2017-x86_64-linux-bin";

  // struct List_Node_Struct *head_node = NULL;
  // List_Node_Struct *head_node = NULL;

     mprintf ("%s:  BEGIN.\n", prefix);

     Nodes_Struct head_nodes;

#if 1
     {
       typedef boostfs::path::string_type string_type;

    // DQ (9/15/2017): Updated to include output directory.
    // PP (10/31/20): Produce Ada temp+obj files in src-file specific directory.
       boostfs::path currentDir    = boostfs::current_path();
       string_type   srcFile       = file->getFileName();
       string_type   gnatOutputDir = currentDir.string<string_type>();

       gnatOutputDir += boostfs::path::preferred_separator;
       gnatOutputDir += "gnatOutput";

       // create a new output directory for every import file to support
       // parallel compilation (e.g., testing).
       boostfs::create_directory(gnatOutputDir);

       size_t pos = srcFile.rfind(boostfs::path::preferred_separator);

       if (pos == string_type::npos) pos = 0;

       gnatOutputDir += boostfs::path::preferred_separator;
       gnatOutputDir += srcFile.substr(pos);
       gnatOutputDir += "-obj";

       boostfs::create_directory(gnatOutputDir);
       boostfs::current_path(gnatOutputDir);

       string_type xyz = boostfs::current_path().string<string_type>();

       mprintf ("changed working directory: %s\n", xyz.c_str());

       char* cstring_SrcFile = const_cast<char*>(srcFile.c_str());
       char* cstring_GnatOutputDir = const_cast<char*>(gnatOutputDir.c_str());

    // DQ (31/8/2017): Definitions of these functions still need to be provided to via libraries to be able to link ROSE.
       dot_asisinit();
       head_nodes = tool_2_wrapper (cstring_SrcFile, const_cast<char*>(gnat_home), cstring_GnatOutputDir);

       if (head_nodes.Elements == NULL) {
          mprintf ("%s:  tool_2_wrapper returned NO elements.\n", prefix);
          status = 1;
       } else {
          mprintf ("%s:  tool_2_wrapper returned %i elements.\n" , prefix, head_nodes.Elements->Next_Count + 1);
       }

       boostfs::current_path(currentDir);
     }
#endif

     mprintf ("%s:  END.\n", prefix);

     Ada_ROSE_Translation::ada_to_ROSE_translation(head_nodes, file);

     dot_asisfinal();

     mprintf ("Leaving ada_main(): status = %d \n", status);

     return status;
   }



