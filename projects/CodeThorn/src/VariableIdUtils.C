/*****************************************
 * Author: Sriram Aananthakrishnan, 2013 *
 *****************************************/

#include "sage3basic.h"
#include "VariableIdUtils.h"

std::string CodeThorn::VariableIdSetPrettyPrint::str(VariableIdSet& vset, VariableIdMapping& vidm)
{
  std::ostringstream ostr;
  ostr << "[";
  VariableIdSet::iterator it = vset.begin();
  for( ; it != vset.end(); )
  {
    ostr << "<" << (*it).toString() << ", " << vidm.variableName(*it)  << ">";
    it++;
    if(it != vset.end())
      ostr << ", ";
  }
  ostr << "]";
  return ostr.str();
}

std::string CodeThorn::VariableIdSetPrettyPrint::str(VariableIdSet& vset)
{
  std::ostringstream ostr;
  ostr << "[";
  VariableIdSet::iterator it = vset.begin();
  for( ; it != vset.end(); )
  {
    ostr << (*it).toString();
    it++;
    if(it != vset.end())
      ostr << ", ";
  }
  ostr << "]";
  return ostr.str();
}

void CodeThorn::set_union(const VariableIdSet& set1, const VariableIdSet& set2, VariableIdSet& rset)
{
  VariableIdSet::const_iterator it1 = set1.begin();
  VariableIdSet::const_iterator it2 = set2.begin();
  VariableIdSet::iterator rit = rset.begin();
  
  // // re-implementation of set-union
  while(true)
  {
    if(it1 == set1.end())
    {
      rset.insert(it2, set2.end());
      break;
    }
    if(it2 == set2.end())
    {
      rset.insert(it1, set1.end());
      break;
    }
    
    if(*it1 < *it2)
    {
      rset.insert(rit, *it1); ++it1; ++rit;
    }
    else if(*it2 < *it1)
    {
      rset.insert(rit, *it2); ++it2; ++rit;
    }
    else
    {
      rset.insert(rit, *it1); ++it1; ++it2; ++rit;
    }
  }
}
