#include "sage3basic.h"
#include "sageBuilder.h"
#include "failSafePragma.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

string FailSafe::toString(failsafe_construct_enum fs_type)
{
  string result; 
  switch (fs_type)
  {
     case e_region: result = "region" ; break;
     case e_status_predicate: result = "status"; break;
     case e_data_predicate: result = "data" ; break;
     case e_tolerance: result = "tolerance" ; break;
     case e_double_redundancy: result = "double_redundancy"; break;
     case e_triple_redundancy: result = "triple_redundancy"; break;
     default: 
       cerr<<"Error: unhandled failsafe construct within FailSafe::toString()."<<endl;
       ROSE_ASSERT (false);
       break;
  }
  return result; 
}
