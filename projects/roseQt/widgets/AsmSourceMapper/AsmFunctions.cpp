
#include "rose.h"

#include <iostream>

#include "AsmFunctions.h"

using namespace std;

AsmFunctions::AsmFunctions( SgNode *node )
{
   typedef Rose_STL_Container<SgNode *>::const_iterator const_iterator;

   Rose_STL_Container<SgNode *> asmFunctions =
      NodeQuery::querySubTree( node, V_SgAsmDwarfSubprogram );

   cout << "Found " << asmFunctions.size() << " Dwarf Subprograms." << endl;

   for( const_iterator i = asmFunctions.begin(); i != asmFunctions.end(); ++i )
   {
      SgAsmDwarfSubprogram *dwarfSub( isSgAsmDwarfSubprogram( *i ) );
      assert( dwarfSub );

      cout << dwarfSub->get_name() << endl;

      SgAsmDwarfLine       *dwarfLine( dwarfSub->get_source_position() );
      if( !dwarfLine )
      {
         cout << "No Dwarf Line available ..." << endl;
         continue;
      }

      //assert( fun );

      const std::string& name( dwarfSub->get_name() );
      uint64_t address( dwarfLine->get_address() );

      if( name.empty() )
      {
         continue;
      }

      /*if( functionNames.find( name ) != functionNames.end() )
      {
         cout << "function name " << name << ": multiple occurence" << endl;
      }*/
      
      cout << "found function " << name << " at adress " << address << endl;
      //functionNames.insert( make_pair( name, fun ) );
   }
}

SgNode *AsmFunctions::getNode( const std::string& name ) const
{
   std::map<std::string, SgNode *>::const_iterator pos( functionNames.find( name ) );

   if( pos == functionNames.end() )
      return NULL;

   return pos->second;
}
