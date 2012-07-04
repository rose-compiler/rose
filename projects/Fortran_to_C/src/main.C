//Fortran to C translation

#include "rose.h"
#include "sageBuilder.h"
#include "f2c/f2cStatement.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace AstFromString;

// phlin 6/20/2011, special function to copy file info of the original SgPragma or Fortran comments.
// This function is duplicated from ROSE/src/frontend/SageIII/ompAstConstruction.cpp 
static bool copyStartFileInfo (SgNode* src, SgNode* dest)
{
  bool result = false;
  ROSE_ASSERT (src && dest);
  // same src and dest, no copy is needed
  if (src == dest) return true;

  SgLocatedNode* lsrc = isSgLocatedNode(src);
  ROSE_ASSERT (lsrc);
  SgLocatedNode* ldest= isSgLocatedNode(dest);
  ROSE_ASSERT (ldest);
  // ROSE_ASSERT (lsrc->get_file_info()->isTransformation() == false);
  // already the same, no copy is needed
  if (lsrc->get_startOfConstruct()->get_filename() == ldest->get_startOfConstruct()->get_filename()
      && lsrc->get_startOfConstruct()->get_line() == ldest->get_startOfConstruct()->get_line()
      && lsrc->get_startOfConstruct()->get_col() == ldest->get_startOfConstruct()->get_col())
    return true; 

  Sg_File_Info* copy = new Sg_File_Info (*(lsrc->get_startOfConstruct())); 
  ROSE_ASSERT (copy != NULL);

   // delete old start of construct
  Sg_File_Info *old_info = ldest->get_startOfConstruct();
  if (old_info) delete (old_info);

  ldest->set_startOfConstruct(copy);
  copy->set_parent(ldest);
//  cout<<"debug: set ldest@"<<ldest <<" with file info @"<< copy <<endl;

  ROSE_ASSERT (lsrc->get_startOfConstruct()->get_filename() == ldest->get_startOfConstruct()->get_filename());
  ROSE_ASSERT (lsrc->get_startOfConstruct()->get_line() == ldest->get_startOfConstruct()->get_line());
  ROSE_ASSERT (lsrc->get_startOfConstruct()->get_col() == ldest->get_startOfConstruct()->get_col());

  ROSE_ASSERT (lsrc->get_startOfConstruct()->get_filename() == ldest->get_file_info()->get_filename());
  ROSE_ASSERT (lsrc->get_startOfConstruct()->get_line() == ldest->get_file_info()->get_line());
  ROSE_ASSERT (lsrc->get_startOfConstruct()->get_col() == ldest->get_file_info()->get_col());

  ROSE_ASSERT (ldest->get_file_info() == copy);
  return result;
}

// Class declaration
class directiveTraversal : public AstSimpleProcessing
{
  public:
    virtual void visit(SgNode* n);
    virtual void parse_directive(SgLocatedNode* locatedNode);
    virtual void convert_Fortran_Comments_to_Pragmas(SgLocatedNode* locatedNode, AttachedPreprocessingInfoType::iterator iter, string directive);
};

void directiveTraversal::convert_Fortran_Comments_to_Pragmas(SgLocatedNode* locatedNode, AttachedPreprocessingInfoType::iterator iter, string directive){
  SgStatement* stmt = (SgStatement*)locatedNode;
  ROSE_ASSERT(stmt != NULL);
  SgScopeStatement * scope = stmt->get_scope();
  ROSE_ASSERT (scope != NULL);
  SgPragmaDeclaration * p_decl;
  PreprocessingInfo* pinfo = *iter;

  p_decl = buildPragmaDeclaration(directive, scope);
  copyStartFileInfo (locatedNode, p_decl);

//cout<<"debug at after addOmpAttribute:"<<stmt <<" " << stmt->getAttachedPreprocessingInfo ()->size() <<endl;
// two cases for where to insert the pragma, depending on where the preprocessing info is attached to stmt
//  1. PreprocessingInfo::before
//     insert the pragma right before the original Fortran statement
//  2. PreprocessingInfo::inside
//      insert it as the last statement within stmt
  PreprocessingInfo::RelativePositionType position = pinfo->getRelativePosition ();      
  if (position == PreprocessingInfo::before)
  { 
    insertStatementBefore (stmt, p_decl, false);
  }
  else if (position == PreprocessingInfo::inside)
  {
    SgScopeStatement* scope = isSgScopeStatement(stmt);
    ROSE_ASSERT (scope != NULL);
    appendStatement(p_decl, scope);
  }
  else if (position == PreprocessingInfo::after)
  {
    insertStatementAfter(stmt, p_decl, false);
  }
  else
  {
    cerr<<"Fortran_directive/src/main.C , illegal PreprocessingInfo::RelativePositionType:"<<position<<endl;
    ROSE_ASSERT (false);
  }
}


void directiveTraversal::parse_directive(SgLocatedNode* locatedNode)
{
  AttachedPreprocessingInfoType* comments;
  AttachedPreprocessingInfoType::iterator iter;

// retrieve all the attached information, including comment, directives, and customized pragma for Fortran.	
  comments = locatedNode->getAttachedPreprocessingInfo();
  if(comments != NULL)
  {
// One locatedNode might have multiple lines of attached information.  We need to print them all.
    iter = comments->begin();
    while (iter != comments->end())
    {
      PreprocessingInfo* pinfo = *iter;
      if(pinfo->getTypeOfDirective() == PreprocessingInfo::FortranStyleComment)
      {
        // retrieve the whole comment string
	string directive = pinfo->getString();
        // transform the comments to lower cases for parsing
	std::transform(directive.begin(), directive.end(), directive.begin(), ::tolower);
 
        c_char = directive.c_str();
        c_sgnode = locatedNode;
        
        if (afs_match_substr("c",false) || afs_match_substr("*",false) || afs_match_substr("!",false))
        {
          if (afs_match_substr("$omp", true))
          {
            // don't change the OpenMP directive.
	    std::cout << "Find OpenMP directive: " << (*iter)->getString() << std::endl;
            iter++;
          }
          else if (afs_match_substr("gcc$", true))
          {
            // don't change the GCC directive.
	    std::cout << "Find GCC directive: " << (*iter)->getString() << std::endl;
            iter++;
          }
          else if (afs_match_substr("dec$", true))
          {
            // only create pragma node for the Intel's vector directive. 
	    std::cout << "Find Intel directive: " << (*iter)->getString() << std::endl;
            if(afs_match_substr("vector always", true))
            {
	      // replace the "vector always" directive into the "simd directive"
              convert_Fortran_Comments_to_Pragmas(locatedNode, iter, "simd");
              // remove the original comment
              comments->erase(iter);
	    }
	    else 
	    {
	      iter++;
	    }
          }
          else if (afs_match_substr("dir$", true))
          {
            // don't change the Cray directive.
	    std::cout << "Find Cray directive: " << (*iter)->getString() << std::endl;
            iter++;
          }
          else if (afs_match_substr("pragma$", true) || afs_match_substr("directive$", true))
          {
            // Create the pragma nodes for the "pragma$" and "directive$" prefix.
	    std::cout << "Find standard directive: " << (*iter)->getString() << std::endl;
            convert_Fortran_Comments_to_Pragmas(locatedNode, iter,c_char);
            // remove the original comment
            comments->erase(iter);
          }
          else
          { iter++; }
        }
      } 
      else 
      {
        if (SgProject::get_verbose() > 0)
        {
          std::cout <<  "Skipping unsupported directive: " << (*iter)->getString() << std::endl;
        }
        iter++;
      }
    }
  }
}



void directiveTraversal::visit(SgNode* n)
{
// On each node look for any comments of Fortran directives
     SgLocatedNode* locatedNode = isSgLocatedNode(n);
     if (locatedNode != NULL)
     {
       parse_directive(locatedNode);
     }

// print out all the variables

     SgScopeStatement* globalScope = isSgScopeStatement(n);
     if (globalScope != NULL)
        {
       // Iterate through the symbol table
          SgSymbolTable::BaseHashType* hashTable = globalScope->get_symbol_table()->get_table();
          SgSymbolTable::hash_iterator i = hashTable->begin();
          while (i != hashTable->end())
             {
               printf ("hash tabel entry: i->first = %s \n",i->first.str());
               i++;
             }
        }

}


class f2cTraversal : public AstSimpleProcessing
{
  public:
    virtual void visit(SgNode* n);
};

void f2cTraversal::visit(SgNode* n)
{
     // Find SgFile node and rename the output file name.
     SgFile* file = isSgFile(n);
     if (file != NULL)
     {
       f2cFile(file);
     }
    
     // Translate Fortran program statmeent
     SgProgramHeaderStatement* ProgramHeaderStatement = isSgProgramHeaderStatement(n);
     if (ProgramHeaderStatement != NULL)
     {
       f2cProgramHeaderStatement(ProgramHeaderStatement);
     }

     // Translate Fortran subroutine statmeent
     SgProcedureHeaderStatement* ProcedureHeaderStatement = isSgProcedureHeaderStatement(n);
     if (ProcedureHeaderStatement != NULL)
     {
       f2cProcedureHeaderStatement(ProcedureHeaderStatement);
     }
}



int main( int argc, char * argv[] )
{
// Build the AST used by ROSE
   SgProject* project = frontend(argc,argv);
// Build the traversal object
   f2cTraversal f2c;
   f2c.traverseInputFiles(project,preorder);

//   directiveTraversal exampleTraversal;
//   exampleTraversal.traverseInputFiles(project,preorder);


   generateDOT(*project);
//   generateAstGraph(project,8000);
   return backend(project);
   //return 0;
}

