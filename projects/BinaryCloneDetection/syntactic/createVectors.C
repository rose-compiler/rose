/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : Example code on how to use RoseBin
 ****************************************************/

#include <stdio.h>
#include <iostream>
#include "rose.h"
//#include <mysql.h>

#include "sqlite3x.h"

bool addRegistersToOutput;
#include "createCloneDetectionVectors.h"

using namespace std;

/****************************************************
 * main of test program
 ****************************************************/
int main(int argc, char* argv[]) {
  const char* language = "x86";
 
  addRegistersToOutput = true;
  //Connecting to DB using Thomas's stuff


  
  std::vector<std::string> argvList(argv, argv + argc);


  // source code analysis *******************************************************
#if 0
  std::list<string> functionNames;
  SgProject* project=NULL;
  if (argc>1) {
    cerr << " Parsing sources ... " ;
    project = frontend(argc, argv);
    cerr << "  done . " << endl;
    std::vector<SgNode*> functionCallList = NodeQuery:: querySubTree (project, V_SgFunctionDeclaration);
    std::vector<SgNode*>::iterator it = functionCallList.begin();
    for (it;it!=functionCallList.end();++it) {
      SgFunctionDeclaration* funcDef = isSgFunctionDeclaration(*it);
      string name = funcDef->get_name().str();
      if (RoseBin_support::filterName(name))
	continue;
      functionNames.push_back(name);
      cerr << " found function: " << name  << endl;
    }
    }
#endif 
  // binary code analysis *******************************************************
  cerr << " Starting binary analysis ... " << endl;

  ReadOptions options(argc,argv);
  std::cout << "Done reading options" << std::endl;

  ROSE_ASSERT(options.inputFile!="");
  RoseFile* roseBin = new RoseFile( (char*)options.inputFile.c_str() );

  RoseBin_Def::RoseAssemblyLanguage=RoseBin_Def::x86;
  RoseBin_Arch::arch=RoseBin_Arch::bit32;
  RoseBin_OS::os_sys=RoseBin_OS::linux_op;
  RoseBin_OS_VER::os_ver=RoseBin_OS_VER::linux_26;

  cerr << " ASSEMBLY LANGUAGE :: " << RoseBin_Def::RoseAssemblyLanguage << endl;
#if 0
  // allow to filter all but these functions
  roseBin->setFunctionFilter(functionNames);
#endif
  // query the DB to retrieve all data
  SgAsmNode* globalBlock = roseBin->retrieve_DB();

  // traverse the AST and test it
  roseBin->test();

  char* map_db = "mapping";


  runCloneDetection(argvList, globalBlock, CreateCloneDetectionVectors::binary, options );
  return 0;

} 
