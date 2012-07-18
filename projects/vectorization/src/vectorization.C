#include "vectorization.h"
#include "CommandOptions.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace vectorization;

/******************************************************************************************************************************/
/*
  Add the SIMD header files to the output file.  The header file is chosen based on the translation command. 
*/
/******************************************************************************************************************************/
void vectorization::addHeaderFile(SgProject* project,vector<string>&argvList)
{
  Rose_STL_Container<SgNode*> globalList = NodeQuery::querySubTree (project,V_SgGlobal);
  for(Rose_STL_Container<SgNode*>::iterator i = globalList.begin(); i != globalList.end(); i++)
  {
    SgGlobal* global = isSgGlobal(*i);
    ROSE_ASSERT(global);

    SgFile* file = getEnclosingFileNode(global);
    ROSE_ASSERT(file);

    string filename = file->get_sourceFileNameWithoutPath();
    std::cout << "global in filename: " << filename << std::endl;

    SgScopeStatement* scopeStatement = global->get_scope();
    string headerString;    

    if (CommandlineProcessing::isOption (argvList,"-m","avx",false))
    {
      std::cout << "insert avx header" << std::endl;
      headerString = "avxintrin.h";
    }
    else if (CommandlineProcessing::isOption (argvList,"-m","sse4.2",false))
    {
      std::cout << "insert sse4.2 header" << std::endl;
      headerString = "nmmintrin.h";
    }
    else if (CommandlineProcessing::isOption (argvList,"-m","sse4.1",false))
    {
      std::cout << "insert sse4.1 header" << std::endl;
      headerString = "smmintrin.h";
    }
    else if (CommandlineProcessing::isOption (argvList,"-m","sse4",false))
    {
      std::cout << "insert sse4 header" << std::endl;
      headerString = "tmmintrin.h";
    }
    else
    {
      std::cout << "insert sse3 header" << std::endl;
      headerString = "xmmintrin.h";
    }
    PreprocessingInfo* headerInfo = insertHeader(headerString,PreprocessingInfo::before,true,scopeStatement);
    headerInfo->set_file_info(global->get_file_info());

  }
}


/******************************************************************************************************************************/
/*
*/
/******************************************************************************************************************************/
void vectorization::analyzeLoop(SgForStatement* forStatement)
{
}

/******************************************************************************************************************************/
/*
*/
/******************************************************************************************************************************/
int vectorization::determineVF(SgForStatement* forStatement)
{
  int VF=1;
  return VF;
}

/******************************************************************************************************************************/
/*
*/
/******************************************************************************************************************************/
void vectorization::updateSIMDDeclaration(SgFunctionDeclaration* functionDeclaration)
{
}

/******************************************************************************************************************************/
/*
*/
/******************************************************************************************************************************/
void vectorization::replaceSIMDOperator(SgForStatement* forStatement)
{
}

/******************************************************************************************************************************/
/*
*/
/******************************************************************************************************************************/
void vectorization::fixLoopBound(SgForStatement* forStatement)
{
}

