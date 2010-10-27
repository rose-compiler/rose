#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>
#include <set>
#include <boost/foreach.hpp>
#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"
//#include "RuntimeSystem.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;


void RtedTransformation::insertNamespaceIntoSourceFile( SgProject* project, vector<SgClassDeclaration*>& traverseClasses) {

   //*******************************************
   // for all of the sourcefiles create a namespace at the top of the file
   // add to top of each source file
   vector<SgNode*> resSF = NodeQuery::querySubTree(project,V_SgSourceFile);
   // insert at top of all C files in reverse order
   // only if the class has a constructor and if it is declared in a header file
   vector<SgNode*>::const_iterator resSFIt = resSF.begin();
   for (;resSFIt!=resSF.end();resSFIt++) {
      SgSourceFile* sf = isSgSourceFile(*resSFIt);
      ROSE_ASSERT(sf);
      bool isInSourceFileSet = isInInstrumentedFile(sf);
      if (isInSourceFileSet) {
         // we should only do this for C++!
         std::string filename = sf->get_file_info()->get_filename();
         if ((filename.find(".cxx")!=std::string::npos ||
               filename.find(".cpp")!=std::string::npos ||
               filename.find(".C")!=std::string::npos  ) ) {
            // if it is not a C but C++ program, then insert namespace
            if (RTEDDEBUG()) cerr << " **** Inserting file into sourceFileRoseNamespaceMap:" << sf -> get_file_info() -> get_filename() << endl;
            //if (filename.find("_s.cpp")!=std::string::npos)
            insertNamespaceIntoSourceFile(sf);
         } else {
            //  cerr << " ** not a cpp file" <<filename << endl;
         }
      }
   }
   if (RTEDDEBUG())  cerr << "Deep copy of all C++ class declarations to allow offsetof to be used." << endl;
   vector<SgNode*> results = NodeQuery::querySubTree(project,V_SgClassDeclaration);
   // insert at top of all C files in reverse order
   // only if the class has a constructor and if it is declared in a header file
   // tps (11/06/2009) : it seems that the reverse iterator does not work on MAC OS, so I added another loop to get the reverse vector
   vector<SgNode*>::const_iterator classItR = results.begin();
   vector<SgNode*> resultsInv;
   for (;classItR!=results.end();classItR++) {
      resultsInv.insert(resultsInv.begin(),*classItR);
   }
   ROSE_ASSERT(resultsInv.size()==results.size());
   vector<SgNode*>::const_iterator classIt = resultsInv.begin();
   for (;classIt!=resultsInv.end();classIt++) {
      SgClassDeclaration* classDecl = isSgClassDeclaration(*classIt);
      if (classDecl->get_definingDeclaration()==classDecl)
         if (!classDecl->get_file_info()->isCompilerGenerated()) {
            string filename = classDecl->get_file_info()->get_filenameString();
            size_t idx = filename.rfind('.');
            std::string extension ="";
            if(idx != std::string::npos)
               extension = filename.substr(idx+1);
            if ((extension!="C" && extension!="cpp" && extension!="cxx") &&
                  filename.find("include-staging")==string::npos &&
                  filename.find("/usr/include")==string::npos
            ) {
               std::vector<std::pair<SgNode*,std::string> > vec = classDecl->returnDataMemberPointers();
               if (RTEDDEBUG()) cerr << "\n ** Deep copy: Found classDecl : " << classDecl->get_name().str() << "  in File: " << filename <<
                     "    with number of datamembers: " << vec.size() << "   defining " <<
                     (classDecl->get_definingDeclaration()==classDecl) << endl;
               if (hasPrivateDataMembers(classDecl)) {
                  /*SgClassDeclaration* cdl = */instrumentClassDeclarationIntoTopOfAllSourceFiles(project, classDecl);
                  //traverseClasses.push_back(cdl);
               } //else
               traverseClasses.push_back(classDecl);
            } else if (
                  filename.find("include-staging")==string::npos ||
                  filename.find("/usr/include")==string::npos) {
               std::string classname = classDecl->get_name().str();
            }
         }
   }
   moveupPreprocessingInfo(project);

}

void RtedTransformation::moveupPreprocessingInfo(SgProject* project) {
   vector<SgNode*> results = NodeQuery::querySubTree(project, V_SgSourceFile);
   vector<SgNode*>::const_iterator classIt = results.begin();
   for (; classIt != results.end(); classIt++) {
      SgSourceFile* sf = isSgSourceFile(*classIt);
      ROSE_ASSERT(sf);
      bool isInSourceFileSet = isInInstrumentedFile(sf);
      if (isInSourceFileSet) {
         // if the first located node has the preprocessing info then we are fine
         // otherwise move the info up to the first one
         if (RTEDDEBUG()) cerr << "Moving up preprocessing info for file : "        << sf->get_file_info()->get_filename() << endl;
         vector<SgNode*> nodes = 	NodeQuery::querySubTree(sf, V_SgLocatedNode);
         vector<SgNode*>::const_iterator nodesIT = nodes.begin();
         SgLocatedNode* firstNode = isSgLocatedNode(*nodesIT);
         if (!firstNode)
            continue; // go to next file
         AttachedPreprocessingInfoType* info =	firstNode->getAttachedPreprocessingInfo();
         //cerr << " Found firstNode  -  info = " << info << endl;
         if (info && info->size() > 0)
            continue; // we are done
         // otherwise find located node with info and move up
         for (; nodesIT != nodes.end(); nodesIT++) {
            SgLocatedNode* node = isSgLocatedNode(*nodesIT);
            ROSE_ASSERT(node);
            info = node->getAttachedPreprocessingInfo();
            //if (info!=NULL)
            //  cerr << "     node  -  info = " << info << "   size = " << info->size() << endl;
            if (info && info->size() > 0) {
               //cerr << " firstNode : " << firstNode->class_name() << " surr(firstNode) : " << getSurroundingStatement(firstNode)->class_name() <<
               //  " node : " << node->class_name() << " surr(node) : " << getSurroundingStatement(node)->class_name() << endl;
               SageInterface::moveUpPreprocessingInfo(
                     getSurroundingStatement(firstNode),
                     getSurroundingStatement(node));
            }
         }

      }
   }
}

bool RtedTransformation::hasPrivateDataMembers(SgClassDeclaration* cd_copy) {
   vector<SgNode*> variables = NodeQuery::querySubTree(cd_copy,  V_SgVariableDeclaration);
   vector<SgNode*>::const_iterator varIt = variables.begin();
   for (; varIt != variables.end(); varIt++) {
      SgVariableDeclaration* node = isSgVariableDeclaration(*varIt);
      SgAccessModifier am =  node->get_declarationModifier().get_accessModifier();
      if (am.isPrivate())
         return true;
   }
   return false;
}

void RtedTransformation::insertNamespaceIntoSourceFile(SgSourceFile* sf) {
   cerr << "Building Namespace RTED" << endl;
   // build new namespace
   // the first namespace is used for all forward declarations
   SgNamespaceDeclarationStatement* rosenamesp = buildNamespaceDeclaration(
         "RTED", sf->get_globalScope());
   if (RTEDDEBUG()) cerr << " *** Prepending namespace to sf->globalScope() : " <<  sf->get_globalScope() << endl;

   SageInterface::prependStatement(rosenamesp, sf->get_globalScope());
   ROSE_ASSERT(rosenamesp->get_definition()->get_parent());
   sourceFileRoseNamespaceMap[sf] = rosenamesp; //make_pair(rosenamesp, rosenamesp2);
}

SgClassDeclaration* RtedTransformation::instrumentClassDeclarationIntoTopOfAllSourceFiles(
      SgProject* project, SgClassDeclaration* classDecl) {
   // **********************
   if (RTEDDEBUG()) cerr <<"@@@ instrumenting into top "<< endl;
   // deep copy the classdecl and make it unparseable
   SgClassDeclaration* cd_copy = isSgClassDeclaration(deepCopyNode(classDecl));
   // cout << ">>>>>> Original ClassType :::: " << classDecl->get_type() << endl;
   // cout << ">>>>>> Copied ClassType :::: " << cd_copy->get_type() << endl;
   // SgClassType* type_copy = new SgClassType(cd_copy);
   //cd_copy->set_type(type_copy);
   ROSE_ASSERT(cd_copy);
   vector<SgNode*> nodes2 = NodeQuery::querySubTree(cd_copy, V_SgLocatedNode);
   vector<SgNode*>::const_iterator nodesIT2 = nodes2.begin();
   for (; nodesIT2 != nodes2.end(); nodesIT2++) {
      SgLocatedNode* node = isSgLocatedNode(*nodesIT2);
      ROSE_ASSERT(node);
      Sg_File_Info* file_info = node->get_file_info();
      file_info->setOutputInCodeGeneration();
      //cerr << "copying node : " << node->class_name() << endl;
   }
   if (RTEDDEBUG()) cerr << "deep copy of firstnondefining" << endl;
   SgClassDeclaration* cdn_copy = isSgClassDeclaration(deepCopyNode(
         classDecl->get_firstNondefiningDeclaration()));
   ROSE_ASSERT(cdn_copy);
   vector<SgNode*> nodes = NodeQuery::querySubTree(cdn_copy, V_SgLocatedNode);
   vector<SgNode*>::const_iterator nodesIT = nodes.begin();
   for (; nodesIT != nodes.end(); nodesIT++) {
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

   if (RTEDDEBUG()) {
      cerr << "@@@@@@@@@@@@@@ Original Class classDecl : " << classDecl << " :: " << cd_copy << endl;
      cerr << "@@@@@@@@@@@@@@ Original Class nondefining : " << classDecl->get_firstNondefiningDeclaration()<< " :: " << cdn_copy << endl;
      cerr << "@@@@@@@@@@@@@@@@@@ TYPE OF cd_copy->get_type() : " << cd_copy->get_type() << endl;
      cerr << "@@@@@@@@@@@@@@@@@@ TYPE OF cdn_copy->get_type() : " <<  cdn_copy->get_type() << endl;

      cerr << "@@@@@@@@@@@@@@@@@@ TYPE OF cd_copy->get_type()->declaration : " <<cd_copy->get_type()->get_declaration() << endl;
      cerr << "@@@@@@@@@@@@@@@@@@ TYPE OF cd_copy->definingDeclaration : " <<	  cd_copy->get_definingDeclaration() << endl;
      cerr << "@@@@@@@@@@@@@@@@@@ TYPE OF cd_copy->set_firstNondefiningDeclaration : " <<	  cd_copy->get_firstNondefiningDeclaration() << endl;
   }
   // **********************
   // add to top of each source file
   vector<SgNode*> results = NodeQuery::querySubTree(project, V_SgSourceFile);
   // insert at top of all source files in reverse order
   // only if the class has private members and if it is declared in a header file
   vector<SgNode*>::const_iterator classIt = results.begin();
   for (; classIt != results.end(); classIt++) {
      SgSourceFile* sf = isSgSourceFile(*classIt);
      ROSE_ASSERT(sf);
      bool isInSourceFileSet = isInInstrumentedFile(sf);
      if (isInSourceFileSet) {
         if (RTEDDEBUG()) cerr << "Looking through sourcefile: " << sf -> get_file_info() -> get_filename() << endl;
         // once we have the new class_decl inserted, we remove all functions and the constructor and destructor
         vector<SgNode*> remNodes = NodeQuery::querySubTree(cd_copy,
               V_SgFunctionDeclaration);
         vector<SgNode*>::const_iterator remNodesIt = remNodes.begin();
         for (; remNodesIt != remNodes.end(); remNodesIt++) {
            SgFunctionDeclaration* node = isSgFunctionDeclaration(
                  *remNodesIt);
            ROSE_ASSERT(node);
            SgStatement* stmt = getSurroundingStatement(node);
            SageInterface::removeStatement(stmt);
         }

         if (RTEDDEBUG()) cerr << "  changing privates to public" << endl;
         // change each private: to public:
         SgClassDefinition* cd_def = cd_copy->get_definition();
         ROSE_ASSERT(cd_def);
         Rose_STL_Container<SgDeclarationStatement*> decls = cd_def->get_members();
         Rose_STL_Container<SgDeclarationStatement*>::const_iterator itDecls = decls.begin();
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
         if (RTEDDEBUG()) cerr << "Finding Namespace RTED  "  <<  endl;
         //typedef std::pair < SgNamespaceDeclarationStatement*,
         //  SgNamespaceDeclarationStatement* > Namesp;
         SourceFileRoseNMType::const_iterator pit = sourceFileRoseNamespaceMap.find(sf);
         if (pit!=sourceFileRoseNamespaceMap.end()) {
            //Namesp p = pit->second;
            //SgNamespaceDeclarationStatement* firstNamespace = p.second;
            //SgNamespaceDeclarationStatement* secondNamespace = p.first;
            SgNamespaceDeclarationStatement* firstNamespace = pit->second;

            // insert at top of file - after includes
            if (RTEDDEBUG()) cerr << " Prepending to source file: " << sf -> get_file_info() -> get_filename() <<
                  "   class : " << cd_copy->get_name().str() << endl;
            // we prepend it to the top of the file and later move the include back up
            //SageInterface::prependStatement(cd_copy, secondNamespace->get_definition() );
            //SageInterface::prependStatement(cdn_copy,firstNamespace->get_definition());
            SageInterface::prependStatement(cdn_copy,firstNamespace->get_definition());
            SageInterface::appendStatement(cd_copy, firstNamespace->get_definition() );
            ROSE_ASSERT(cdn_copy->get_symbol_from_symbol_table() != NULL);

            classesInRTEDNamespace[classDecl->get_definition()] = cd_def;
            //	    classesInRTEDNamespace[cd_def] = cd_def;

         } else {
            cerr << "ERROR ::: No sourceFile in sourceFileNamespaceMap found for :" <<sf-> get_file_info() -> get_filename() <<endl;
            exit(1);
         }
      }
   }
   return cd_copy;
}

#endif

