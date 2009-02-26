#ifndef MACRO_REWRWAPPER_H
#define MACRO_REWRWAPPER_H


#include "macdb/macro.h"

#include <boost/wave.hpp>

#include <boost/regex.hpp>
#include <functional>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "argumentFilterer.h"
#include "linearizeAST.h"
#include "waveHelpers.h"

std::vector<SgNode*> queryForLine(SgNode* node, Sg_File_Info* compareFileInfo);


//A map of macro defs to macro calls to that def
typedef struct { 
                 //The prepocessing info matching the macro call
                 PreprocessingInfo*       macro_call;
                 //A vector of std::string representing the classnames of the
                 //AST constructs matching the macro call
                 std::vector<SgNode*>  comparisonLinearization;

                 //Smallest set of statements containing macro
                 std::vector<SgNode*>  stmtsContainingMacro;

                 //Unparsed string of the stmts or exprs containing the macro
                 //call
                 std::string ASTStringMatchingMacroCall;
               } MappedMacroCall;

//A map of macro defs to macro calls to that def
typedef std::map<PreprocessingInfo*, std::vector<MappedMacroCall> >  map_def_call;


class AnalyzeMacroCalls{
   public:
      //Map over macro calls and their corresponding AST to the macro definition
      map_def_call mapDefsToCalls;

   private:
      SgProject* project;
      //multimap of PreprocessingInfo::CMacroDefs to PreprocessingInfo::CMacroCall
      macro_def_call_type macroDefToCalls;
 
      //internal include paths
      std::list<std::string> internalIncludePathList;
   public:
      AnalyzeMacroCalls(SgProject*);
    
      //get the map of macro definitions to macro calls 
      macro_def_call_type& getMapOfDefsToCalls();
      map_def_call&      getMapDefsToCalls();

      //Will find the smallest stmt matching macro call using positions. If you want the
      //smallest set of stmts or expressions containing the macro call you can find this
      //from this stmt.
      SgNode*
      findSmallestStmtMatchingMacroCallUsingPositions(PreprocessingInfo* currentInfo);

      //Will create a std::vecto<SgNode*> where for each index in the global vector 
      //representing the token stream there is either a corresponding SgNode or NULL.
      std::vector<SgNode*> 
      findMappingOfTokensToAST(SgNode* node, PreprocessingInfo* currentInfo );

      //This function will iterate over all macro calls
      void iterate_over_all_macro_calls(macro_def_call_type& macro_def);

      //This function will add all macro calls to the sqlite database.
      //PS! Database file has to be loaded already
      //Inconsistencies will then be found using the database. The benefit
      //of this is that it is a productive way to do whole program analysis now.
      void add_all_macro_calls_to_db();

      //The function will check for inconsistencies using the information found in
      // iterate_over_all_macro_calls(macro_def_call_type& macro_def);
      std::pair<int,int> check_for_inconsistencies();
};

//This external variable have to be defied somewhere. It will enable very verbose 
//output for debugging purposes
extern bool VERBOSE_MESSAGES_OF_WAVE;

//When checking for inconsistencies in the db this variable will allow us to 
//see if the macro call has already been entered into the db.
extern bool do_db_optimization;



#endif
