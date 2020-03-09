/*
 Scalarization transformation for paraDyn

 Pei-Hung Lin (06/29/2018)
 */

#include "rose.h"
#include "rose_config.h" // obtain macros defining backend compiler names, etc.
#include "SgNodeHelper.h"
#include "AstMatching.h"
#include "AstTerm.h"
#include <Sawyer/CommandLine.h>

#include "scalarizer.h"

using namespace std;
using namespace SageInterface;
using namespace scalarizer;


//! [parseCommandLine decl]
Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;
    //! [parseCommandLine decl]

    //! [parseCommandLine standard]
    SwitchGroup standard;
    standard.doc("The following switches are recognized by all tools in this package.");
    //! [parseCommandLine standard]

    //! [parseCommandLine helpversion]
    standard.insert(Switch("help", 'h')
                    .shortName('?')
                    .action(showHelpAndExit(0))
                    .doc("Show this documentation."));

    //! [parseCommandLine helpversion]
    

    //! [parseCommandLine debug]
    SwitchGroup scalarizer;
    scalarizer.doc("The following switches are specific to scalarizer.");

    scalarizer.insert(Switch("debug")
                .intrinsicValue(true, scalarizer::enable_debug)
                .doc("Enable the debugging mode"));

    //! [parseCommandLine parser]
    Parser parser;
    parser
        .purpose("Array scalarization")
        .doc("synopsis",
             "@prop{programName} [@v{switches}] @v{specimen_name}")
        .doc("description",
             "This program performs array scalarization for C and Fortran program "
             "to help GPU performance optimization. ");
    //! [parseCommandLine parser]

    //! [parseCommandLine parse]
    parser.skippingUnknownSwitches(true);
    return parser.with(standard).with(scalarizer).parse(argc, argv).apply();
}
//! [parseCommandLine parse]

//! Check if the current Fortran SgFile has fixed source form
static bool scalarizer::isFixedSourceForm(SgNode* c_sgnode)
{
  bool result = false;
  SgFile * file = SageInterface::getEnclosingFileNode(c_sgnode);
  ROSE_ASSERT(file != NULL);

  // Only make sense for Fortran files
  ROSE_ASSERT (file->get_Fortran_only());
  if (file->get_inputFormat()==SgFile::e_unknown_output_format )
  { // default case: only f77 has fixed form
    if (file->get_F77_only())
      result = true;
    else 
      result = false;
  }
  else // explicit case: any Fortran could be set to fixed form 
  {
    if (file->get_inputFormat()==SgFile::e_fixed_form_output_format)
      result = true;
    else
      result = false;
  }
  return result;
}

static bool scalarizer::is_directive_sentinels(const char* str, SgNode* c_sgnode)
{
  const char* c_char = str;
  bool result = false;
  // two additional case for fixed form
  if (isFixedSourceForm(c_sgnode))
  {
    if (match_substr("c$rose scalarization", c_char) || match_substr("*$rose scalarization", c_char))
      result = true;
  }
  // a common case for all situations
  if (match_substr("!$rose scalarization", c_char))
    result = true;
  return result;
}

static bool scalarizer::match_substr(const char* substr, const char* c_char)
{
  bool result = true;
  const char* old_char = c_char;
  // we skip leading space from the target string

  while ((*c_char)==' '||(*c_char)=='\t')
  {
    c_char++;
  }  
  size_t len =strlen(substr);
  for (size_t i =0; i<len; i++)
  {
    if ((*c_char)==substr[i])
    {
      c_char++;
    }
    else
    {
      result = false;
      c_char = old_char;
      break;
    }
  }
  return result;
}


// Find variable name lsit from the pragma statement. works for Fortran only
vector<string> scalarizer::getFortranTargetnameList(SgNode* root)
{
  vector<string> resultlist;
  ROSE_ASSERT(root);

  // Fortran AST does not support SgPragmaNode.  Need to look for every comment and check if it has valid pragma
  vector<SgLocatedNode*> LocatedNodeList = SageInterface::querySubTree<SgLocatedNode> (root,V_SgLocatedNode);

  for (vector<SgLocatedNode*>::iterator i = LocatedNodeList.begin(); i != LocatedNodeList.end(); i++)
  {
    AttachedPreprocessingInfoType* comments = (*i)->getAttachedPreprocessingInfo();
    if(comments)
    {
      for (AttachedPreprocessingInfoType::iterator j = comments->begin(); j != comments->end(); j++)
      {
        PreprocessingInfo * pinfo = *j;
        if(pinfo->getTypeOfDirective() == PreprocessingInfo::FortranStyleComment)
        {
           string buffer = pinfo->getString();
           if(is_directive_sentinels(buffer.c_str(), *i))
           {
             if(enable_debug)
               cout << "found matched pragma" << endl;
             SgVariableDeclaration* varDeclStmt = isSgVariableDeclaration(*i);
             ROSE_ASSERT(varDeclStmt);
             SgInitializedNamePtrList varList = varDeclStmt->get_variables();

             for(vector<SgInitializedName*>::iterator i=varList.begin(); i<varList.end(); ++i)
             {
               SgVariableSymbol* symbol = isSgVariableSymbol((*i)->search_for_symbol_from_symbol_table());
               ROSE_ASSERT(symbol);
               SgName varname = symbol->get_name();
               resultlist.push_back(varname.getString());
             }
           }
        }
      }
    }
  }
  return resultlist;
}


// Find variable name lsit from the pragma statement. works for C only
//
vector<string> scalarizer::getTargetnameList(SgNode* root)
{
  vector<string> resultlist;
  ROSE_ASSERT(root);

  // Search for SgPragmaDeclaration to find C pragma list
  Rose_STL_Container<SgNode*> pragmaList = NodeQuery::querySubTree(root, V_SgPragmaDeclaration); 
  for(Rose_STL_Container<SgNode*>::iterator it = pragmaList.begin(); it != pragmaList.end(); it++)
  {
    ROSE_ASSERT(*it);
    SgPragmaDeclaration* pragmaStmt = isSgPragmaDeclaration(*it);
    SgPragma* pragma = isSgPragma(pragmaStmt->get_pragma());
    ROSE_ASSERT(pragma); 
    string srcString = pragma->get_pragma();
    if(srcString.compare(0,26,"pragma rose scalarization") == 0)
    {
      if(enable_debug)
        cout << "found pragma" << endl;
      SgVariableDeclaration* varDeclStmt = isSgVariableDeclaration(*it);
      ROSE_ASSERT(varDeclStmt);
      SgInitializedNamePtrList varList = varDeclStmt->get_variables();

      for(vector<SgInitializedName*>::iterator i=varList.begin(); i<varList.end(); ++i)
      {
        SgVariableSymbol* symbol = isSgVariableSymbol((*i)->search_for_symbol_from_symbol_table());
        ROSE_ASSERT(symbol);
        SgName varname = symbol->get_name();
        resultlist.push_back(varname.getString());
      }
    }
  }
  return resultlist;
}

// Change the type of a variable symbol
void scalarizer::transformType(SgVariableSymbol* sym, SgType* newType)
{
  SgInitializedName* initName = sym->get_declaration();
  initName->set_type(newType);
}

// Change the type of a variable symbol
void scalarizer::transformArrayType(SgBasicBlock* funcBody, SgVariableSymbol* sym, SgType* newType)
{
  RoseAst ast(funcBody);
  std::string matchexpression;
  // $ARR[$IDX1,$IDX2]=$RHS
  matchexpression+="$Root=SgPntrArrRefExp($ARR,$IDX1)";
  AstMatching m;
  MatchResult r=m.performMatching(matchexpression,funcBody);
  for(MatchResult::iterator i=r.begin();i!=r.end();++i) {
    if(enable_debug)
      std::cout << "MATCH-LHS: \n"; 
    //SgNode* n=(*i)["X"];
    for(SingleMatchVarBindings::iterator vars_iter=(*i).begin();vars_iter!=(*i).end();++vars_iter) {
      SgNode* matchedTerm=(*vars_iter).second;
      if(enable_debug)
        std::cout << "  VAR: " << (*vars_iter).first << "=" << AstTerm::astTermWithNullValuesToString(matchedTerm) << " @" << matchedTerm << std::endl;
    }
    SgNode* root=(*i)["$Root"];
    SgPntrArrRefExp* arrayRef = isSgPntrArrRefExp(root);
    if((*i)["$ARR"]) {
      SgVarRefExp* lhsVarRef = isSgVarRefExp((*i)["$ARR"]);
      ROSE_ASSERT(lhsVarRef);
      SgVariableSymbol* arrayNameSymbol = lhsVarRef->get_symbol();
      if(arrayNameSymbol==sym) {
        SgNodeHelper::replaceExpression(arrayRef,lhsVarRef,false);
      } else {
        if(enable_debug)
          cout<<"DEBUG: lhs-matches, but symbol does not. skipping."<<arrayNameSymbol->get_name()<<"!="<<sym->get_name()<<endl;
        continue;
      }
    }
  }
}

int main(int argc, char** argv)
{
  Sawyer::CommandLine::ParserResult cmdline = parseCommandLine(argc, argv);
  std::vector<std::string> positionalArgs = cmdline.unreachedArgs();

  // Build the AST used by ROSE
  SgProject* project = frontend(argc, argv);

  isFortran = is_Fortran_language();
  // Run internal consistency tests on AST
  AstTests::runAllTests(project);

  // get variable list
  vector<string> namelist;

  // For each source file in the project
  SgFilePtrList & ptr_list = project->get_fileList();
  for (SgFilePtrList::iterator iter = ptr_list.begin(); iter!=ptr_list.end(); iter++)
  {
    SgFile* sageFile = (*iter);
    SgSourceFile * sfile = isSgSourceFile(sageFile);
    ROSE_ASSERT(sfile);
    //SgGlobal *root = sfile->get_globalScope();

    // find the function list
    Rose_STL_Container<SgNode*> defList = NodeQuery::querySubTree(sfile, V_SgFunctionDefinition); 
    // for each function
    for (Rose_STL_Container<SgNode*>::iterator p = defList.begin(); p != defList.end(); ++p) 
    {
      map<SgVariableSymbol*, int> varRefList;
      
      SgFunctionDefinition *defn = isSgFunctionDefinition(*p);
      ROSE_ASSERT (defn != NULL);

      if(isFortran)
        namelist = getFortranTargetnameList(defn);  
      else
        namelist = getTargetnameList(defn);
     
      SgBasicBlock* funcBody = defn->get_body(); 
      // Process each variable name and perform the transformation
      for(vector<string>::iterator it=namelist.begin(); it != namelist.end(); it++)
      {
        if(enable_debug)
          cout << "Processing name: " << *it << endl;
        SgVariableSymbol* sym = lookupVariableSymbolInParentScopes(*it, funcBody);
        ROSE_ASSERT(sym);
        SgArrayType* symType = isSgArrayType(sym->get_type());
        if(symType == NULL)
        {
          if(enable_debug)
            cout << "Not a variable with array type" << endl;
          break;
        }
        //  Assuming single dimension array only
        SgType* baseType = symType->get_base_type();
        // Change type in variable declartion
        transformType(sym, baseType);

        // Follow the design in typeForge by Markus
        transformArrayType(funcBody, sym, baseType);

      } 
    }
  }


  if(enable_debug)
    generateDOT(*project);

  // Output preprocessed source file.
  int status = backend (project);
  return status;
}

