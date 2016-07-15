
#include "MintTools.h"

#include "OmpAttribute.h"       //openmp pragmas are defined                                                       

#include "NameGenerator.hh"     //unique function name generator for outlining  

using namespace std;
using namespace rose;
using namespace SageInterface;
using namespace SageBuilder;
using namespace OmpSupport;



static NameGenerator g_copy_param_names ("param_", 0, "_");
static NameGenerator g_outlined_func_names ("mint_", 0, "_");
static NameGenerator g_gridDim_names ("gridDim_", 0, "_");
static NameGenerator g_blockDim_names ("blockDim_", 0, "_");
static NameGenerator g_outlined_arg_names ("mint_", 0, "_");
static NameGenerator g_device_names ("dev_", 0, "_");

//----------------------------------------------------                                                //! Change output file's suffix to .cu 
void MintTools::setOutputFileName(SgFile* cur_file)
{
  ROSE_ASSERT(cur_file);
  string orig_name = cur_file->get_file_info()->get_filenameString();

  string file_suffix = StringUtility::fileNameSuffix(orig_name);

  if (CommandlineProcessing::isCFileNameSuffix(file_suffix))
    {
      orig_name = StringUtility::stripPathFromFileName(orig_name);
      string naked_name = StringUtility::stripFileSuffixFromFileName(orig_name);
      cur_file->set_unparse_output_filename("mint_"+naked_name+".cu");
      //string output_name =  cur_file->get_unparse_output_filename();
      //cout<<"output file name is :"<<output_name<<endl;                                                                                                     
    }
} // setOutputFileName()                                                                                                                                     

//! Hash a string into an unsigned long integer.                                                                                                                           
static unsigned long hashStringToULong (const string& s)
{
  unsigned long sum = 0;
  for (size_t i = 0; i < s.length (); ++i)
    sum += (unsigned long)s[i];
  return sum;
}

SgOmpParallelStatement* MintTools::getEnclosingParallelRegion(SgNode* n,bool includingSelf)
{
  SgOmpParallelStatement* temp = getEnclosingNode<SgOmpParallelStatement>(n,includingSelf);
  if (temp)
    {
      return temp; //MintPragmas::isParallelRegionPragma(isSgNode(temp));
    }
  else
    return NULL;
}

string MintTools::generateCopyParamName(const SgStatement* stmt)
{
  // Generate a prefix.                                                                                                                                                    
  stringstream s;
  s << g_copy_param_names.next ();
  const Sg_File_Info* info = stmt->get_startOfConstruct ();
  ROSE_ASSERT (info);

  //s << hashStringToULong (info->get_raw_filename ()) ;

  return s.str ();
}


string MintTools::generateDeviceVarName(const SgStatement* stmt)
{
  // Generate a prefix.                                                                                                                                                    
  stringstream s;
  s << g_device_names.next ();
  const Sg_File_Info* info = stmt->get_startOfConstruct ();
  ROSE_ASSERT (info);

  //s << hashStringToULong (info->get_raw_filename ()) ;

  return s.str ();
}

string MintTools::generateArgName(const SgStatement* stmt)
{
  // Generate a prefix.                                                                                                                                                    
  stringstream s;
  s << g_outlined_arg_names.next ();
  const Sg_File_Info* info = stmt->get_startOfConstruct ();
  ROSE_ASSERT (info);

  s << hashStringToULong (info->get_raw_filename ()) ;

  return s.str ();
}

string MintTools::generateFuncName(const SgStatement* stmt)
{
  // Generate a prefix.                                                                                                                                                    
  stringstream s;
  s << g_outlined_func_names.next ();
  const Sg_File_Info* info = stmt->get_startOfConstruct ();
  ROSE_ASSERT (info);

  s << hashStringToULong (info->get_raw_filename ()) ;

  return s.str ();
}


string MintTools::generateGridDimName(const SgStatement* stmt)
{
  // Generate a prefix.                                                                                                                                                    
  stringstream s;
  s << g_gridDim_names.next ();
  const Sg_File_Info* info = stmt->get_startOfConstruct ();
  ROSE_ASSERT (info);

  s << hashStringToULong (info->get_raw_filename ()) ;

  return s.str ();
}

string MintTools::generateBlockDimName(const SgStatement* stmt)
{
  // Generate a prefix.                                                                                                                                                    
  stringstream s;
  s << g_blockDim_names.next ();
  const Sg_File_Info* info = stmt->get_startOfConstruct ();
  ROSE_ASSERT (info);

  s << hashStringToULong (info->get_raw_filename ()) ;

  return s.str ();
}



void MintTools::forLoopIndexRanges(SgNode* node, MintInitNameExpMap_t& index_ranges)

{
    ROSE_ASSERT(node != NULL);
    SgOmpForStatement* target = isSgOmpForStatement(node);
    ROSE_ASSERT (target != NULL);
    SgScopeStatement* p_scope = target->get_scope();
    ROSE_ASSERT (p_scope != NULL);
    SgStatement * body =  target->get_body();
    ROSE_ASSERT(body != NULL);

    // The OpenMP syntax requires that the omp for pragma is immediately followed by the for loop.
    SgForStatement * for_loops = isSgForStatement(body);
    ROSE_ASSERT(for_loops);

    //Step1: Find the indices, uppers, lowers and isIncrementals of the loops in a nested loop 
    std::vector<SgForStatement* > loopNest= SageInterface::querySubTree<SgForStatement>(for_loops,V_SgForStatement);
  
    for (std::vector<SgForStatement* > ::iterator i = loopNest.begin(); i!= loopNest.end(); i++)
      {
	SgForStatement* cur_loop = *i;

	SgInitializedName * orig_index = NULL;
	SgExpression* orig_lower = NULL;
	SgExpression* orig_upper= NULL;
	SgExpression* orig_stride= NULL;
	
	// if the loop iteration space is incremental      
	bool isIncremental = true; 
	bool isInclusiveUpperBound = false ;
	// grab the original loop 's controlling information                                  
	// checks if loop index is not being written in the loop body
	bool is_canonical = isCanonicalForLoop (cur_loop, &orig_index, & orig_lower, 
						&orig_upper, &orig_stride, NULL, &isIncremental, &isInclusiveUpperBound);
	ROSE_ASSERT(is_canonical == true);
	
	//upper-lower + 1

	if(isIncremental){

	  SgExpression* raw_range_exp =buildSubtractOp(copyExpression(orig_upper), copyExpression(orig_lower));
	  index_ranges[orig_index] = isInclusiveUpperBound ? buildAddOp(raw_range_exp, buildIntVal(1)) : raw_range_exp ; 
	}
	else {
	  SgExpression* raw_range_exp =buildSubtractOp(copyExpression(orig_lower), copyExpression(orig_upper));
	  index_ranges[orig_index] = isInclusiveUpperBound ? buildAddOp(raw_range_exp, buildIntVal(1)) : raw_range_exp;
	}
	//cout <<"INFO-mint: Processing Loop " << " with index " << orig_index->get_name().str()  << endl;
      }
}




SgInitializedName* MintTools::findInitName(SgNode* node, string var_name)
{

  Rose_STL_Container<SgNode*> varList = NodeQuery::querySubTree(node, V_SgInitializedName);

  Rose_STL_Container<SgNode*>::iterator var;
  
  for(var = varList.begin(); var != varList.end(); var++)
    {
      SgInitializedName* i_name = isSgInitializedName(*var);

      if(var_name == i_name->get_name().str())
	return i_name;
    }

  return NULL;
}

void MintTools::printAllExpressions(SgNode* node)
{

  Rose_STL_Container<SgNode*> expList = NodeQuery::querySubTree(node, V_SgExpression);

  Rose_STL_Container<SgNode*>::iterator exp;
  
  for(exp = expList.begin(); exp != expList.end(); exp++)
    {
      ROSE_ASSERT(*exp);

      SgExpression* curExp = isSgExpression(*exp);

      cout << "Mint: " << curExp->unparseToString() << endl;

    }
}
void MintTools::printAllStatements(SgNode* node)
{

  Rose_STL_Container<SgNode*> sList = NodeQuery::querySubTree(node, V_SgStatement);

  Rose_STL_Container<SgNode*>::iterator s;
  
  for(s = sList.begin(); s != sList.end(); s++)
    {
      ROSE_ASSERT(*s);

      SgStatement* sExp = isSgStatement(*s);

      cout << "Mint: " << sExp->unparseToString() << endl;

    }
}


