// MACHINE GENERATED SOURCE FILE WITH ROSE (Grammar.h)--- DO NOT MODIFY!

#include "sage3basic.h"

#include "AST_FILE_IO.h"

// The header file ("rose_config.h") should only be included by source files that require it.
#include "rose_config.h"

#if _MSC_VER
#define USE_CPP_NEW_DELETE_OPERATORS 0
#endif


using namespace std;

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgNode::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgNode -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 32 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgSupport::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgSupport -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 48 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgModifier::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgModifier -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 64 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgModifierNodes::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgModifierNodes -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_next == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 81 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgConstVolatileModifier::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgConstVolatileModifier -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 97 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgStorageModifier::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgStorageModifier -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 113 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAccessModifier::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAccessModifier -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 129 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgFunctionModifier::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgFunctionModifier -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_opencl_vec_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 146 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUPC_AccessModifier::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUPC_AccessModifier -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 162 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgSpecialFunctionModifier::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgSpecialFunctionModifier -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 178 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgElaboratedTypeModifier::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgElaboratedTypeModifier -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 194 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgLinkageModifier::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgLinkageModifier -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 210 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgBaseClassModifier::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgBaseClassModifier -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 226 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeModifier::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeModifier -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 242 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgDeclarationModifier::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgDeclarationModifier -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 258 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOpenclAccessModeModifier::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOpenclAccessModeModifier -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 274 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgName::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgName -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 290 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgSymbolTable::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgSymbolTable -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_table == NULL )
          indexCounter++;
       else
          for ( rose_hash_multimap::const_iterator it_table= p_table->begin(); it_table != p_table->end(); ++it_table) 
             {
               if ( it_table->second == childNode ) { return indexCounter; } indexCounter++;
             }
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 313 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAttribute::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAttribute -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 329 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgPragma::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgPragma -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 346 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgBitAttribute::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgBitAttribute -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 362 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgFuncDecl_attr::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgFuncDecl_attr -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 378 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgClassDecl_attr::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgClassDecl_attr -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 394 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
Sg_File_Info::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of Sg_File_Info -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 410 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgFile::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgFile -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 427 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgSourceFile::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgSourceFile -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_globalScope == childNode ) { return indexCounter; } indexCounter++;
          for ( SgModuleStatementPtrList::const_iterator source_module_list_iterator = p_module_list.begin(); source_module_list_iterator != p_module_list.end(); ++source_module_list_iterator) 
             { 
               if ( *source_module_list_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
          for ( SgTokenPtrList::const_iterator source_token_list_iterator = p_token_list.begin(); source_token_list_iterator != p_token_list.end(); ++source_token_list_iterator) 
             { 
               if ( *source_token_list_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 453 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgBinaryComposite::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgBinaryComposite -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_genericFileList == childNode ) { return indexCounter; } indexCounter++;
     if ( p_interpretations == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 472 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUnknownFile::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUnknownFile -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_globalScope == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 490 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgProject::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgProject -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_fileList_ptr == childNode ) { return indexCounter; } indexCounter++;
     if ( p_directoryList == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 508 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOptions::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOptions -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 524 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUnparse_Info::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUnparse_Info -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_declstatement_ptr == childNode ) { return indexCounter; } indexCounter++;
     if ( p_current_context == childNode ) { return indexCounter; } indexCounter++;
          for ( static SgTypePtrList::const_iterator source_structureTagProcessingList_iterator = p_structureTagProcessingList.begin(); source_structureTagProcessingList_iterator != p_structureTagProcessingList.end(); ++source_structureTagProcessingList_iterator) 
             { 
               if ( *source_structureTagProcessingList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_current_namespace == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_current_function_call == childNode ) { return indexCounter; } indexCounter++;
     if ( p_current_scope == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 553 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgBaseClass::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgBaseClass -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_base_class == childNode ) { return indexCounter; } indexCounter++;
     if ( p_baseClassModifier == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 571 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypedefSeq::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypedefSeq -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgTypePtrList::const_iterator source_typedefs_iterator = p_typedefs.begin(); source_typedefs_iterator != p_typedefs.end(); ++source_typedefs_iterator) 
             { 
               if ( *source_typedefs_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 591 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTemplateParameter::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTemplateParameter -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_defaultTypeParameter == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_defaultExpressionParameter == childNode ) { return indexCounter; } indexCounter++;
     if ( p_templateDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_defaultTemplateDeclarationParameter == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 613 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTemplateArgument::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTemplateArgument -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_templateDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 632 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgDirectory::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgDirectory -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_fileList == childNode ) { return indexCounter; } indexCounter++;
     if ( p_directoryList == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 650 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgFileList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgFileList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgFilePtrList::const_iterator source_listOfFiles_iterator = p_listOfFiles.begin(); source_listOfFiles_iterator != p_listOfFiles.end(); ++source_listOfFiles_iterator) 
             { 
               if ( *source_listOfFiles_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 670 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgDirectoryList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgDirectoryList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgDirectoryPtrList::const_iterator source_listOfDirectories_iterator = p_listOfDirectories.begin(); source_listOfDirectories_iterator != p_listOfDirectories.end(); ++source_listOfDirectories_iterator) 
             { 
               if ( *source_listOfDirectories_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 690 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgFunctionParameterTypeList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgFunctionParameterTypeList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgTypePtrList::const_iterator source_arguments_iterator = p_arguments.begin(); source_arguments_iterator != p_arguments.end(); ++source_arguments_iterator) 
             { 
               if ( *source_arguments_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 710 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgQualifiedName::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgQualifiedName -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_scope == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 727 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTemplateArgumentList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTemplateArgumentList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgTemplateArgumentPtrList::const_iterator source_args_iterator = p_args.begin(); source_args_iterator != p_args.end(); ++source_args_iterator) 
             { 
               if ( *source_args_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 747 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTemplateParameterList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTemplateParameterList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgTemplateParameterPtrList::const_iterator source_args_iterator = p_args.begin(); source_args_iterator != p_args.end(); ++source_args_iterator) 
             { 
               if ( *source_args_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 767 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgGraph::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgGraph -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 783 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgIncidenceDirectedGraph::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgIncidenceDirectedGraph -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 799 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgBidirectionalGraph::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgBidirectionalGraph -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 815 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgStringKeyedBidirectionalGraph::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgStringKeyedBidirectionalGraph -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 831 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgIntKeyedBidirectionalGraph::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgIntKeyedBidirectionalGraph -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 847 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgIncidenceUndirectedGraph::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgIncidenceUndirectedGraph -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 863 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgGraphNode::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgGraphNode -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_SgNode == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 880 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgGraphEdge::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgGraphEdge -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_node_A == childNode ) { return indexCounter; } indexCounter++;
     if ( p_node_B == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 898 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgDirectedGraphEdge::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgDirectedGraphEdge -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_node_A == childNode ) { return indexCounter; } indexCounter++;
     if ( p_node_B == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 916 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUndirectedGraphEdge::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUndirectedGraphEdge -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_node_A == childNode ) { return indexCounter; } indexCounter++;
     if ( p_node_B == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 934 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgGraphNodeList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgGraphNodeList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 950 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgGraphEdgeList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgGraphEdgeList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 966 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeTable::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeTable -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_type_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 983 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgNameGroup::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgNameGroup -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 999 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgDimensionObject::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgDimensionObject -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_array == childNode ) { return indexCounter; } indexCounter++;
     if ( p_shape == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1017 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgFormatItem::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgFormatItem -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_data == childNode ) { return indexCounter; } indexCounter++;
     if ( p_format_item_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1035 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgFormatItemList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgFormatItemList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgFormatItemPtrList::const_iterator source_format_item_list_iterator = p_format_item_list.begin(); source_format_item_list_iterator != p_format_item_list.end(); ++source_format_item_list_iterator) 
             { 
               if ( *source_format_item_list_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1055 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgDataStatementGroup::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgDataStatementGroup -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgDataStatementObjectPtrList::const_iterator source_object_list_iterator = p_object_list.begin(); source_object_list_iterator != p_object_list.end(); ++source_object_list_iterator) 
             { 
               if ( *source_object_list_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
          for ( SgDataStatementValuePtrList::const_iterator source_value_list_iterator = p_value_list.begin(); source_value_list_iterator != p_value_list.end(); ++source_value_list_iterator) 
             { 
               if ( *source_value_list_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1079 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgDataStatementObject::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgDataStatementObject -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_variableReference_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1096 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgDataStatementValue::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgDataStatementValue -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_initializer_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_repeat_expression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_constant_expression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1115 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1136 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeUnknown::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeUnknown -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1157 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeChar::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeChar -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1178 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeSignedChar::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeSignedChar -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1199 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeUnsignedChar::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeUnsignedChar -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1220 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeShort::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeShort -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1241 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeSignedShort::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeSignedShort -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1262 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeUnsignedShort::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeUnsignedShort -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1283 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeInt::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeInt -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1304 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeSignedInt::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeSignedInt -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1325 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeUnsignedInt::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeUnsignedInt -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1346 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeLong::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeLong -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1367 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeSignedLong::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeSignedLong -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1388 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeUnsignedLong::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeUnsignedLong -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1409 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeVoid::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeVoid -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1430 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeGlobalVoid::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeGlobalVoid -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1451 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeWchar::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeWchar -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1472 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeFloat::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeFloat -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1493 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeDouble::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeDouble -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1514 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeLongLong::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeLongLong -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1535 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeSignedLongLong::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeSignedLongLong -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1556 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeUnsignedLongLong::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeUnsignedLongLong -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1577 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeLongDouble::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeLongDouble -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1598 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeString::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeString -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lengthExpression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1620 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeBool::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeBool -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1641 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgPointerType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgPointerType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_base_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1663 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgPointerMemberType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgPointerMemberType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_class_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_base_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1686 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgReferenceType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgReferenceType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_base_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1708 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgNamedType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgNamedType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_declaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1730 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgClassType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgClassType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_declaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1752 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgEnumType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgEnumType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_declaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1774 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypedefType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypedefType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent_scope == childNode ) { return indexCounter; } indexCounter++;
     if ( p_declaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1797 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgModifierType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgModifierType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_base_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1819 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgFunctionType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgFunctionType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_return_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_orig_return_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_argument_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1843 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgMemberFunctionType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgMemberFunctionType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_class_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_return_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_orig_return_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_argument_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1868 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgPartialFunctionType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgPartialFunctionType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_class_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_return_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_orig_return_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_argument_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1893 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgPartialFunctionModifierType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgPartialFunctionModifierType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_class_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_return_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_orig_return_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_argument_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1918 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgArrayType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgArrayType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_base_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_index == childNode ) { return indexCounter; } indexCounter++;
     if ( p_dim_info == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1942 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeEllipse::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeEllipse -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1963 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTemplateType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTemplateType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 1984 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgQualifiedNameType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgQualifiedNameType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_base_type == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2010 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeComplex::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeComplex -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_base_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2032 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeImaginary::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeImaginary -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_base_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2054 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeDefault::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeDefault -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2075 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeCAFTeam::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeCAFTeam -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2096 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeCrayPointer::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeCrayPointer -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2117 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeLabel::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeLabel -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ref_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_ptr_to == childNode ) { return indexCounter; } indexCounter++;
     if ( p_modifiers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_typedefs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_kind == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2138 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgLocatedNode::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgLocatedNode -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2156 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgLocatedNodeSupport::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgLocatedNodeSupport -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2174 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgCommonBlockObject::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgCommonBlockObject -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_variable_reference_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2193 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgInitializedName::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgInitializedName -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_typeptr == childNode ) { return indexCounter; } indexCounter++;
     if ( p_initptr == childNode ) { return indexCounter; } indexCounter++;
     if ( p_prev_decl_item == childNode ) { return indexCounter; } indexCounter++;
     if ( p_declptr == childNode ) { return indexCounter; } indexCounter++;
     if ( p_storageModifier == childNode ) { return indexCounter; } indexCounter++;
     if ( p_scope == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2217 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgInterfaceBody::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgInterfaceBody -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_functionDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2236 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgRenamePair::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgRenamePair -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2254 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpClause::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpClause -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2272 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpOrderedClause::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpOrderedClause -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2290 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpNowaitClause::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpNowaitClause -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2308 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpUntiedClause::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpUntiedClause -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2326 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpDefaultClause::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpDefaultClause -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2344 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpExpressionClause::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpExpressionClause -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_expression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2363 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpCollapseClause::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpCollapseClause -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_expression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2382 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpIfClause::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpIfClause -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_expression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2401 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpNumThreadsClause::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpNumThreadsClause -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_expression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2420 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpVariablesClause::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpVariablesClause -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgVarRefExpPtrList::const_iterator source_variables_iterator = p_variables.begin(); source_variables_iterator != p_variables.end(); ++source_variables_iterator) 
             { 
               if ( *source_variables_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2442 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpCopyprivateClause::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpCopyprivateClause -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgVarRefExpPtrList::const_iterator source_variables_iterator = p_variables.begin(); source_variables_iterator != p_variables.end(); ++source_variables_iterator) 
             { 
               if ( *source_variables_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2464 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpPrivateClause::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpPrivateClause -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgVarRefExpPtrList::const_iterator source_variables_iterator = p_variables.begin(); source_variables_iterator != p_variables.end(); ++source_variables_iterator) 
             { 
               if ( *source_variables_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2486 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpFirstprivateClause::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpFirstprivateClause -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgVarRefExpPtrList::const_iterator source_variables_iterator = p_variables.begin(); source_variables_iterator != p_variables.end(); ++source_variables_iterator) 
             { 
               if ( *source_variables_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2508 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpSharedClause::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpSharedClause -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgVarRefExpPtrList::const_iterator source_variables_iterator = p_variables.begin(); source_variables_iterator != p_variables.end(); ++source_variables_iterator) 
             { 
               if ( *source_variables_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2530 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpCopyinClause::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpCopyinClause -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgVarRefExpPtrList::const_iterator source_variables_iterator = p_variables.begin(); source_variables_iterator != p_variables.end(); ++source_variables_iterator) 
             { 
               if ( *source_variables_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2552 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpLastprivateClause::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpLastprivateClause -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgVarRefExpPtrList::const_iterator source_variables_iterator = p_variables.begin(); source_variables_iterator != p_variables.end(); ++source_variables_iterator) 
             { 
               if ( *source_variables_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2574 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpReductionClause::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpReductionClause -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgVarRefExpPtrList::const_iterator source_variables_iterator = p_variables.begin(); source_variables_iterator != p_variables.end(); ++source_variables_iterator) 
             { 
               if ( *source_variables_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2596 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpScheduleClause::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpScheduleClause -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_chunk_size == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2615 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2634 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgScopeStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgScopeStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_symbol_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2655 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgGlobal::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgGlobal -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgDeclarationStatementPtrList::const_iterator source_declarations_iterator = p_declarations.begin(); source_declarations_iterator != p_declarations.end(); ++source_declarations_iterator) 
             { 
               if ( *source_declarations_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_symbol_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2680 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgBasicBlock::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgBasicBlock -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgStatementPtrList::const_iterator source_statements_iterator = p_statements.begin(); source_statements_iterator != p_statements.end(); ++source_statements_iterator) 
             { 
               if ( *source_statements_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_symbol_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2705 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgIfStmt::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgIfStmt -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_conditional == childNode ) { return indexCounter; } indexCounter++;
     if ( p_true_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_false_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_else_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_end_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_symbol_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2731 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgForStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgForStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_for_init_stmt == childNode ) { return indexCounter; } indexCounter++;
     if ( p_test == childNode ) { return indexCounter; } indexCounter++;
     if ( p_increment == childNode ) { return indexCounter; } indexCounter++;
     if ( p_loop_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_symbol_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2756 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgFunctionDefinition::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgFunctionDefinition -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_symbol_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2778 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgClassDefinition::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgClassDefinition -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgDeclarationStatementPtrList::const_iterator source_members_iterator = p_members.begin(); source_members_iterator != p_members.end(); ++source_members_iterator) 
             { 
               if ( *source_members_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
          for ( SgBaseClassPtrList::const_iterator source_inheritances_iterator = p_inheritances.begin(); source_inheritances_iterator != p_inheritances.end(); ++source_inheritances_iterator) 
             { 
               if ( *source_inheritances_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_symbol_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2807 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTemplateInstantiationDefn::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTemplateInstantiationDefn -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgDeclarationStatementPtrList::const_iterator source_members_iterator = p_members.begin(); source_members_iterator != p_members.end(); ++source_members_iterator) 
             { 
               if ( *source_members_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
          for ( SgBaseClassPtrList::const_iterator source_inheritances_iterator = p_inheritances.begin(); source_inheritances_iterator != p_inheritances.end(); ++source_inheritances_iterator) 
             { 
               if ( *source_inheritances_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_symbol_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2836 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgWhileStmt::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgWhileStmt -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_condition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_end_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_symbol_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2860 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgDoWhileStmt::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgDoWhileStmt -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_condition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_symbol_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2883 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgSwitchStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgSwitchStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_item_selector == childNode ) { return indexCounter; } indexCounter++;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_end_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_symbol_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2907 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgCatchOptionStmt::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgCatchOptionStmt -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_condition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_trystmt == childNode ) { return indexCounter; } indexCounter++;
     if ( p_symbol_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2931 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgNamespaceDefinitionStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgNamespaceDefinitionStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgDeclarationStatementPtrList::const_iterator source_declarations_iterator = p_declarations.begin(); source_declarations_iterator != p_declarations.end(); ++source_declarations_iterator) 
             { 
               if ( *source_declarations_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_namespaceDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_symbol_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2957 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgBlockDataStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgBlockDataStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_symbol_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 2979 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAssociateStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAssociateStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_variable_declaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_symbol_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3002 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgFortranDo::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgFortranDo -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_initialization == childNode ) { return indexCounter; } indexCounter++;
     if ( p_bound == childNode ) { return indexCounter; } indexCounter++;
     if ( p_increment == childNode ) { return indexCounter; } indexCounter++;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_end_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_symbol_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3028 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgFortranNonblockedDo::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgFortranNonblockedDo -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_end_statement == childNode ) { return indexCounter; } indexCounter++;
     if ( p_initialization == childNode ) { return indexCounter; } indexCounter++;
     if ( p_bound == childNode ) { return indexCounter; } indexCounter++;
     if ( p_increment == childNode ) { return indexCounter; } indexCounter++;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_end_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_symbol_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3055 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgForAllStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgForAllStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_forall_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_end_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_symbol_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3079 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUpcForAllStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUpcForAllStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_for_init_stmt == childNode ) { return indexCounter; } indexCounter++;
     if ( p_test == childNode ) { return indexCounter; } indexCounter++;
     if ( p_increment == childNode ) { return indexCounter; } indexCounter++;
     if ( p_affinity == childNode ) { return indexCounter; } indexCounter++;
     if ( p_loop_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_symbol_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3105 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgCAFWithTeamStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgCAFWithTeamStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_teamId == childNode ) { return indexCounter; } indexCounter++;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_symbol_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3128 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgFunctionTypeTable::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgFunctionTypeTable -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_function_type_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3148 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgDeclarationStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgDeclarationStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3173 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgFunctionParameterList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgFunctionParameterList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgInitializedNamePtrList::const_iterator source_args_iterator = p_args.begin(); source_args_iterator != p_args.end(); ++source_args_iterator) 
             { 
               if ( *source_args_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3202 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgVariableDeclaration::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgVariableDeclaration -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_baseTypeDefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgInitializedNamePtrList::const_iterator source_variables_iterator = p_variables.begin(); source_variables_iterator != p_variables.end(); ++source_variables_iterator) 
             { 
               if ( *source_variables_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3232 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgVariableDefinition::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgVariableDefinition -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_vardefn == childNode ) { return indexCounter; } indexCounter++;
     if ( p_bitfield == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3259 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgClinkageDeclarationStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgClinkageDeclarationStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3284 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgClinkageStartStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgClinkageStartStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3309 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgClinkageEndStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgClinkageEndStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3334 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgEnumDeclaration::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgEnumDeclaration -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_type == childNode ) { return indexCounter; } indexCounter++;
          for ( SgInitializedNamePtrList::const_iterator source_enumerators_iterator = p_enumerators.begin(); source_enumerators_iterator != p_enumerators.end(); ++source_enumerators_iterator) 
             { 
               if ( *source_enumerators_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_scope == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3365 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmStmt::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmStmt -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgExpressionPtrList::const_iterator source_operands_iterator = p_operands.begin(); source_operands_iterator != p_operands.end(); ++source_operands_iterator) 
             { 
               if ( *source_operands_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3394 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAttributeSpecificationStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAttributeSpecificationStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parameter_list == childNode ) { return indexCounter; } indexCounter++;
          for ( SgDataStatementGroupPtrList::const_iterator source_data_statement_group_list_iterator = p_data_statement_group_list.begin(); source_data_statement_group_list_iterator != p_data_statement_group_list.end(); ++source_data_statement_group_list_iterator) 
             { 
               if ( *source_data_statement_group_list_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_bind_list == childNode ) { return indexCounter; } indexCounter++;
          for ( SgDimensionObjectPtrList::const_iterator source_dimension_object_list_iterator = p_dimension_object_list.begin(); source_dimension_object_list_iterator != p_dimension_object_list.end(); ++source_dimension_object_list_iterator) 
             { 
               if ( *source_dimension_object_list_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3429 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgFormatStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgFormatStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_format_item_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3455 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTemplateDeclaration::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTemplateDeclaration -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgTemplateParameterPtrList::const_iterator source_templateParameters_iterator = p_templateParameters.begin(); source_templateParameters_iterator != p_templateParameters.end(); ++source_templateParameters_iterator) 
             { 
               if ( *source_templateParameters_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_scope == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3485 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTemplateInstantiationDirectiveStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTemplateInstantiationDirectiveStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_declaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3511 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUseStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUseStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgRenamePairPtrList::const_iterator source_rename_list_iterator = p_rename_list.begin(); source_rename_list_iterator != p_rename_list.end(); ++source_rename_list_iterator) 
             { 
               if ( *source_rename_list_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_module == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3541 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgParameterStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgParameterStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3566 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgNamespaceDeclarationStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgNamespaceDeclarationStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3592 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgEquivalenceStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgEquivalenceStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_equivalence_set_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3618 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgInterfaceStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgInterfaceStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgInterfaceBodyPtrList::const_iterator source_interface_body_list_iterator = p_interface_body_list.begin(); source_interface_body_list_iterator != p_interface_body_list.end(); ++source_interface_body_list_iterator) 
             { 
               if ( *source_interface_body_list_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_end_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3648 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgNamespaceAliasDeclarationStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgNamespaceAliasDeclarationStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_namespaceDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3674 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgCommonBlock::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgCommonBlock -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgCommonBlockObjectPtrList::const_iterator source_block_list_iterator = p_block_list.begin(); source_block_list_iterator != p_block_list.end(); ++source_block_list_iterator) 
             { 
               if ( *source_block_list_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3703 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypedefDeclaration::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypedefDeclaration -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_base_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_declaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent_scope == childNode ) { return indexCounter; } indexCounter++;
     if ( p_scope == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3733 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgStatementFunctionStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgStatementFunctionStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_function == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3760 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgCtorInitializerList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgCtorInitializerList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgInitializedNamePtrList::const_iterator source_ctors_iterator = p_ctors.begin(); source_ctors_iterator != p_ctors.end(); ++source_ctors_iterator) 
             { 
               if ( *source_ctors_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3789 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgPragmaDeclaration::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgPragmaDeclaration -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_pragma == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3815 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUsingDirectiveStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUsingDirectiveStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_namespaceDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3841 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgClassDeclaration::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgClassDeclaration -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_scope == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3869 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTemplateInstantiationDecl::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTemplateInstantiationDecl -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_templateDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgTemplateArgumentPtrList::const_iterator source_templateArguments_iterator = p_templateArguments.begin(); source_templateArguments_iterator != p_templateArguments.end(); ++source_templateArguments_iterator) 
             { 
               if ( *source_templateArguments_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_scope == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3902 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgDerivedTypeStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgDerivedTypeStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_end_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_scope == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3931 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgModuleStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgModuleStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_end_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_scope == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3960 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgImplicitStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgImplicitStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgInitializedNamePtrList::const_iterator source_variables_iterator = p_variables.begin(); source_variables_iterator != p_variables.end(); ++source_variables_iterator) 
             { 
               if ( *source_variables_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 3989 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUsingDeclarationStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUsingDeclarationStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_declaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_initializedName == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4016 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgNamelistStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgNamelistStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgNameGroupPtrList::const_iterator source_group_list_iterator = p_group_list.begin(); source_group_list_iterator != p_group_list.end(); ++source_group_list_iterator) 
             { 
               if ( *source_group_list_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4045 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgImportStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgImportStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgExpressionPtrList::const_iterator source_import_list_iterator = p_import_list.begin(); source_import_list_iterator != p_import_list.end(); ++source_import_list_iterator) 
             { 
               if ( *source_import_list_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4074 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgFunctionDeclaration::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgFunctionDeclaration -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parameterList == childNode ) { return indexCounter; } indexCounter++;
          for ( SgTypePtrList::const_iterator source_exceptionSpecification_iterator = p_exceptionSpecification.begin(); source_exceptionSpecification_iterator != p_exceptionSpecification.end(); ++source_exceptionSpecification_iterator) 
             { 
               if ( *source_exceptionSpecification_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_scope == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4107 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgMemberFunctionDeclaration::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgMemberFunctionDeclaration -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_CtorInitializerList == childNode ) { return indexCounter; } indexCounter++;
     if ( p_associatedClassDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parameterList == childNode ) { return indexCounter; } indexCounter++;
          for ( SgTypePtrList::const_iterator source_exceptionSpecification_iterator = p_exceptionSpecification.begin(); source_exceptionSpecification_iterator != p_exceptionSpecification.end(); ++source_exceptionSpecification_iterator) 
             { 
               if ( *source_exceptionSpecification_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_scope == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4142 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTemplateInstantiationMemberFunctionDecl::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTemplateInstantiationMemberFunctionDecl -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_templateDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgTemplateArgumentPtrList::const_iterator source_templateArguments_iterator = p_templateArguments.begin(); source_templateArguments_iterator != p_templateArguments.end(); ++source_templateArguments_iterator) 
             { 
               if ( *source_templateArguments_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_CtorInitializerList == childNode ) { return indexCounter; } indexCounter++;
     if ( p_associatedClassDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parameterList == childNode ) { return indexCounter; } indexCounter++;
          for ( SgTypePtrList::const_iterator source_exceptionSpecification_iterator = p_exceptionSpecification.begin(); source_exceptionSpecification_iterator != p_exceptionSpecification.end(); ++source_exceptionSpecification_iterator) 
             { 
               if ( *source_exceptionSpecification_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_scope == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4182 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTemplateInstantiationFunctionDecl::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTemplateInstantiationFunctionDecl -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_templateDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgTemplateArgumentPtrList::const_iterator source_templateArguments_iterator = p_templateArguments.begin(); source_templateArguments_iterator != p_templateArguments.end(); ++source_templateArguments_iterator) 
             { 
               if ( *source_templateArguments_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parameterList == childNode ) { return indexCounter; } indexCounter++;
          for ( SgTypePtrList::const_iterator source_exceptionSpecification_iterator = p_exceptionSpecification.begin(); source_exceptionSpecification_iterator != p_exceptionSpecification.end(); ++source_exceptionSpecification_iterator) 
             { 
               if ( *source_exceptionSpecification_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_scope == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4220 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgProgramHeaderStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgProgramHeaderStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_end_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parameterList == childNode ) { return indexCounter; } indexCounter++;
          for ( SgTypePtrList::const_iterator source_exceptionSpecification_iterator = p_exceptionSpecification.begin(); source_exceptionSpecification_iterator != p_exceptionSpecification.end(); ++source_exceptionSpecification_iterator) 
             { 
               if ( *source_exceptionSpecification_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_scope == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4254 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgProcedureHeaderStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgProcedureHeaderStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_end_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_result_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parameterList == childNode ) { return indexCounter; } indexCounter++;
          for ( SgTypePtrList::const_iterator source_exceptionSpecification_iterator = p_exceptionSpecification.begin(); source_exceptionSpecification_iterator != p_exceptionSpecification.end(); ++source_exceptionSpecification_iterator) 
             { 
               if ( *source_exceptionSpecification_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_scope == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4289 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgEntryStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgEntryStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_result_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parameterList == childNode ) { return indexCounter; } indexCounter++;
          for ( SgTypePtrList::const_iterator source_exceptionSpecification_iterator = p_exceptionSpecification.begin(); source_exceptionSpecification_iterator != p_exceptionSpecification.end(); ++source_exceptionSpecification_iterator) 
             { 
               if ( *source_exceptionSpecification_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_scope == childNode ) { return indexCounter; } indexCounter++;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4323 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgContainsStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgContainsStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4348 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgC_PreprocessorDirectiveStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgC_PreprocessorDirectiveStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4373 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgIncludeDirectiveStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgIncludeDirectiveStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4398 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgDefineDirectiveStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgDefineDirectiveStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4423 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUndefDirectiveStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUndefDirectiveStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4448 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgIfdefDirectiveStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgIfdefDirectiveStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4473 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgIfndefDirectiveStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgIfndefDirectiveStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4498 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgIfDirectiveStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgIfDirectiveStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4523 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgDeadIfDirectiveStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgDeadIfDirectiveStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4548 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgElseDirectiveStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgElseDirectiveStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4573 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgElseifDirectiveStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgElseifDirectiveStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4598 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgEndifDirectiveStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgEndifDirectiveStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4623 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgLineDirectiveStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgLineDirectiveStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4648 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgWarningDirectiveStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgWarningDirectiveStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4673 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgErrorDirectiveStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgErrorDirectiveStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4698 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgEmptyDirectiveStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgEmptyDirectiveStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4723 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgIncludeNextDirectiveStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgIncludeNextDirectiveStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4748 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgIdentDirectiveStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgIdentDirectiveStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4773 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgLinemarkerDirectiveStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgLinemarkerDirectiveStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4798 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpThreadprivateStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpThreadprivateStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgVarRefExpPtrList::const_iterator source_variables_iterator = p_variables.begin(); source_variables_iterator != p_variables.end(); ++source_variables_iterator) 
             { 
               if ( *source_variables_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4827 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgFortranIncludeLine::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgFortranIncludeLine -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4852 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgJavaImportStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgJavaImportStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_definingDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_firstNondefiningDeclaration == childNode ) { return indexCounter; } indexCounter++;
          for ( SgQualifiedNamePtrList::const_iterator source_qualifiedNameList_iterator = p_qualifiedNameList.begin(); source_qualifiedNameList_iterator != p_qualifiedNameList.end(); ++source_qualifiedNameList_iterator) 
             { 
               if ( *source_qualifiedNameList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4877 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgExprStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgExprStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_expression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4897 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgLabelStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgLabelStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_scope == childNode ) { return indexCounter; } indexCounter++;
     if ( p_statement == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4918 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgCaseOptionStmt::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgCaseOptionStmt -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_key == childNode ) { return indexCounter; } indexCounter++;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_key_range_end == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4940 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTryStmt::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTryStmt -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_catch_statement_seq_root == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4961 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgDefaultOptionStmt::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgDefaultOptionStmt -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 4981 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgBreakStmt::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgBreakStmt -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5000 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgContinueStmt::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgContinueStmt -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5019 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgReturnStmt::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgReturnStmt -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_expression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5039 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgGotoStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgGotoStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_label_expression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5060 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgSpawnStmt::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgSpawnStmt -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_the_func == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5080 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgNullStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgNullStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5099 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgVariantStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgVariantStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5118 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgForInitStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgForInitStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgStatementPtrList::const_iterator source_init_stmt_iterator = p_init_stmt.begin(); source_init_stmt_iterator != p_init_stmt.end(); ++source_init_stmt_iterator) 
             { 
               if ( *source_init_stmt_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5141 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgCatchStatementSeq::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgCatchStatementSeq -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgStatementPtrList::const_iterator source_catch_statement_seq_iterator = p_catch_statement_seq.begin(); source_catch_statement_seq_iterator != p_catch_statement_seq.end(); ++source_catch_statement_seq_iterator) 
             { 
               if ( *source_catch_statement_seq_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5164 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgStopOrPauseStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgStopOrPauseStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_code == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5184 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgIOStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgIOStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_io_stmt_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_unit == childNode ) { return indexCounter; } indexCounter++;
     if ( p_iostat == childNode ) { return indexCounter; } indexCounter++;
     if ( p_err == childNode ) { return indexCounter; } indexCounter++;
     if ( p_iomsg == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5208 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgPrintStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgPrintStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_format == childNode ) { return indexCounter; } indexCounter++;
     if ( p_io_stmt_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_unit == childNode ) { return indexCounter; } indexCounter++;
     if ( p_iostat == childNode ) { return indexCounter; } indexCounter++;
     if ( p_err == childNode ) { return indexCounter; } indexCounter++;
     if ( p_iomsg == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5233 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgReadStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgReadStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_format == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rec == childNode ) { return indexCounter; } indexCounter++;
     if ( p_end == childNode ) { return indexCounter; } indexCounter++;
     if ( p_namelist == childNode ) { return indexCounter; } indexCounter++;
     if ( p_advance == childNode ) { return indexCounter; } indexCounter++;
     if ( p_size == childNode ) { return indexCounter; } indexCounter++;
     if ( p_eor == childNode ) { return indexCounter; } indexCounter++;
     if ( p_asynchronous == childNode ) { return indexCounter; } indexCounter++;
     if ( p_io_stmt_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_unit == childNode ) { return indexCounter; } indexCounter++;
     if ( p_iostat == childNode ) { return indexCounter; } indexCounter++;
     if ( p_err == childNode ) { return indexCounter; } indexCounter++;
     if ( p_iomsg == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5265 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgWriteStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgWriteStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_format == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rec == childNode ) { return indexCounter; } indexCounter++;
     if ( p_namelist == childNode ) { return indexCounter; } indexCounter++;
     if ( p_advance == childNode ) { return indexCounter; } indexCounter++;
     if ( p_asynchronous == childNode ) { return indexCounter; } indexCounter++;
     if ( p_io_stmt_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_unit == childNode ) { return indexCounter; } indexCounter++;
     if ( p_iostat == childNode ) { return indexCounter; } indexCounter++;
     if ( p_err == childNode ) { return indexCounter; } indexCounter++;
     if ( p_iomsg == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5294 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOpenStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOpenStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_status == childNode ) { return indexCounter; } indexCounter++;
     if ( p_access == childNode ) { return indexCounter; } indexCounter++;
     if ( p_form == childNode ) { return indexCounter; } indexCounter++;
     if ( p_recl == childNode ) { return indexCounter; } indexCounter++;
     if ( p_blank == childNode ) { return indexCounter; } indexCounter++;
     if ( p_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_action == childNode ) { return indexCounter; } indexCounter++;
     if ( p_delim == childNode ) { return indexCounter; } indexCounter++;
     if ( p_pad == childNode ) { return indexCounter; } indexCounter++;
     if ( p_round == childNode ) { return indexCounter; } indexCounter++;
     if ( p_sign == childNode ) { return indexCounter; } indexCounter++;
     if ( p_asynchronous == childNode ) { return indexCounter; } indexCounter++;
     if ( p_io_stmt_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_unit == childNode ) { return indexCounter; } indexCounter++;
     if ( p_iostat == childNode ) { return indexCounter; } indexCounter++;
     if ( p_err == childNode ) { return indexCounter; } indexCounter++;
     if ( p_iomsg == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5331 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgCloseStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgCloseStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_status == childNode ) { return indexCounter; } indexCounter++;
     if ( p_io_stmt_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_unit == childNode ) { return indexCounter; } indexCounter++;
     if ( p_iostat == childNode ) { return indexCounter; } indexCounter++;
     if ( p_err == childNode ) { return indexCounter; } indexCounter++;
     if ( p_iomsg == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5356 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgInquireStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgInquireStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_access == childNode ) { return indexCounter; } indexCounter++;
     if ( p_form == childNode ) { return indexCounter; } indexCounter++;
     if ( p_recl == childNode ) { return indexCounter; } indexCounter++;
     if ( p_blank == childNode ) { return indexCounter; } indexCounter++;
     if ( p_exist == childNode ) { return indexCounter; } indexCounter++;
     if ( p_opened == childNode ) { return indexCounter; } indexCounter++;
     if ( p_number == childNode ) { return indexCounter; } indexCounter++;
     if ( p_named == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_sequential == childNode ) { return indexCounter; } indexCounter++;
     if ( p_direct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_formatted == childNode ) { return indexCounter; } indexCounter++;
     if ( p_unformatted == childNode ) { return indexCounter; } indexCounter++;
     if ( p_nextrec == childNode ) { return indexCounter; } indexCounter++;
     if ( p_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_action == childNode ) { return indexCounter; } indexCounter++;
     if ( p_read == childNode ) { return indexCounter; } indexCounter++;
     if ( p_write == childNode ) { return indexCounter; } indexCounter++;
     if ( p_readwrite == childNode ) { return indexCounter; } indexCounter++;
     if ( p_delim == childNode ) { return indexCounter; } indexCounter++;
     if ( p_pad == childNode ) { return indexCounter; } indexCounter++;
     if ( p_asynchronous == childNode ) { return indexCounter; } indexCounter++;
     if ( p_decimal == childNode ) { return indexCounter; } indexCounter++;
     if ( p_stream == childNode ) { return indexCounter; } indexCounter++;
     if ( p_size == childNode ) { return indexCounter; } indexCounter++;
     if ( p_pending == childNode ) { return indexCounter; } indexCounter++;
     if ( p_iolengthExp == childNode ) { return indexCounter; } indexCounter++;
     if ( p_io_stmt_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_unit == childNode ) { return indexCounter; } indexCounter++;
     if ( p_iostat == childNode ) { return indexCounter; } indexCounter++;
     if ( p_err == childNode ) { return indexCounter; } indexCounter++;
     if ( p_iomsg == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5408 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgFlushStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgFlushStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_io_stmt_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_unit == childNode ) { return indexCounter; } indexCounter++;
     if ( p_iostat == childNode ) { return indexCounter; } indexCounter++;
     if ( p_err == childNode ) { return indexCounter; } indexCounter++;
     if ( p_iomsg == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5432 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgBackspaceStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgBackspaceStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_io_stmt_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_unit == childNode ) { return indexCounter; } indexCounter++;
     if ( p_iostat == childNode ) { return indexCounter; } indexCounter++;
     if ( p_err == childNode ) { return indexCounter; } indexCounter++;
     if ( p_iomsg == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5456 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgRewindStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgRewindStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_io_stmt_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_unit == childNode ) { return indexCounter; } indexCounter++;
     if ( p_iostat == childNode ) { return indexCounter; } indexCounter++;
     if ( p_err == childNode ) { return indexCounter; } indexCounter++;
     if ( p_iomsg == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5480 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgEndfileStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgEndfileStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_io_stmt_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_unit == childNode ) { return indexCounter; } indexCounter++;
     if ( p_iostat == childNode ) { return indexCounter; } indexCounter++;
     if ( p_err == childNode ) { return indexCounter; } indexCounter++;
     if ( p_iomsg == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5504 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgWaitStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgWaitStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_io_stmt_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_unit == childNode ) { return indexCounter; } indexCounter++;
     if ( p_iostat == childNode ) { return indexCounter; } indexCounter++;
     if ( p_err == childNode ) { return indexCounter; } indexCounter++;
     if ( p_iomsg == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5528 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgWhereStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgWhereStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_condition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_elsewhere == childNode ) { return indexCounter; } indexCounter++;
     if ( p_end_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5551 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgElseWhereStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgElseWhereStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_condition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_elsewhere == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5573 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgNullifyStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgNullifyStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_pointer_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5593 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgArithmeticIfStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgArithmeticIfStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_conditional == childNode ) { return indexCounter; } indexCounter++;
     if ( p_less_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_equal_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_greater_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5616 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAssignStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAssignStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_value == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5637 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgComputedGotoStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgComputedGotoStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_labelList == childNode ) { return indexCounter; } indexCounter++;
     if ( p_label_index == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5658 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAssignedGotoStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAssignedGotoStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_targets == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5678 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAllocateStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAllocateStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_expr_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_stat_expression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_errmsg_expression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_source_expression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5701 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgDeallocateStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgDeallocateStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_expr_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_stat_expression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_errmsg_expression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5723 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUpcNotifyStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUpcNotifyStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_notify_expression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5743 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUpcWaitStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUpcWaitStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_wait_expression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5763 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUpcBarrierStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUpcBarrierStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_barrier_expression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5783 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUpcFenceStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUpcFenceStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5802 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpBarrierStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpBarrierStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5821 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpTaskwaitStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpTaskwaitStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5840 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpFlushStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpFlushStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgVarRefExpPtrList::const_iterator source_variables_iterator = p_variables.begin(); source_variables_iterator != p_variables.end(); ++source_variables_iterator) 
             { 
               if ( *source_variables_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5863 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpBodyStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpBodyStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5883 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpAtomicStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpAtomicStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5903 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpMasterStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpMasterStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5923 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpOrderedStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpOrderedStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5943 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpCriticalStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpCriticalStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5963 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpSectionStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpSectionStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 5983 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpWorkshareStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpWorkshareStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6003 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpClauseBodyStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpClauseBodyStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgOmpClausePtrList::const_iterator source_clauses_iterator = p_clauses.begin(); source_clauses_iterator != p_clauses.end(); ++source_clauses_iterator) 
             { 
               if ( *source_clauses_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6027 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpParallelStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpParallelStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgOmpClausePtrList::const_iterator source_clauses_iterator = p_clauses.begin(); source_clauses_iterator != p_clauses.end(); ++source_clauses_iterator) 
             { 
               if ( *source_clauses_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6051 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpSingleStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpSingleStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgOmpClausePtrList::const_iterator source_clauses_iterator = p_clauses.begin(); source_clauses_iterator != p_clauses.end(); ++source_clauses_iterator) 
             { 
               if ( *source_clauses_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6075 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpTaskStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpTaskStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgOmpClausePtrList::const_iterator source_clauses_iterator = p_clauses.begin(); source_clauses_iterator != p_clauses.end(); ++source_clauses_iterator) 
             { 
               if ( *source_clauses_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6099 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpForStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpForStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgOmpClausePtrList::const_iterator source_clauses_iterator = p_clauses.begin(); source_clauses_iterator != p_clauses.end(); ++source_clauses_iterator) 
             { 
               if ( *source_clauses_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6123 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpDoStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpDoStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgOmpClausePtrList::const_iterator source_clauses_iterator = p_clauses.begin(); source_clauses_iterator != p_clauses.end(); ++source_clauses_iterator) 
             { 
               if ( *source_clauses_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6147 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOmpSectionsStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOmpSectionsStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgOmpClausePtrList::const_iterator source_clauses_iterator = p_clauses.begin(); source_clauses_iterator != p_clauses.end(); ++source_clauses_iterator) 
             { 
               if ( *source_clauses_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6171 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgSequenceStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgSequenceStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_numeric_label == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6190 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6209 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUnaryOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUnaryOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6230 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgExpressionRoot::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgExpressionRoot -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6251 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgMinusOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgMinusOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6272 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUnaryAddOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUnaryAddOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6293 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgNotOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgNotOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6314 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgPointerDerefExp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgPointerDerefExp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6335 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAddressOfOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAddressOfOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6356 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgMinusMinusOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgMinusMinusOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6377 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgPlusPlusOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgPlusPlusOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6398 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgBitComplementOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgBitComplementOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6419 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgCastExp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgCastExp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6441 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgThrowOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgThrowOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6462 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgRealPartOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgRealPartOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6483 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgImagPartOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgImagPartOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6504 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgConjugateOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgConjugateOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6525 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUserDefinedUnaryOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUserDefinedUnaryOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_symbol == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6547 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgBinaryOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgBinaryOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6570 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgArrowExp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgArrowExp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6593 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgDotExp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgDotExp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6616 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgDotStarOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgDotStarOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6639 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgArrowStarOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgArrowStarOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6662 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgEqualityOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgEqualityOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6685 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgLessThanOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgLessThanOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6708 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgGreaterThanOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgGreaterThanOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6731 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgNotEqualOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgNotEqualOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6754 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgLessOrEqualOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgLessOrEqualOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6777 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgGreaterOrEqualOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgGreaterOrEqualOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6800 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAddOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAddOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6823 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgSubtractOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgSubtractOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6846 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgMultiplyOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgMultiplyOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6869 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgDivideOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgDivideOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6892 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgIntegerDivideOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgIntegerDivideOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6915 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgModOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgModOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6938 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAndOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAndOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6961 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgOrOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgOrOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 6984 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgBitXorOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgBitXorOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7007 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgBitAndOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgBitAndOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7030 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgBitOrOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgBitOrOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7053 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgCommaOpExp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgCommaOpExp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7076 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgLshiftOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgLshiftOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7099 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgRshiftOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgRshiftOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7122 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgPntrArrRefExp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgPntrArrRefExp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7145 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgScopeOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgScopeOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7168 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAssignOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAssignOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7191 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgPlusAssignOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgPlusAssignOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7214 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgMinusAssignOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgMinusAssignOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7237 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAndAssignOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAndAssignOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7260 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgIorAssignOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgIorAssignOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7283 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgMultAssignOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgMultAssignOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7306 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgDivAssignOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgDivAssignOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7329 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgModAssignOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgModAssignOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7352 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgXorAssignOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgXorAssignOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7375 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgLshiftAssignOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgLshiftAssignOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7398 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgRshiftAssignOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgRshiftAssignOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7421 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgExponentiationOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgExponentiationOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7444 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgConcatenationOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgConcatenationOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7467 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgPointerAssignOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgPointerAssignOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7490 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUserDefinedBinaryOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUserDefinedBinaryOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_symbol == childNode ) { return indexCounter; } indexCounter++;
     if ( p_lhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7514 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgExprListExp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgExprListExp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgExpressionPtrList::const_iterator source_expressions_iterator = p_expressions.begin(); source_expressions_iterator != p_expressions.end(); ++source_expressions_iterator) 
             { 
               if ( *source_expressions_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7537 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgVarRefExp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgVarRefExp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_symbol == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7558 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgClassNameRefExp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgClassNameRefExp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_symbol == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7578 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgFunctionRefExp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgFunctionRefExp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_symbol_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_function_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7600 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgMemberFunctionRefExp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgMemberFunctionRefExp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_symbol_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_function_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7621 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgValueExp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgValueExp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7641 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgBoolValExp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgBoolValExp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7661 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgStringVal::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgStringVal -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7681 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgShortVal::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgShortVal -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7701 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgCharVal::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgCharVal -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7721 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUnsignedCharVal::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUnsignedCharVal -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7741 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgWcharVal::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgWcharVal -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7761 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUnsignedShortVal::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUnsignedShortVal -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7781 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgIntVal::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgIntVal -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7801 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgEnumVal::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgEnumVal -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_declaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7822 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUnsignedIntVal::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUnsignedIntVal -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7842 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgLongIntVal::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgLongIntVal -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7862 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgLongLongIntVal::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgLongLongIntVal -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7882 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUnsignedLongLongIntVal::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUnsignedLongLongIntVal -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7902 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUnsignedLongVal::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUnsignedLongVal -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7922 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgFloatVal::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgFloatVal -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7942 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgDoubleVal::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgDoubleVal -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7962 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgLongDoubleVal::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgLongDoubleVal -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 7982 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgComplexVal::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgComplexVal -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_real_value == childNode ) { return indexCounter; } indexCounter++;
     if ( p_imaginary_value == childNode ) { return indexCounter; } indexCounter++;
     if ( p_precisionType == childNode ) { return indexCounter; } indexCounter++;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8005 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUpcThreads::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUpcThreads -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8025 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUpcMythread::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUpcMythread -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_originalExpressionTree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8045 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgFunctionCallExp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgFunctionCallExp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_function == childNode ) { return indexCounter; } indexCounter++;
     if ( p_args == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8067 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgSizeOfOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgSizeOfOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operand_expr == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operand_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8089 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUpcLocalsizeofExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUpcLocalsizeofExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_expression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operand_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8111 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUpcBlocksizeofExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUpcBlocksizeofExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_expression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operand_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8133 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUpcElemsizeofExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUpcElemsizeofExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_expression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operand_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8155 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypeIdOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypeIdOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operand_expr == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operand_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8177 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgConditionalExp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgConditionalExp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_conditional_exp == childNode ) { return indexCounter; } indexCounter++;
     if ( p_true_exp == childNode ) { return indexCounter; } indexCounter++;
     if ( p_false_exp == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8200 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgNewExp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgNewExp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_specified_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_placement_args == childNode ) { return indexCounter; } indexCounter++;
     if ( p_constructor_args == childNode ) { return indexCounter; } indexCounter++;
     if ( p_builtin_args == childNode ) { return indexCounter; } indexCounter++;
     if ( p_newOperatorDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8224 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgDeleteExp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgDeleteExp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_variable == childNode ) { return indexCounter; } indexCounter++;
     if ( p_deleteOperatorDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8245 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgThisExp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgThisExp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_class_symbol == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8265 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgRefExp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgRefExp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_type_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8285 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgInitializer::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgInitializer -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8304 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAggregateInitializer::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAggregateInitializer -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_initializers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8325 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgConstructorInitializer::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgConstructorInitializer -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_declaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_args == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8347 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAssignInitializer::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAssignInitializer -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operand_i == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8368 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgDesignatedInitializer::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgDesignatedInitializer -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_designatorList == childNode ) { return indexCounter; } indexCounter++;
     if ( p_memberInit == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8389 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgVarArgStartOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgVarArgStartOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8411 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgVarArgOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgVarArgOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operand_expr == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8432 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgVarArgEndOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgVarArgEndOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operand_expr == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8453 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgVarArgCopyOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgVarArgCopyOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs_operand == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs_operand == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8475 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgVarArgStartOneOperandOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgVarArgStartOneOperandOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operand_expr == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8496 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgNullExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgNullExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8515 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgVariantExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgVariantExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8534 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgSubscriptExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgSubscriptExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lowerBound == childNode ) { return indexCounter; } indexCounter++;
     if ( p_upperBound == childNode ) { return indexCounter; } indexCounter++;
     if ( p_stride == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8556 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgColonShapeExp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgColonShapeExp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8575 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsteriskShapeExp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsteriskShapeExp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8594 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgImpliedDo::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgImpliedDo -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_do_var_initialization == childNode ) { return indexCounter; } indexCounter++;
     if ( p_last_val == childNode ) { return indexCounter; } indexCounter++;
     if ( p_increment == childNode ) { return indexCounter; } indexCounter++;
     if ( p_object_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_implied_do_scope == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8618 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgIOItemExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgIOItemExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_io_item == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8638 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgStatementExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgStatementExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_statement == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8658 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmOp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmOp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_expression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8678 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgLabelRefExp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgLabelRefExp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_symbol == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8698 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgActualArgumentExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgActualArgumentExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_expression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8718 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgUnknownArrayOrFunctionReference::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgUnknownArrayOrFunctionReference -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_named_reference == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_list == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8739 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgPseudoDestructorRefExp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgPseudoDestructorRefExp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_object_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_expression_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8760 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgCAFCoExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgCAFCoExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_teamId == childNode ) { return indexCounter; } indexCounter++;
     if ( p_teamRank == childNode ) { return indexCounter; } indexCounter++;
     if ( p_referData == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8782 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgCudaKernelCallExp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgCudaKernelCallExp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_function == childNode ) { return indexCounter; } indexCounter++;
     if ( p_args == childNode ) { return indexCounter; } indexCounter++;
     if ( p_exec_config == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8804 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgCudaKernelExecConfig::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgCudaKernelExecConfig -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_grid == childNode ) { return indexCounter; } indexCounter++;
     if ( p_blocks == childNode ) { return indexCounter; } indexCounter++;
     if ( p_shared == childNode ) { return indexCounter; } indexCounter++;
     if ( p_stream == childNode ) { return indexCounter; } indexCounter++;
     if ( p_operatorPosition == childNode ) { return indexCounter; } indexCounter++;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8827 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgToken::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgToken -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_startOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_endOfConstruct == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8845 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgSymbol::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgSymbol -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8861 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgVariableSymbol::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgVariableSymbol -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_declaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8878 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgFunctionSymbol::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgFunctionSymbol -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_declaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8895 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgMemberFunctionSymbol::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgMemberFunctionSymbol -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_declaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8912 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgRenameSymbol::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgRenameSymbol -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_original_symbol == childNode ) { return indexCounter; } indexCounter++;
     if ( p_declaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8930 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgFunctionTypeSymbol::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgFunctionTypeSymbol -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8947 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgClassSymbol::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgClassSymbol -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_declaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8964 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTemplateSymbol::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTemplateSymbol -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_declaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8981 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgEnumSymbol::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgEnumSymbol -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_declaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 8998 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgEnumFieldSymbol::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgEnumFieldSymbol -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_declaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9015 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgTypedefSymbol::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgTypedefSymbol -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_declaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9032 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgLabelSymbol::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgLabelSymbol -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_declaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_fortran_statement == childNode ) { return indexCounter; } indexCounter++;
     if ( p_fortran_alternate_return_parameter == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9051 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgDefaultSymbol::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgDefaultSymbol -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9068 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgNamespaceSymbol::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgNamespaceSymbol -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_declaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_aliasDeclaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9086 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgIntrinsicSymbol::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgIntrinsicSymbol -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_declaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9103 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgModuleSymbol::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgModuleSymbol -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_declaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9120 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgInterfaceSymbol::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgInterfaceSymbol -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_declaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9137 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgCommonSymbol::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgCommonSymbol -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_declaration == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9154 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAliasSymbol::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAliasSymbol -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_alias == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9171 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmBinaryAddressSymbol::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmBinaryAddressSymbol -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_address == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9188 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmBinaryDataSymbol::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmBinaryDataSymbol -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_address == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9205 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmNode::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmNode -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9221 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmStatement::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmStatement -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9237 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDeclaration::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDeclaration -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9253 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDataStructureDeclaration::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDataStructureDeclaration -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9269 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmFunctionDeclaration::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmFunctionDeclaration -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmStatementPtrList::const_iterator source_statementList_iterator = p_statementList.begin(); source_statementList_iterator != p_statementList.end(); ++source_statementList_iterator) 
             { 
               if ( *source_statementList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
          for ( SgAsmStatementPtrList::const_iterator source_dest_iterator = p_dest.begin(); source_dest_iterator != p_dest.end(); ++source_dest_iterator) 
             { 
               if ( *source_dest_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_symbol_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9294 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmFieldDeclaration::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmFieldDeclaration -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9310 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmBlock::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmBlock -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmStatementPtrList::const_iterator source_statementList_iterator = p_statementList.begin(); source_statementList_iterator != p_statementList.end(); ++source_statementList_iterator) 
             { 
               if ( *source_statementList_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9330 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmInstruction::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmInstruction -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operandList == childNode ) { return indexCounter; } indexCounter++;
          for ( SgAsmStatementPtrList::const_iterator source_sources_iterator = p_sources.begin(); source_sources_iterator != p_sources.end(); ++source_sources_iterator) 
             { 
               if ( *source_sources_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9351 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmx86Instruction::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmx86Instruction -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operandList == childNode ) { return indexCounter; } indexCounter++;
          for ( SgAsmStatementPtrList::const_iterator source_sources_iterator = p_sources.begin(); source_sources_iterator != p_sources.end(); ++source_sources_iterator) 
             { 
               if ( *source_sources_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9372 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmArmInstruction::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmArmInstruction -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operandList == childNode ) { return indexCounter; } indexCounter++;
          for ( SgAsmStatementPtrList::const_iterator source_sources_iterator = p_sources.begin(); source_sources_iterator != p_sources.end(); ++source_sources_iterator) 
             { 
               if ( *source_sources_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9393 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmPowerpcInstruction::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmPowerpcInstruction -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operandList == childNode ) { return indexCounter; } indexCounter++;
          for ( SgAsmStatementPtrList::const_iterator source_sources_iterator = p_sources.begin(); source_sources_iterator != p_sources.end(); ++source_sources_iterator) 
             { 
               if ( *source_sources_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9414 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9430 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmValueExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmValueExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_unfolded_expression_tree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_symbol == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9448 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmByteValueExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmByteValueExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_unfolded_expression_tree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_symbol == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9466 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmWordValueExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmWordValueExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_unfolded_expression_tree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_symbol == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9484 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDoubleWordValueExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDoubleWordValueExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_unfolded_expression_tree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_symbol == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9502 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmQuadWordValueExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmQuadWordValueExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_unfolded_expression_tree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_symbol == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9520 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmSingleFloatValueExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmSingleFloatValueExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_unfolded_expression_tree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_symbol == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9538 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDoubleFloatValueExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDoubleFloatValueExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_unfolded_expression_tree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_symbol == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9556 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmVectorValueExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmVectorValueExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_unfolded_expression_tree == childNode ) { return indexCounter; } indexCounter++;
     if ( p_symbol == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9575 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmBinaryExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmBinaryExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9593 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmBinaryAdd::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmBinaryAdd -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9611 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmBinarySubtract::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmBinarySubtract -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9629 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmBinaryMultiply::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmBinaryMultiply -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9647 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmBinaryDivide::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmBinaryDivide -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9665 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmBinaryMod::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmBinaryMod -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9683 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmBinaryAddPreupdate::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmBinaryAddPreupdate -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9701 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmBinarySubtractPreupdate::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmBinarySubtractPreupdate -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9719 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmBinaryAddPostupdate::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmBinaryAddPostupdate -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9737 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmBinarySubtractPostupdate::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmBinarySubtractPostupdate -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9755 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmBinaryLsl::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmBinaryLsl -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9773 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmBinaryLsr::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmBinaryLsr -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9791 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmBinaryAsr::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmBinaryAsr -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9809 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmBinaryRor::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmBinaryRor -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_lhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rhs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9827 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmUnaryExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmUnaryExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operand == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9844 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmUnaryPlus::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmUnaryPlus -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operand == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9861 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmUnaryMinus::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmUnaryMinus -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operand == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9878 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmUnaryRrx::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmUnaryRrx -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operand == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9895 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmUnaryArmSpecialRegisterList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmUnaryArmSpecialRegisterList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_operand == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9912 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmMemoryReferenceExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmMemoryReferenceExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_address == childNode ) { return indexCounter; } indexCounter++;
     if ( p_segment == childNode ) { return indexCounter; } indexCounter++;
     if ( p_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9931 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmRegisterReferenceExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmRegisterReferenceExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9948 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmx86RegisterReferenceExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmx86RegisterReferenceExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9965 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmArmRegisterReferenceExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmArmRegisterReferenceExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9982 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmPowerpcRegisterReferenceExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmPowerpcRegisterReferenceExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_type == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 9999 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmControlFlagsExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmControlFlagsExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10015 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmCommonSubExpression::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmCommonSubExpression -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_subexpression == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10032 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmExprListExp::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmExprListExp -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmExpressionPtrList::const_iterator source_expressions_iterator = p_expressions.begin(); source_expressions_iterator != p_expressions.end(); ++source_expressions_iterator) 
             { 
               if ( *source_expressions_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10052 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmInterpretation::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmInterpretation -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_dwarf_info == childNode ) { return indexCounter; } indexCounter++;
     if ( p_headers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_global_block == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10071 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmOperandList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmOperandList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmExpressionPtrList::const_iterator source_operands_iterator = p_operands.begin(); source_operands_iterator != p_operands.end(); ++source_operands_iterator) 
             { 
               if ( *source_operands_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10091 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10107 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmTypeByte::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmTypeByte -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10123 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmTypeWord::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmTypeWord -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10139 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmTypeDoubleWord::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmTypeDoubleWord -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10155 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmTypeQuadWord::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmTypeQuadWord -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10171 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmTypeDoubleQuadWord::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmTypeDoubleQuadWord -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10187 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmType80bitFloat::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmType80bitFloat -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10203 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmType128bitFloat::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmType128bitFloat -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10219 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmTypeSingleFloat::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmTypeSingleFloat -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10235 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmTypeDoubleFloat::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmTypeDoubleFloat -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10251 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmTypeVector::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmTypeVector -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_elementType == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10268 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmExecutableFileFormat::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmExecutableFileFormat -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10284 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmGenericDLL::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmGenericDLL -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10301 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmGenericFormat::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmGenericFormat -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10317 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmGenericDLLList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmGenericDLLList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmGenericDLLPtrList::const_iterator source_dlls_iterator = p_dlls.begin(); source_dlls_iterator != p_dlls.end(); ++source_dlls_iterator) 
             { 
               if ( *source_dlls_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10337 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfEHFrameEntryFD::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfEHFrameEntryFD -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10353 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmGenericFile::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmGenericFile -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_headers == childNode ) { return indexCounter; } indexCounter++;
     if ( p_holes == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10371 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmGenericSection::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmGenericSection -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10390 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmGenericHeader::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmGenericHeader -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_exec_format == childNode ) { return indexCounter; } indexCounter++;
     if ( p_dlls == childNode ) { return indexCounter; } indexCounter++;
     if ( p_sections == childNode ) { return indexCounter; } indexCounter++;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10412 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmPEFileHeader::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmPEFileHeader -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_rvasize_pairs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_section_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_coff_symtab == childNode ) { return indexCounter; } indexCounter++;
     if ( p_exec_format == childNode ) { return indexCounter; } indexCounter++;
     if ( p_dlls == childNode ) { return indexCounter; } indexCounter++;
     if ( p_sections == childNode ) { return indexCounter; } indexCounter++;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10437 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmLEFileHeader::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmLEFileHeader -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_dos2_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_section_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_page_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_resname_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_nonresname_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_entry_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_reloc_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_exec_format == childNode ) { return indexCounter; } indexCounter++;
     if ( p_dlls == childNode ) { return indexCounter; } indexCounter++;
     if ( p_sections == childNode ) { return indexCounter; } indexCounter++;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10466 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmNEFileHeader::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmNEFileHeader -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_dos2_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_section_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_resname_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_nonresname_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_module_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_entry_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_exec_format == childNode ) { return indexCounter; } indexCounter++;
     if ( p_dlls == childNode ) { return indexCounter; } indexCounter++;
     if ( p_sections == childNode ) { return indexCounter; } indexCounter++;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10494 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDOSFileHeader::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDOSFileHeader -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_relocs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_rm_section == childNode ) { return indexCounter; } indexCounter++;
     if ( p_exec_format == childNode ) { return indexCounter; } indexCounter++;
     if ( p_dlls == childNode ) { return indexCounter; } indexCounter++;
     if ( p_sections == childNode ) { return indexCounter; } indexCounter++;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10518 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfFileHeader::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfFileHeader -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_section_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_segment_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_exec_format == childNode ) { return indexCounter; } indexCounter++;
     if ( p_dlls == childNode ) { return indexCounter; } indexCounter++;
     if ( p_sections == childNode ) { return indexCounter; } indexCounter++;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10542 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfSection::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfSection -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_linked_section == childNode ) { return indexCounter; } indexCounter++;
     if ( p_section_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_segment_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10564 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfSymbolSection::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfSymbolSection -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_symbols == childNode ) { return indexCounter; } indexCounter++;
     if ( p_linked_section == childNode ) { return indexCounter; } indexCounter++;
     if ( p_section_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_segment_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10587 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfRelocSection::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfRelocSection -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_target_section == childNode ) { return indexCounter; } indexCounter++;
     if ( p_entries == childNode ) { return indexCounter; } indexCounter++;
     if ( p_linked_section == childNode ) { return indexCounter; } indexCounter++;
     if ( p_section_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_segment_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10611 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfDynamicSection::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfDynamicSection -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_entries == childNode ) { return indexCounter; } indexCounter++;
     if ( p_linked_section == childNode ) { return indexCounter; } indexCounter++;
     if ( p_section_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_segment_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10634 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfStringSection::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfStringSection -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_strtab == childNode ) { return indexCounter; } indexCounter++;
     if ( p_linked_section == childNode ) { return indexCounter; } indexCounter++;
     if ( p_section_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_segment_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10657 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfNoteSection::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfNoteSection -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_entries == childNode ) { return indexCounter; } indexCounter++;
     if ( p_linked_section == childNode ) { return indexCounter; } indexCounter++;
     if ( p_section_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_segment_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10680 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfEHFrameSection::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfEHFrameSection -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ci_entries == childNode ) { return indexCounter; } indexCounter++;
     if ( p_linked_section == childNode ) { return indexCounter; } indexCounter++;
     if ( p_section_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_segment_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10703 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfSymverSection::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfSymverSection -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_entries == childNode ) { return indexCounter; } indexCounter++;
     if ( p_linked_section == childNode ) { return indexCounter; } indexCounter++;
     if ( p_section_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_segment_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10726 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfSymverDefinedSection::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfSymverDefinedSection -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_entries == childNode ) { return indexCounter; } indexCounter++;
     if ( p_linked_section == childNode ) { return indexCounter; } indexCounter++;
     if ( p_section_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_segment_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10749 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfSymverNeededSection::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfSymverNeededSection -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_entries == childNode ) { return indexCounter; } indexCounter++;
     if ( p_linked_section == childNode ) { return indexCounter; } indexCounter++;
     if ( p_section_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_segment_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10772 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfSectionTable::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfSectionTable -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10791 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfSegmentTable::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfSegmentTable -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10810 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmPESection::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmPESection -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_section_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10830 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmPEImportSection::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmPEImportSection -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_import_directories == childNode ) { return indexCounter; } indexCounter++;
     if ( p_section_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10851 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmPEExportSection::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmPEExportSection -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_export_dir == childNode ) { return indexCounter; } indexCounter++;
     if ( p_exports == childNode ) { return indexCounter; } indexCounter++;
     if ( p_section_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10873 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmPEStringSection::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmPEStringSection -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_strtab == childNode ) { return indexCounter; } indexCounter++;
     if ( p_section_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10894 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmPESectionTable::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmPESectionTable -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10913 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDOSExtendedHeader::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDOSExtendedHeader -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10932 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmCoffSymbolTable::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmCoffSymbolTable -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_symbols == childNode ) { return indexCounter; } indexCounter++;
     if ( p_strtab == childNode ) { return indexCounter; } indexCounter++;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10953 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmNESection::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmNESection -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_st_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_reloc_table == childNode ) { return indexCounter; } indexCounter++;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10974 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmNESectionTable::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmNESectionTable -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 10993 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmNENameTable::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmNENameTable -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11012 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmNEModuleTable::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmNEModuleTable -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_strtab == childNode ) { return indexCounter; } indexCounter++;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11032 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmNEStringTable::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmNEStringTable -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11051 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmNEEntryTable::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmNEEntryTable -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmNEEntryPointPtrList::const_iterator source_entries_iterator = p_entries.begin(); source_entries_iterator != p_entries.end(); ++source_entries_iterator) 
             { 
               if ( *source_entries_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11074 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmNERelocTable::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmNERelocTable -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmNERelocEntryPtrList::const_iterator source_entries_iterator = p_entries.begin(); source_entries_iterator != p_entries.end(); ++source_entries_iterator) 
             { 
               if ( *source_entries_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11097 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmLESection::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmLESection -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_st_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11117 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmLESectionTable::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmLESectionTable -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11136 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmLENameTable::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmLENameTable -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11155 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmLEPageTable::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmLEPageTable -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmLEPageTableEntryPtrList::const_iterator source_entries_iterator = p_entries.begin(); source_entries_iterator != p_entries.end(); ++source_entries_iterator) 
             { 
               if ( *source_entries_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11178 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmLEEntryTable::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmLEEntryTable -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmLEEntryPointPtrList::const_iterator source_entries_iterator = p_entries.begin(); source_entries_iterator != p_entries.end(); ++source_entries_iterator) 
             { 
               if ( *source_entries_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11201 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmLERelocTable::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmLERelocTable -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmLERelocEntryPtrList::const_iterator source_entries_iterator = p_entries.begin(); source_entries_iterator != p_entries.end(); ++source_entries_iterator) 
             { 
               if ( *source_entries_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_file == childNode ) { return indexCounter; } indexCounter++;
     if ( p_header == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11224 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmGenericSymbol::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmGenericSymbol -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_bound == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11242 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmCoffSymbol::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmCoffSymbol -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_bound == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11260 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfSymbol::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfSymbol -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_bound == childNode ) { return indexCounter; } indexCounter++;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11278 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmGenericStrtab::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmGenericStrtab -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_container == childNode ) { return indexCounter; } indexCounter++;
     if ( p_dont_free == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11296 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfStrtab::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfStrtab -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_container == childNode ) { return indexCounter; } indexCounter++;
     if ( p_dont_free == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11314 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmCoffStrtab::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmCoffStrtab -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_container == childNode ) { return indexCounter; } indexCounter++;
     if ( p_dont_free == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11332 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmGenericSymbolList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmGenericSymbolList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmGenericSymbolPtrList::const_iterator source_symbols_iterator = p_symbols.begin(); source_symbols_iterator != p_symbols.end(); ++source_symbols_iterator) 
             { 
               if ( *source_symbols_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11352 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmGenericSectionList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmGenericSectionList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmGenericSectionPtrList::const_iterator source_sections_iterator = p_sections.begin(); source_sections_iterator != p_sections.end(); ++source_sections_iterator) 
             { 
               if ( *source_sections_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11372 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmGenericHeaderList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmGenericHeaderList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmGenericHeaderPtrList::const_iterator source_headers_iterator = p_headers.begin(); source_headers_iterator != p_headers.end(); ++source_headers_iterator) 
             { 
               if ( *source_headers_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11392 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmGenericString::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmGenericString -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11408 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmBasicString::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmBasicString -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11424 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmStoredString::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmStoredString -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_storage == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11441 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfSectionTableEntry::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfSectionTableEntry -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11457 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfSegmentTableEntry::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfSegmentTableEntry -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11473 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfSymbolList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfSymbolList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmElfSymbolPtrList::const_iterator source_symbols_iterator = p_symbols.begin(); source_symbols_iterator != p_symbols.end(); ++source_symbols_iterator) 
             { 
               if ( *source_symbols_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11493 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmPEImportILTEntry::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmPEImportILTEntry -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_hnt_entry == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11510 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfRelocEntry::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfRelocEntry -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11526 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfRelocEntryList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfRelocEntryList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmElfRelocEntryPtrList::const_iterator source_entries_iterator = p_entries.begin(); source_entries_iterator != p_entries.end(); ++source_entries_iterator) 
             { 
               if ( *source_entries_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11546 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmPEExportEntry::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmPEExportEntry -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_forwarder == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11564 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmPEExportEntryList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmPEExportEntryList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmPEExportEntryPtrList::const_iterator source_exports_iterator = p_exports.begin(); source_exports_iterator != p_exports.end(); ++source_exports_iterator) 
             { 
               if ( *source_exports_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11584 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfDynamicEntry::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfDynamicEntry -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11601 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfDynamicEntryList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfDynamicEntryList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmElfDynamicEntryPtrList::const_iterator source_entries_iterator = p_entries.begin(); source_entries_iterator != p_entries.end(); ++source_entries_iterator) 
             { 
               if ( *source_entries_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11621 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfSegmentTableEntryList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfSegmentTableEntryList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmElfSegmentTableEntryPtrList::const_iterator source_entries_iterator = p_entries.begin(); source_entries_iterator != p_entries.end(); ++source_entries_iterator) 
             { 
               if ( *source_entries_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11641 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmStringStorage::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmStringStorage -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_strtab == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11658 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfNoteEntry::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfNoteEntry -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11675 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfNoteEntryList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfNoteEntryList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmElfNoteEntryPtrList::const_iterator source_entries_iterator = p_entries.begin(); source_entries_iterator != p_entries.end(); ++source_entries_iterator) 
             { 
               if ( *source_entries_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11695 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfSymverEntry::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfSymverEntry -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11711 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfSymverEntryList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfSymverEntryList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmElfSymverEntryPtrList::const_iterator source_entries_iterator = p_entries.begin(); source_entries_iterator != p_entries.end(); ++source_entries_iterator) 
             { 
               if ( *source_entries_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11731 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfSymverDefinedEntry::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfSymverDefinedEntry -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_entries == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11748 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfSymverDefinedEntryList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfSymverDefinedEntryList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmElfSymverDefinedEntryPtrList::const_iterator source_entries_iterator = p_entries.begin(); source_entries_iterator != p_entries.end(); ++source_entries_iterator) 
             { 
               if ( *source_entries_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11768 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfSymverDefinedAux::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfSymverDefinedAux -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11785 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfSymverDefinedAuxList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfSymverDefinedAuxList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmElfSymverDefinedAuxPtrList::const_iterator source_entries_iterator = p_entries.begin(); source_entries_iterator != p_entries.end(); ++source_entries_iterator) 
             { 
               if ( *source_entries_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11805 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfSymverNeededEntry::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfSymverNeededEntry -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_file_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_entries == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11823 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfSymverNeededEntryList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfSymverNeededEntryList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmElfSymverNeededEntryPtrList::const_iterator source_entries_iterator = p_entries.begin(); source_entries_iterator != p_entries.end(); ++source_entries_iterator) 
             { 
               if ( *source_entries_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11843 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfSymverNeededAux::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfSymverNeededAux -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11860 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfSymverNeededAuxList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfSymverNeededAuxList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmElfSymverNeededAuxPtrList::const_iterator source_entries_iterator = p_entries.begin(); source_entries_iterator != p_entries.end(); ++source_entries_iterator) 
             { 
               if ( *source_entries_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11880 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmPEImportDirectory::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmPEImportDirectory -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_ilt == childNode ) { return indexCounter; } indexCounter++;
     if ( p_dll_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_iat == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11899 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmPEImportHNTEntry::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmPEImportHNTEntry -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11916 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmPESectionTableEntry::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmPESectionTableEntry -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11932 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmPEExportDirectory::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmPEExportDirectory -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_name == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11949 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmPERVASizePair::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmPERVASizePair -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_section == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11966 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmCoffSymbolList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmCoffSymbolList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmCoffSymbolPtrList::const_iterator source_symbols_iterator = p_symbols.begin(); source_symbols_iterator != p_symbols.end(); ++source_symbols_iterator) 
             { 
               if ( *source_symbols_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 11986 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmPERVASizePairList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmPERVASizePairList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmPERVASizePairPtrList::const_iterator source_pairs_iterator = p_pairs.begin(); source_pairs_iterator != p_pairs.end(); ++source_pairs_iterator) 
             { 
               if ( *source_pairs_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12006 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfEHFrameEntryCI::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfEHFrameEntryCI -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_fd_entries == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12023 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmPEImportHNTEntryList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmPEImportHNTEntryList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmPEImportHNTEntryPtrList::const_iterator source_hintnames_iterator = p_hintnames.begin(); source_hintnames_iterator != p_hintnames.end(); ++source_hintnames_iterator) 
             { 
               if ( *source_hintnames_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12043 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmPEImportILTEntryList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmPEImportILTEntryList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmPEImportILTEntryPtrList::const_iterator source_vector_iterator = p_vector.begin(); source_vector_iterator != p_vector.end(); ++source_vector_iterator) 
             { 
               if ( *source_vector_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12063 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmPEImportLookupTable::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmPEImportLookupTable -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_entries == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12080 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmPEImportDirectoryList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmPEImportDirectoryList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmPEImportDirectoryPtrList::const_iterator source_vector_iterator = p_vector.begin(); source_vector_iterator != p_vector.end(); ++source_vector_iterator) 
             { 
               if ( *source_vector_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12100 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmNEEntryPoint::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmNEEntryPoint -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12116 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmNERelocEntry::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmNERelocEntry -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12132 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmNESectionTableEntry::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmNESectionTableEntry -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12148 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfEHFrameEntryCIList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfEHFrameEntryCIList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmElfEHFrameEntryCIPtrList::const_iterator source_entries_iterator = p_entries.begin(); source_entries_iterator != p_entries.end(); ++source_entries_iterator) 
             { 
               if ( *source_entries_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12168 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmLEPageTableEntry::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmLEPageTableEntry -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12184 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmLEEntryPoint::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmLEEntryPoint -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmLEEntryPointPtrList::const_iterator source_entries_iterator = p_entries.begin(); source_entries_iterator != p_entries.end(); ++source_entries_iterator) 
             { 
               if ( *source_entries_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12204 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmLESectionTableEntry::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmLESectionTableEntry -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12220 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmElfEHFrameEntryFDList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmElfEHFrameEntryFDList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmElfEHFrameEntryFDPtrList::const_iterator source_entries_iterator = p_entries.begin(); source_entries_iterator != p_entries.end(); ++source_entries_iterator) 
             { 
               if ( *source_entries_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12240 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfInformation::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfInformation -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12256 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfMacro::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfMacro -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12272 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfMacroList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfMacroList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmDwarfMacroPtrList::const_iterator source_macro_list_iterator = p_macro_list.begin(); source_macro_list_iterator != p_macro_list.end(); ++source_macro_list_iterator) 
             { 
               if ( *source_macro_list_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12292 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfLine::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfLine -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12308 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfLineList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfLineList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmDwarfLinePtrList::const_iterator source_line_list_iterator = p_line_list.begin(); source_line_list_iterator != p_line_list.end(); ++source_line_list_iterator) 
             { 
               if ( *source_line_list_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12328 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfCompilationUnitList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfCompilationUnitList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmDwarfCompilationUnitPtrList::const_iterator source_cu_list_iterator = p_cu_list.begin(); source_cu_list_iterator != p_cu_list.end(); ++source_cu_list_iterator) 
             { 
               if ( *source_cu_list_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12348 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfConstruct::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfConstruct -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12365 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfArrayType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfArrayType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12383 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfClassType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfClassType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12401 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfEntryPoint::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfEntryPoint -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12418 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfEnumerationType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfEnumerationType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12436 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfFormalParameter::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfFormalParameter -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12453 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfImportedDeclaration::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfImportedDeclaration -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12470 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfLabel::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfLabel -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12487 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfLexicalBlock::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfLexicalBlock -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12505 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfMember::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfMember -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12522 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfPointerType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfPointerType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12539 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfReferenceType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfReferenceType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12556 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfCompilationUnit::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfCompilationUnit -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_line_info == childNode ) { return indexCounter; } indexCounter++;
     if ( p_language_constructs == childNode ) { return indexCounter; } indexCounter++;
     if ( p_macro_info == childNode ) { return indexCounter; } indexCounter++;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12576 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfStringType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfStringType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12593 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfStructureType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfStructureType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12611 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfSubroutineType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfSubroutineType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12629 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfTypedef::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfTypedef -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12646 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfUnionType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfUnionType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12664 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfUnspecifiedParameters::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfUnspecifiedParameters -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12681 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfVariant::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfVariant -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12698 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfCommonBlock::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfCommonBlock -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12716 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfCommonInclusion::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfCommonInclusion -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12733 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfInheritance::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfInheritance -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12750 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfInlinedSubroutine::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfInlinedSubroutine -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12768 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfModule::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfModule -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12785 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfPtrToMemberType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfPtrToMemberType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12802 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfSetType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfSetType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12819 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfSubrangeType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfSubrangeType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12836 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfWithStmt::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfWithStmt -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12853 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfAccessDeclaration::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfAccessDeclaration -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12870 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfBaseType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfBaseType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12887 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfCatchBlock::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfCatchBlock -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12904 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfConstType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfConstType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12921 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfConstant::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfConstant -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12938 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfEnumerator::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfEnumerator -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12955 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfFileType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfFileType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12972 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfFriend::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfFriend -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 12989 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfNamelist::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfNamelist -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13006 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfNamelistItem::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfNamelistItem -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13023 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfPackedType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfPackedType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13040 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfSubprogram::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfSubprogram -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13058 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfTemplateTypeParameter::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfTemplateTypeParameter -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13075 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfTemplateValueParameter::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfTemplateValueParameter -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13092 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfThrownType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfThrownType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13109 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfTryBlock::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfTryBlock -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13126 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfVariantPart::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfVariantPart -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13143 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfVariable::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfVariable -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13160 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfVolatileType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfVolatileType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13177 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfDwarfProcedure::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfDwarfProcedure -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13194 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfRestrictType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfRestrictType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13211 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfInterfaceType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfInterfaceType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13228 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfNamespace::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfNamespace -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_body == childNode ) { return indexCounter; } indexCounter++;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13246 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfImportedModule::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfImportedModule -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13263 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfUnspecifiedType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfUnspecifiedType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13280 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfPartialUnit::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfPartialUnit -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13297 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfImportedUnit::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfImportedUnit -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13314 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfMutableType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfMutableType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13331 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfCondition::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfCondition -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13348 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfSharedType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfSharedType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13365 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfFormatLabel::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfFormatLabel -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13382 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfFunctionTemplate::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfFunctionTemplate -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13399 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfClassTemplate::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfClassTemplate -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13416 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfUpcSharedType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfUpcSharedType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13433 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfUpcStrictType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfUpcStrictType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13450 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfUpcRelaxedType::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfUpcRelaxedType -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13467 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfUnknownConstruct::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfUnknownConstruct -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
     if ( p_source_position == childNode ) { return indexCounter; } indexCounter++;
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13484 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmDwarfConstructList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmDwarfConstructList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmDwarfConstructPtrList::const_iterator source_list_iterator = p_list.begin(); source_list_iterator != p_list.end(); ++source_list_iterator) 
             { 
               if ( *source_list_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13504 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmInterpretationList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmInterpretationList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmInterpretationPtrList::const_iterator source_interpretations_iterator = p_interpretations.begin(); source_interpretations_iterator != p_interpretations.end(); ++source_interpretations_iterator) 
             { 
               if ( *source_interpretations_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }

/* #line 13524 "../../../src/frontend/SageIII//Cxx_GrammarGetChildIndex.C" */

/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */

long
SgAsmGenericFileList::getChildIndex( SgNode* childNode ) const
   {
  // ------------ checking pointers of SgAsmGenericFileList -------------------
     ROSE_ASSERT ( p_freepointer == AST_FileIO::IS_VALID_POINTER() );
     int indexCounter = 0;
          for ( SgAsmGenericFilePtrList::const_iterator source_files_iterator = p_files.begin(); source_files_iterator != p_files.end(); ++source_files_iterator) 
             { 
               if ( *source_files_iterator == childNode ) { return indexCounter; } indexCounter++;
             } 
     if ( p_parent == childNode ) { return indexCounter; } indexCounter++;
  // Child not found, return -1 (default value for returnValue) as index position to signal this.
     return -1;
/* #line 8 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarGetChildIndex.macro" */
   }


