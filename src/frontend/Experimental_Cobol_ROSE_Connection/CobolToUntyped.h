#ifndef COBOL_TO_UNTYPED_H
#define COBOL_TO_UNTYPED_H

#include <fstream>
#include <string>
#include "gnucobpt.h"

namespace CobolSupport {

class CobolToUntyped
{
 public:
   CobolToUntyped(SgSourceFile* source);
   virtual ~CobolToUntyped();

   SgUntypedFile*        get_file()  { return pUntypedFile; }
   SgUntypedGlobalScope* get_scope() { return pUntypedFile->get_scope(); }

   std::string getCurrentFilename()
      {
         return pSourceFile->get_sourceFileNameWithPath();
      }

   void convert_CobolSourceProgram(const struct cb_program* prog, struct cobpt_base_list* local_cache);

 protected:
   SgUntypedFile* pUntypedFile;
   SgSourceFile*  pSourceFile;

}; // class CobolToUntyped
}  // namespace CobolSupport

#endif
