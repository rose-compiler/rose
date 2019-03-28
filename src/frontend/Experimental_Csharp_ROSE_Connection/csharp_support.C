// C++ code calling a C# function.


#include <dlfcn.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <string>

#include <rose_paths.h>

// using namespace std;

bool process(std::string &lib, std::string &function, std::string sourceFileNameWithPath)
   {
     void *handle          = NULL;
     void (*process)(char*,char*) = NULL;
     char *error           = NULL;

     printf ("In C++ process(): calling dlopen() lib = %s \n",lib.c_str());

     handle = dlopen (lib.c_str(), RTLD_LAZY);

     printf ("In C++ process(): handle = %p \n",handle);

  // if (!handle)
     if (handle == NULL)
        {
          printf ("handle == NULL \n");

          fputs (dlerror(), stderr);
          return false;
        }

     printf ("In C++ process(): calling dlsym() function = %s \n",function.c_str());

     process = (void (*)(char*,char*)) dlsym(handle, function.c_str());

     printf ("In C++ process(): after call to dlsym(): process = %p \n",process);

  // if ((error = dlerror()) != nullptr)
     if ((error = dlerror()) != NULL)
        {
          printf ("error == NULL \n");

          fputs(error, stderr);
          return false;
        }

     printf ("In C++ process(): sourceFileNameWithPath = %s \n",sourceFileNameWithPath.c_str());

     printf ("In C++ process(): calling process() \n");

     char buffer[2000];
     char* s = strncpy(buffer, sourceFileNameWithPath.c_str(), 1000);
     s[1001] = '\0';

     char buffer_2[2000];
     char* top_builddir = strncpy(buffer_2,ROSE_AUTOMAKE_TOP_BUILDDIR.c_str(),1000);
     
     top_builddir[1001] = '\0';

     printf ("In C++ process(): top_builddir = %s s = %s \n",top_builddir,s);
     process(top_builddir,s);
     
     printf ("In C++ process(): calling dlclose() \n");
     dlclose(handle);

     printf ("Leaving C++ process() \n");
     return true;
   }
      

#ifdef BUILD_EXECUTABLE

// We need to define this when we build the executable (though it will not run).
// const std::string ROSE_AUTOMAKE_TOP_BUILDDIR = "";
// int main(int argc, char** argv)
#else

// #error "Should not be reached!"

// int csharp_main(int argc, char** argv)
int csharp_main(int argc, char** argv, std::string sourceFileNameWithPath)
#endif
   {
  // std::string lib("./Kazelib.so");
  // std::string lib("./csharp_support_lib.so");
     //~ std::string libdir = ROSE_AUTOMAKE_TOP_BUILDDIR + "/src/frontend/Experimental_Csharp_ROSE_Connection/csharp_support_lib.so";
     //~ std::string lib(libdir);

     std::string lib("libcsharpBuilder.so");

     printf ("In csharp_main(): lib = %s \n",lib.c_str());

  // std::string function("process");
     std::string function = "process";

     printf ("In csharp_main(): calling process(lib,function): lib = %s function = %s \n",lib.c_str(),function.c_str());

     printf ("In csharp_main(): sourceFileNameWithPath = %s \n",sourceFileNameWithPath.c_str());

  // assert ( process(lib, function) );
  // int status = process(lib, function);
     int status = process(lib, function, sourceFileNameWithPath);
     assert (status != 0);
     
     printf ("Leaving csharp_main() \n");

     return 0;
   }



