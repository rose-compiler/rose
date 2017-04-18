#ifndef OFP_TRAVERSAL_HPP
#define OFP_TRAVERSAL_HPP

#include <aterm2.h>
#include "FASTNodes.hpp"

namespace OFP {

class Traversal
{
 public:
   Traversal(FAST::Converter* fast_converter)
     {
       converter = fast_converter;
     }

 protected:
   FAST::Converter* converter;

 public:

ATbool traverse_Program(ATerm term);
ATbool traverse_OptStartCommentBlock(ATerm term, std::string & var_OptStartCommentBlock);
ATbool traverse_ListPlusOfProgramUnit(ATerm term, FAST::Scope* scope);
ATbool traverse_MainProgram(ATerm term, FAST::MainProgram** main_program);
ATbool traverse_OptProgramStmt(ATerm term, FAST::   ProgramStmt** program_stmt);
ATbool traverse_EndProgramStmt(ATerm term, FAST::EndProgramStmt** end_program_stmt);
ATbool traverse_OptLabel(ATerm term, std::string & var_OptLabel);
ATbool traverse_Name(ATerm term, std::string & var_Name);
ATbool traverse_OptProgramName(ATerm term, std::string & var_OptProgramName);
ATbool traverse_eos(ATerm term, std::string & var_eos);
ATbool traverse_InitialSpecPart(ATerm term, FAST::Scope* scope);
ATbool traverse_ListStarOfUseStmt(ATerm term, FAST::Scope* scope);
ATbool traverse_UseStmt(ATerm term, FAST::UseStmt** var_UseStmt);
ATbool traverse_SpecAndExecPart(ATerm term, FAST::Scope* scope);
ATbool traverse_ListStarOfSpecAndExecConstruct(ATerm term, FAST::Scope* scope);
ATbool traverse_OptInternalSubprogramPart(ATerm term, FAST::Scope* scope);

}; // class Traversal
}  // namespace OFP

#endif
