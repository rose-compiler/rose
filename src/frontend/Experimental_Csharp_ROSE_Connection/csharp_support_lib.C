// The code in this file is turned into a dynamic library (see the Makefile).

#include <rose_paths.h>

#include "csharp_support.h"
#include <mono/jit/jit.h>

// DQ (10/18/2017): I think these are required for mono_string_new().
#include <mono/metadata/object.h>
#include <mono/metadata/environment.h>

#include <mono/metadata/assembly.h>
#include <assert.h>
#include <mono/metadata/debug-helpers.h>

#include <string.h>

// static void processThroughMono(std::string& dll)
static void processThroughMono(std::string& dll, std::string sourceFileNameWithPath)
   {
     printf ("In C++ processThroughMono(): dll = %s \n",dll.c_str());

  // Initialize mono runtime
     MonoDomain* domain = mono_jit_init (dll.c_str());

     if (domain == NULL)
        {
          std::cout << "Error: mono_jit_init() returned NULL: dll = " << dll << std::endl;
        }
     assert(domain != NULL);

     MonoAssembly *assembly;

     printf ("In C++ processThroughMono(): calling mono_domain_assembly_open(): domain = %p dll = %s \n",domain,dll.c_str());

     assembly = mono_domain_assembly_open (domain, dll.c_str());

     printf ("In C++ mono_domain_assembly_open(): assembly = %p \n",assembly);

     if (assembly == NULL)
        {
          std::cout << "Error: mono_domain_assembly_open() returned NULL: dll = " << dll << std::endl;
        }
     assert(assembly != NULL);

     printf ("In C++ processThroughMono(): calling mono_assembly_get_image(): assembly = %p \n",assembly);

     MonoImage *image =  mono_assembly_get_image  (assembly);
     assert(image);

     printf ("In C++ processThroughMono(): calling mono_method_desc_new(): TestDLL.Program:process \n");

     MonoMethodDesc* desc = mono_method_desc_new ("TestDLL.Program:process", true);
     assert(desc);

     printf ("In C++ processThroughMono(): calling mono_method_desc_search_in_image(): desc = %p image = %p \n",desc,image);

     MonoMethod* method = mono_method_desc_search_in_image (desc, image);
     assert(method);

     printf ("In C++ processThroughMono(): calling Container(): method = %p \n",method);

  // Create our container
     Container* c = new Container();

     printf ("In C++ processThroughMono(): sourceFileNameWithPath = %s \n",sourceFileNameWithPath.c_str());

     char buffer[2000];
  // char* s = const_cast<char*>(sourceFileNameWithPath.c_str());
     char* s = strncpy(buffer,sourceFileNameWithPath.c_str(),1000);
     s[1001] = '\0';

  // StringBuilder sss;

     printf ("In C++ processThroughMono(): s = %s \n",s);


     printf ("In C++ processThroughMono(): Cast to uint64: c = %p \n",c);

  // Cast to uint64
  // uint64_t ptr = reinterpret_cast<uint64_t>(c);

  // Fill it as an argument before the mono method invokation
     void* args[1];
  // void* args[2];

     printf ("In C++ processThroughMono(): Fill it as an argument before the mono method invokation \n");

  // args[0] = &ptr;
  // args[1] = (void*)s;
     args[0] = mono_string_new(domain, s);

     printf ("In C++ processThroughMono(): Invoke C# code \n");

  // Invoke C# code
     mono_runtime_invoke (method, nullptr, args, nullptr);

     printf ("In C++ processThroughMono(): Clean mono runtime \n");

  // Clean mono runtime
     mono_jit_cleanup (domain);

     printf ("In C++ processThroughMono(): call dump function on C++ data strcuture \n");

  // We did it!
     c->dump();

     printf ("Leaving C++ processThroughMono() \n");
   }

#if 0
void process()
#else
// void process( std::string sourceFileNameWithPath)
void process( char* sourceFileNameWithPath_char)
#endif
   {
     std::cout << "process()!!" << std::endl;

  // std::string dll("test.dll");
  // std::string dll("csharp_main.dll");
  // std::string dll = ROSE_AUTOMAKE_TOP_BUILDDIR + "/src/frontend/Experimental_Csharp_ROSE_Connection/csharp_main.dll";

  // Note reference to source directory (not cleazr how to specify locaion of dll built by mcs).
  //                   ROSE_AUTOMAKE_TOP_SRCDIR
  // std::string dll = ROSE_AUTOMAKE_TOP_SRCDIR + "/src/frontend/Experimental_Csharp_ROSE_Connection/csharp_main.dll";
  // std::string dll = ROSE_AUTOMAKE_TOP_BUILDDIR + "/src/frontend/Experimental_Csharp_ROSE_Connection/csharp_main.dll";
     std::string dll = "/data1/ROSE_CompileTree/git-LINUX-64bit-4.8.4-EDG49-BOOST_1_60-dq-language-development-rc/src/frontend/Experimental_Csharp_ROSE_Connection/csharp_main.dll";

#if 0
     std::string sourceFileNameWithPath = "unknown";
     printf ("In C++ process(std::string): sourceFileNameWithPath = %s \n",sourceFileNameWithPath.c_str());
#else
     printf ("In C++ process(std::string): sourceFileNameWithPath = %s \n",sourceFileNameWithPath_char);
#endif

     std::string sourceFileNameWithPath = sourceFileNameWithPath_char;

     printf ("In C++ process(std::string): calling processThroughMono(): dll = %s \n",dll.c_str());

  // processThroughMono(dll);
     processThroughMono(dll,sourceFileNameWithPath);

     printf ("In C++ process(std::string): DONE: calling processThroughMono(): dll = %s \n",dll.c_str());
   }

void storeResults(uint64_t container,int value)
   {
     Container* c = reinterpret_cast<Container*>(container);
     c->storeValue(value);

   }

