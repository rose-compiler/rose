#ifndef AT_TO_UNTYPED_JOVIAL_TRAVERSAL_H
#define AT_TO_UNTYPED_JOVIAL_TRAVERSAL_H

#include <aterm2.h>
#include "FASTNodes.hpp"

namespace OFP {

class ATtoUntypedJovialTraversal
{
 public:
   ATtoUntypedJovialTraversal(SgSourceFile* source);
   virtual ~ATtoUntypedJovialTraversal();

   SgUntypedFile*        get_file()  { return pUntypedFile; }
   SgUntypedGlobalScope* get_scope() { return pUntypedFile->get_scope(); }

   std::string getCurrentFilename()
      {
         return pSourceFile->get_sourceFileNameWithPath();
      }

 protected:
   SgUntypedFile* pUntypedFile;
   SgSourceFile*  pSourceFile;

 public:

// 1.1
ATbool traverse_Module(ATerm term);

// 1.2.1 COMPOOL MODULES
ATbool traverse_CompoolModule(ATerm term, SgUntypedScope* scope);

// 1.2.2 PROCEDURE MODULES
ATbool traverse_ProcedureModule(ATerm term, SgUntypedScope* scope);

// 1.2.3 MAIN PROGRAM MODULES
ATbool traverse_MainProgramModule(ATerm term, SgUntypedScope* scope);

}; // class ATtoUntypedJovialTraversal
}  // namespace OFP

#endif
