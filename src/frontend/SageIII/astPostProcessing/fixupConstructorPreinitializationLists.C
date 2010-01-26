// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "fixupConstructorPreinitializationLists.h"
// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

void
resetContructorInitilizerLists()
   {
  // DQ (3/10/2007): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Reset Contructor pre-Initilization Lists:");

     ResetContructorInitilizerLists t;

  // t.traverseMemoryPool();
     SgCtorInitializerList::traverseMemoryPoolNodes(t);
   }

void
ResetContructorInitilizerLists::visit(SgNode* node)
   {
#if 0
     printf ("##### ResetContructorInitilizerLists::visit(node = %p = %s) \n",node,node->sage_class_name());
#endif

     SgCtorInitializerList* ctorInitializerList = isSgCtorInitializerList(node);
     ROSE_ASSERT(ctorInitializerList != NULL);

     SgInitializedNamePtrList & ctorList = ctorInitializerList->get_ctors();

     SgInitializedNamePtrList::iterator i = ctorList.begin();
     while (i != ctorList.end())
        {
          SgInitializedName* initializedName = *i;

       // This is likely a base class, but even if it is a data member initialization of a class type we still want to check it
       // Actually I think we need to distinguish between these to case (as they appear in "B(int i): A<int>(i), x(7),a(99) {};")
       // But we could just cheat for now and look for "____L" substrings in the name
          SgClassType* classType = isSgClassType(initializedName->get_type());
          if (classType != NULL)
             {
               string name = initializedName->get_name().str();
               if (name.find("____L") != string::npos)
                  {
                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
                    ROSE_ASSERT(classDeclaration != NULL);
#if PRINT_DEVELOPER_WARNINGS
                    printf ("Error: found a base class name that needs to be updated for initializedName = %p name = %s resetting using %s \n",
                         initializedName,initializedName->get_name().str(),classDeclaration->get_name().str());
#endif
                    initializedName->set_name(classDeclaration->get_name());
                  }
             }

          i++;
        }
   }


