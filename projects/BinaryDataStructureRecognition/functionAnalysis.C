#include "rose.h"

#include "astStructRecognition.h"

using namespace std;
using namespace rose;

FunctionAnalysisAttribute::FunctionAnalysisAttribute ( string n, SgAsmFunctionDeclaration* f, SgProject* p )
   : name(n), function(f), project(p)
   {
  // Do any initial analysis here!
   }

MemberFunctionAnalysisAttribute::MemberFunctionAnalysisAttribute ( string n, SgAsmFunctionDeclaration* f, SgProject* p )
   : FunctionAnalysisAttribute(n,f,p)
   {
  // Do any initial analysis here!

     associatedVirtualTable             = globalScopeAttribute->associatedVirtualFunctionTable(f);
     associatedDataStructureDeclaration = NULL;


   }


void
addGenericCommentsAboutFunctions ( SgAsmFunctionDeclaration* asmFunction )
   {
     string name = asmFunction->get_name();

  // List the different functions and there predefined semantics.
     string comment;

#if 0
     printf ("In addGenericCommentsAboutFunctions(): name = %s \n",name.c_str());
#endif

     if (name == "__gmon_start__")
        {
          comment = "Predefined Semantics: This function responsible for starting up the profiler (see csu/gmon-start.c)";
        }

     if (name == "__libc_start_main")
        {
       // Notice now to add extra lines to the comments we have to add the commetn markers as well.
          comment = "Predefined Semantics: The __libc_start_main() function shall perform any necessary initialization of the execution environment, \n"
                    "// call the main function with appropriate arguments, and handle the return from main(). \n"
                    "// If the main() function returns, the return value shall be passed to the exit() function."
                    "// Note: __libc_start_main() is not in the source standard; it is only in the binary standard.";
        }

     if (name == "_init" || name == "_fini")
        {
          comment = "Predefined Semantics: Two special functions are defined for initializing and finalizing an executable: _init and _fini.";
        }

     if (name == "_start")
        {
          comment = "Predefined Semantics: This is the real entry of the binary executable (as loaded byu the OS) \n"
                    "// sets up the environment, opens the default descriptors, and then calls the application's main() function.";
        }

     if (name == "call_gmon_start")
        {
          comment = "Predefined Semantics: The function call_gmon_start initializes the gmon profiling system. This system \n"
                    "// is enabled when binaries are compiled with the -pg flag, and creates output for \n"
                    "// use with gprof(1).";
        }

     if (name == "__do_global_dtors_aux")
        {
          comment = "Predefined Semantics: This function responsible for calling the destructors for global objects (C++ specific)";
        }

     if (name == "__do_global_ctors_aux")
        {
          comment = "Predefined Semantics: This function responsible for calling the constructors for global objects (C++ specific)";
        }

     if (name == "__i686.get_pc_thunk.bx")
        {
          comment = "Predefined Semantics: This function is a standard method for acquiring the address of the GOT at runtime and \n"
                    "// storing the result in ebx. The name is what gcc uses by convention when generating PIC objects."
                    "// We filter the name to generate a valid function name.";
        }

     if (name == "frame_dummy")
        {
          comment = "Predefined Semantics: This function is a wrapper for __register_frame_info which is in \n"
                    "// turn a wrapper for __register_frame_info_bases.";
        }

     if (name == "__libc_csu_init" || name == "__libc_csu_fini")
        {
       // comment = "Predefined Semantics: I don't know what these __libc_csu_init and __libc_csu_fini functions do.";
          comment = "These functions are passed to __libc_start_main by the startup code. \n"
                    "// These get statically linked into each program.  For dynamically linked \n"
                    "// programs, this module will come from libc_nonshared.a and differs from \n"
                    "// the libc.a module in that it doesn't call the preinit array.";
        }

     if (name == "main")
        {
          comment = "Predefined Semantics: This function is the user's \"main\" function.";
        }

  // Add the predefined comment if it was specified.
     if (comment.empty() == false)
          asmFunction->set_comment(comment);
   }





void addFunctionAnalysisAttributes( SgProject* project )
   {
     struct Visitor: public AstSimpleProcessing
        {
       // Save the project so that we can use it in the visit function
          SgProject* project;
          int counter;

          GlobalScopeAnalysisAttribute* globalScopeAnalysisAttribute;

          Visitor( SgProject* p ) : project(p), counter(0) {}

          virtual void visit(SgNode* astNode)
             {
            // Note that the SgAsmInterpretation will be visited before the instructions (functions).
               SgAsmInterpretation* asmGlobalScope = isSgAsmInterpretation(astNode);
               if (asmGlobalScope != NULL)
                  {
                    globalScopeAnalysisAttribute = dynamic_cast<GlobalScopeAnalysisAttribute*>(asmGlobalScope->getAttribute("GlobalScopeAnalysisAttribute"));
                    ROSE_ASSERT(globalScopeAnalysisAttribute != NULL);
                  }

            // Handle functions as they are visited in the AST.

            // DQ (8/30/2013): This type was renamed and so the original function is not available (changed).
            // SgAsmFunctionDeclaration* asmFunction = isSgAsmFunctionDeclaration(astNode);
               SgAsmFunctionDeclaration* asmFunction = isSgAsmFunction(astNode);
               if (asmFunction != NULL)
                  {
                 // This was initalized from the traversal constructor.
                    ROSE_ASSERT(project != NULL);

                 // This was initalized from earlier in the traversal (preorder traversal is assumed).
                    ROSE_ASSERT(globalScopeAnalysisAttribute != NULL);
#if 0
                    string name = "function_" + StringUtility::numberToString(counter);
#else
                    string name = asmFunction->get_name();
                    if (name == "")
                       {
                         name = "unnamed_function_" + StringUtility::numberToString(counter);
                       }
#endif
                    printf ("name = %s kind = %u \n",name.c_str(),asmFunction->get_function_kind());

                 // Add a collection of predefined comments for common functions
                    addGenericCommentsAboutFunctions(asmFunction);

                    if (globalScopeAnalysisAttribute->isVirtualMemberFunction(asmFunction) == true)
                       {
                         printf ("Found a virtual function = %p \n",(void*)asmFunction->get_address());

                      // Rename the function.
                         name = "virtual_member_function_" + StringUtility::numberToString(counter);
                         MemberFunctionAnalysisAttribute* functionAnalysisAttribute = new MemberFunctionAnalysisAttribute(name,asmFunction,project);
                         ROSE_ASSERT(functionAnalysisAttribute != NULL);

                      // Add it to the AST (so it can be found later in another pass over the AST)
                         asmFunction->addNewAttribute("FunctionAnalysisAttribute",functionAnalysisAttribute);
#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                       }
                      else
                       {
                         FunctionAnalysisAttribute* functionAnalysisAttribute = new FunctionAnalysisAttribute(name,asmFunction,project);
                         ROSE_ASSERT(functionAnalysisAttribute != NULL);

                      // Add it to the AST (so it can be found later in another pass over the AST)
                         asmFunction->addNewAttribute("FunctionAnalysisAttribute",functionAnalysisAttribute);

                      // Add the function to global scope
                         globalScopeAnalysisAttribute->associatedDeclarationList.push_back(asmFunction);
                       }

                    counter++;
                  }
             }
        };

      Visitor v(project);

      v.traverse(project, preorder);
   }


