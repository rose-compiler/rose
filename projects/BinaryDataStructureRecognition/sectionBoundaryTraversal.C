#include "rose.h"

#include "astStructRecognition.h"

using namespace std;

void
SectionBoundaryTraversal::visit(SgNode* n)
   {
     SgAsmGenericSection* asmGenericSection = isSgAsmGenericSection(n);
     if (asmGenericSection != NULL)
        {
       // unsigned long address = asmGenericSection->get_starting_memory_address();
          unsigned long address = asmGenericSection->get_mapped_preferred_rva();
          unsigned long offset = asmGenericSection->get_starting_memory_address();

       // DQ (8/30/2013): get_name() is now an SgName instead oc a std::string.
       // printf ("asmGenericSection class_name() = %s = %s = address = %p \n",asmGenericSection->class_name().c_str(),asmGenericSection->get_name().get_string().c_str(),(void*)address);
       // printf ("asmGenericSection class_name() = %s = %s  address = %p address+offset = %p \n",asmGenericSection->class_name().c_str(),asmGenericSection->get_name()->c_str(),(void*)address,(void*)(offset+address));
          printf ("asmGenericSection class_name() = %s = %s  address = %p address+offset = %p \n",asmGenericSection->class_name().c_str(),asmGenericSection->get_name()->get_string().c_str(),(void*)address,(void*)(offset+address));

       // SgAsmElfSection* asmElfSection = isSgAsmElfSection(n);
       // ROSE_ASSERT(asmElfSection != NULL);

          if (asmGenericSection->is_mapped())
             {
               size_t base  = asmGenericSection->get_mapped_preferred_rva();
               size_t bound = base + asmGenericSection->get_mapped_size();

               printf ("   base = %p bound = %p \n",(void*)base,(void*)bound);
             }
            else
             {
               printf ("   not a mapped section \n");
             }
        }
   }

vector<SgAsmGenericSection*>
generateSectionList ( SgProject* project )
   {
     ROSE_ASSERT (project != NULL);

  // Build the traversal object
     SectionBoundaryTraversal t;

  // Call the traversal starting at the project node of the AST 
  // can be specified to be preorder or postorder).
     t.traverseInputFiles(project,preorder);

     printf ("t.sectionList.size() = %zu \n",t.sectionList.size());

     return t.sectionList;
   }



