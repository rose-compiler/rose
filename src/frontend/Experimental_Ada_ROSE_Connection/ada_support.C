// C++ code calling an Ada function.

// #include <rose_paths.h>
// #include <rose.h>

// DQ (11/13/2017): This is a violation, sage3basic.h must be the first file included.
// #include "rose_config.h"

#include "sage3basic.h"

#include "rose_config.h"

// #include <assert.h>

#include "ada_support.h"

// using namespace std;

// #include <stdio.h>
#include "a_nodes.h"
#include "tool_2_wrapper.h"

// extern "C" void dot_asisinit (void);
// extern "C" void dot_asisfinal (void);

#ifdef BUILD_EXECUTABLE
int main(int argc, char** argv)
#else
  int ada_main(int argc, char** argv, SgSourceFile* file)
#endif
   {
     int status = 0;

#if 1
     printf ("In ada_support.C: In ada_main(): calling ada support for file = %s \n",file->getFileName().c_str());
#endif

     const char *prefix = "call_asis_tool_2.main";
  // char *target_file = "../test_units/unit_2.adb";
  // const char *target_file = file->getFileName().c_str();
     char target_file[1024];
     size_t size = file->getFileName().length();
     if (size >= 1024)
        {
          printf ("Error in preparing filename for ada support. \n");
          ROSE_ASSERT(false);
        }

     strncpy(target_file,file->getFileName().c_str(),size+1);
  // target_file[size+1] = '\0';

  // char *gnat_home   = "/usr/workspace/wsb/charles/bin/adacore/gnat-gpl-2017-x86_64-linux";
     char *gnat_home   = std::getenv("GNAT_HOME");

     if (!gnat_home) gnat_home = "/home/quinlan1/ROSE/ADA/x86_64-linux/adagpl-2017/gnatgpl/gnat-gpl-2017-x86_64-linux-bin";

  // struct List_Node_Struct *head_node = NULL;
  // List_Node_Struct *head_node = NULL;
     Nodes_Struct head_nodes;

     printf ("%s:  BEGIN.\n", prefix);

#if 1
  // DQ (9/15/2017): Updated to include output directory.
     char* outputDirectory = "";

  // DQ (31/8/2017): Definitions of these functions still need to be provided to via libraries to be able to link ROSE.
     dot_asisinit();
     head_nodes = tool_2_wrapper (target_file, gnat_home,outputDirectory);

     if (head_nodes.Elements == NULL) {
        printf ("%s:  tool_2_wrapper returned NO elements.\n", prefix);
     } else {
        printf ("%s:  tool_2_wrapper returned %i elements.\n" , prefix, head_nodes.Elements->Next_Count + 1);
     }
#endif

     printf ("%s:  END.\n", prefix);

     ROSE_ASSERT (status == 0);

     Ada_ROSE_Translation::ada_to_ROSE_translation(head_nodes,file);

     dot_asisfinal();

     printf ("Leaving ada_main(): status = %d \n",status);

     return status;
   }



