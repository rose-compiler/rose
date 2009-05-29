
#ifndef ASM_FUNCTIONS_H
#define ASM_FUNCTIONS_H

#include <string>
#include <map>

class SgNode;

class AsmFunctions
{
   public:
      AsmFunctions( SgNode *node );

      SgNode *getNode( const std::string& name ) const;

   private:
      std::map<std::string, SgNode *> functionNames;
};

#endif
