#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>
#include <set>

#include "rosez.hpp"

#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

using namespace std;
using namespace SageBuilder;

static
bool hasPrivateDataMembers(SgClassDeclaration* cd_copy)
{
   const SgNodePtrList&          variables = NodeQuery::querySubTree(cd_copy, V_SgVariableDeclaration);
   SgNodePtrList::const_iterator varIt = variables.begin();
   for (; varIt != variables.end(); ++varIt) {
      SgVariableDeclaration* node = isSgVariableDeclaration(*varIt);
      SgAccessModifier am =  node->get_declarationModifier().get_accessModifier();
      if (am.isPrivate())
         return true;
   }
   return false;
}


namespace {
  struct ClassCollector : AstSimpleProcessing
  {
    std::vector<SgClassDeclaration*>& classdecls;

    explicit
    ClassCollector(std::vector<SgClassDeclaration*>& out)
    : AstSimpleProcessing(), classdecls(out)
    {}

    void visit(SgNode* n)
    {
      SgClassDeclaration* classDecl = isSgClassDeclaration(n);

      if (!classDecl) return;

      if (  classDecl->get_definingDeclaration() == classDecl
         && !classDecl->get_file_info()->isCompilerGenerated()
         )
      {
          const string filename = classDecl->get_file_info()->get_filenameString();
          const size_t idx = filename.rfind('.');
          std::string  extension;

          if (idx != std::string::npos)
          {
             extension = filename.substr(idx+1);
          }

          // \note only for classes in header files that are not system includes
          if (  extension != "C"
             && extension != "cpp"
             && extension != "cxx"
             && extension != "cc"
             && filename.find("include-staging") == string::npos
             && filename.find("/usr/include") == string::npos
             )
          {
            classdecls.push_back(classDecl);
          }
      }
    }
  };
}

static
void collectClassesInHeaderFiles(SgProject* project, std::vector<SgClassDeclaration*>& classdecls)
{
  ClassCollector collector(classdecls);

  collector.traverse(project, preorder);
}


void RtedTransformation::insertNamespaceIntoSourceFile( SgProject* project )
{
   //*******************************************
   // for all of the sourcefiles create a namespace at the top of the file
   // add to top of each source file
   // insert at top of all C files in reverse order
   // only if the class has a constructor and if it is declared in a header file
   std::vector<SgSourceFile*>::const_iterator aa = srcfiles.begin();
   std::vector<SgSourceFile*>::const_iterator zz = srcfiles.end();
   for (;aa != zz; ++aa)
   {
      SgSourceFile* sf = *aa;
      bool          isInSourceFileSet = isInInstrumentedFile(sf);

      if (isInSourceFileSet)
      {
         // we should only do this for C++!
         std::string filename = sf->get_file_info()->get_filename();
         if (  filename.find(".cxx") != std::string::npos
            || filename.find(".cpp") != std::string::npos
            || filename.find(".cc")  != std::string::npos
            || filename.find(".C")   != std::string::npos
            )
         {
            // if it is not a C but C++ program, then insert namespace
            if (RTEDDEBUG) cerr << " **** Inserting file into sourceFileRoseNamespaceMap:" << sf -> get_file_info() -> get_filename() << endl;
            //if (filename.find("_s.cpp")!=std::string::npos)
            insertNamespaceIntoSourceFile(sf);
         }
      }
   }

   if (RTEDDEBUG)  cerr << "Deep copy of all C++ class declarations to allow offsetof to be used." << endl;
   std::vector<SgClassDeclaration*> classdecls;

   collectClassesInHeaderFiles(project, classdecls);

   // insert at top of all C files in reverse order
   // only if the class has a constructor and if it is declared in a header file
   std::vector<SgClassDeclaration*>::const_reverse_iterator classaa = classdecls.rbegin();
   std::vector<SgClassDeclaration*>::const_reverse_iterator classzz = classdecls.rend();

   while (classaa != classzz)
   {
     SgClassDeclaration* classDecl = *classaa;

     if (hasPrivateDataMembers(classDecl))
     {
        instrumentClassDeclarationIntoTopOfAllSourceFiles(project, classDecl);
     }

     visit_isClassDefinition(classDecl->get_definition());

     ++classaa;
   }

   // \pp not sure why moveupPreprocessingInfo is needed
   //     commented out for now.
   // moveupPreprocessingInfo(project);
}

/// \brief  starting from n returns the first AST node that is an SgStatement
/// \return n, if n is a SgStatement;
///         otherwise, the first parent of n that is a SgStatement.
static
SgStatement* getStatementLevelNode(SgLocatedNode& n)
{
  SgStatement* stmt = isSgStatement(&n);
  if (stmt) return stmt;

  return &ez::ancestor<SgStatement>(n);
}


void RtedTransformation::moveupPreprocessingInfo(SgProject* project)
{
   std::vector<SgSourceFile*>::const_iterator aa = srcfiles.begin();
   std::vector<SgSourceFile*>::const_iterator zz = srcfiles.end();
   for (; aa != zz; ++aa)
   {
      SgSourceFile* sf = *aa;

      if (!isInInstrumentedFile(sf)) continue;

      // if the first located node has the preprocessing info then we are fine
      // otherwise move the info up to the first one
      if (RTEDDEBUG) cerr << "Moving up preprocessing info for file : "        << sf->get_file_info()->get_filename() << endl;

      // \pp this flattens the entire tree ...
      const SgNodePtrList&            nodes = NodeQuery::querySubTree(sf, V_SgLocatedNode);
      if (nodes.empty()) continue; // go to next file

      SgNodePtrList::const_iterator   nodesIT = nodes.begin();
      SgLocatedNode*                  firstNode = isSgLocatedNode(*nodesIT);
      AttachedPreprocessingInfoType*  info =  firstNode->getAttachedPreprocessingInfo();

      // if the first node already has the preprocessing info we are done
      if (info && info->size() > 0) continue; // we are done

      // otherwise find located node with info and move up
      // \pp \todo insert ++nodesIT b/c the node is already handled anyway
      for (; nodesIT != nodes.end(); ++nodesIT)
      {
        SgLocatedNode* node = isSgLocatedNode(*nodesIT);
        ROSE_ASSERT(node);
        info = node->getAttachedPreprocessingInfo();
        //if (info!=NULL)
        //  cerr << "     node  -  info = " << info << "   size = " << info->size() << endl;
        if (info && info->size() > 0) {
           //cerr << " firstNode : " << firstNode->class_name() << " surr(firstNode) : " << getSurroundingStatement(firstNode)->class_name() <<
           //  " node : " << node->class_name() << " surr(node) : " << getSurroundingStatement(node)->class_name() << endl;
           SageInterface::moveUpPreprocessingInfo(
                 getStatementLevelNode(*firstNode),
                 getStatementLevelNode(*node));
        }
      }
   }
}


void RtedTransformation::insertNamespaceIntoSourceFile(SgSourceFile* sf) {
   cerr << "Building Namespace RTED" << endl;
   // build new namespace
   // the first namespace is used for all forward declarations
   SgNamespaceDeclarationStatement* rosenamesp = buildNamespaceDeclaration(
         "RTED", sf->get_globalScope());
   if (RTEDDEBUG) cerr << " *** Prepending namespace to sf->globalScope() : " <<  sf->get_globalScope() << endl;

   SageInterface::prependStatement(rosenamesp, sf->get_globalScope());
   ROSE_ASSERT(rosenamesp->get_definition()->get_parent());
   sourceFileRoseNamespaceMap[sf] = rosenamesp; //make_pair(rosenamesp, rosenamesp2);
}

SgClassDeclaration* RtedTransformation::instrumentClassDeclarationIntoTopOfAllSourceFiles(
      SgProject* project, SgClassDeclaration* classDecl) {
   // **********************
   if (RTEDDEBUG) cerr <<"@@@ instrumenting into top "<< endl;
   // deep copy the classdecl and make it unparseable
   SgClassDeclaration* cd_copy = SageInterface::deepCopy(classDecl);
   // cout << ">>>>>> Original ClassType :::: " << classDecl->get_type() << endl;
   // cout << ">>>>>> Copied ClassType :::: " << cd_copy->get_type() << endl;
   // SgClassType* type_copy = new SgClassType(cd_copy);
   //cd_copy->set_type(type_copy);
   ROSE_ASSERT(cd_copy);
   const SgNodePtrList&          nodes2 = NodeQuery::querySubTree(cd_copy, V_SgLocatedNode);
   SgNodePtrList::const_iterator nodesIT2 = nodes2.begin();
   for (; nodesIT2 != nodes2.end(); ++nodesIT2) {
      SgLocatedNode* node = isSgLocatedNode(*nodesIT2);
      ROSE_ASSERT(node);
      Sg_File_Info* file_info = node->get_file_info();
      file_info->setOutputInCodeGeneration();
      //cerr << "copying node : " << node->class_name() << endl;
   }
   if (RTEDDEBUG) cerr << "deep copy of firstnondefining" << endl;

   SgClassDeclaration* nondefDecl = isSgClassDeclaration(classDecl->get_firstNondefiningDeclaration());
   SgClassDeclaration* cdn_copy = SageInterface::deepCopy(nondefDecl);
   ROSE_ASSERT(cdn_copy);

   const SgNodePtrList&          nodes = NodeQuery::querySubTree(cdn_copy, V_SgLocatedNode);
   SgNodePtrList::const_iterator nodesIT = nodes.begin();
   for (; nodesIT != nodes.end(); ++nodesIT) {
      SgLocatedNode* node = isSgLocatedNode(*nodesIT);
      ROSE_ASSERT(node);
      Sg_File_Info* file_info = node->get_file_info();
      file_info->setOutputInCodeGeneration();
   }
   cd_copy->set_firstNondefiningDeclaration(cdn_copy);
   SgClassType* cls_type = SgClassType::createType(cdn_copy);
   cls_type->set_declaration(cdn_copy);
   ROSE_ASSERT(cls_type != NULL);
   ROSE_ASSERT (cls_type->get_declaration() == cdn_copy);
   cdn_copy->set_type(cls_type);
   cdn_copy->set_definingDeclaration(cd_copy);
   cd_copy->set_type(cdn_copy->get_type());

   if (RTEDDEBUG) {
      cerr << "@@@@@@@@@@@@@@ Original Class classDecl : " << classDecl << " :: " << cd_copy << endl;
      cerr << "@@@@@@@@@@@@@@ Original Class nondefining : " << classDecl->get_firstNondefiningDeclaration()<< " :: " << cdn_copy << endl;
      cerr << "@@@@@@@@@@@@@@@@@@ TYPE OF cd_copy->get_type() : " << cd_copy->get_type() << endl;
      cerr << "@@@@@@@@@@@@@@@@@@ TYPE OF cdn_copy->get_type() : " <<  cdn_copy->get_type() << endl;

      cerr << "@@@@@@@@@@@@@@@@@@ TYPE OF cd_copy->get_type()->declaration : " <<cd_copy->get_type()->get_declaration() << endl;
      cerr << "@@@@@@@@@@@@@@@@@@ TYPE OF cd_copy->definingDeclaration : " <<   cd_copy->get_definingDeclaration() << endl;
      cerr << "@@@@@@@@@@@@@@@@@@ TYPE OF cd_copy->set_firstNondefiningDeclaration : " <<   cd_copy->get_firstNondefiningDeclaration() << endl;
   }
   // **********************
   // add to top of each source file

   // insert at top of all source files in reverse order
   // only if the class has private members and if it is declared in a header file
   std::vector<SgSourceFile*>::const_iterator aa = srcfiles.begin();
   std::vector<SgSourceFile*>::const_iterator zz = srcfiles.end();
   for (; aa != zz; ++aa)
   {
      SgSourceFile* sf = *aa;
      assert( isInInstrumentedFile(sf) );

      if (RTEDDEBUG) cerr << "Looking through sourcefile: " << sf -> get_file_info() -> get_filename() << endl;
      // once we have the new class_decl inserted, we remove all functions and the constructor and destructor
      const SgNodePtrList&          remNodes = NodeQuery::querySubTree(cd_copy, V_SgFunctionDeclaration);
      SgNodePtrList::const_iterator remNodesIt = remNodes.begin();
      for (; remNodesIt != remNodes.end(); ++remNodesIt) {
        SgFunctionDeclaration* node = isSgFunctionDeclaration(*remNodesIt);
        ROSE_ASSERT(node);
        SageInterface::removeStatement(node);
      }

      if (RTEDDEBUG) cerr << "  changing privates to public" << endl;
      // change each private: to public:
      SgClassDefinition* cd_def = cd_copy->get_definition();
      ROSE_ASSERT(cd_def);
      SgDeclarationStatementPtrList decls = cd_def->get_members();
      SgDeclarationStatementPtrList::const_iterator itDecls = decls.begin();
      for (;itDecls!=decls.end();++itDecls) {
        SgVariableDeclaration* node = isSgVariableDeclaration(*itDecls);
        if (node) {
           SgDeclarationModifier& mod = node->get_declarationModifier();
           SgAccessModifier& am = mod.get_accessModifier();
           if (am.isPrivate() || am.isProtected())
              am.setPublic();
        }
      }

      // get the namespace RTED to put new class into
      if (RTEDDEBUG) cerr << "Finding Namespace RTED  "  <<  endl;

      SourceFileRoseNMType::const_iterator pit = sourceFileRoseNamespaceMap.find(sf);
      ROSE_ASSERT(pit!=sourceFileRoseNamespaceMap.end());

      SgNamespaceDeclarationStatement* firstNamespace = pit->second;

      // insert at top of file - after includes
      if (RTEDDEBUG)
      {
         cerr << " Prepending to source file: " << sf -> get_file_info() -> get_filename()
              << "   class : " << cd_copy->get_name().str()
              << endl;
      }

      // we prepend it to the top of the file and later move the include back up
      SageInterface::prependStatement(cdn_copy,firstNamespace->get_definition());
      SageInterface::appendStatement(cd_copy, firstNamespace->get_definition() );

      ROSE_ASSERT(cdn_copy->get_symbol_from_symbol_table() != NULL);
      classesInRTEDNamespace[classDecl->get_definition()] = cd_def;
   }

   return cd_copy;
}

#endif
