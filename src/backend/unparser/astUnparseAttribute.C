// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

using namespace std;

// Initialization of static data (used as name in AstAttribute mechanism)
const std::string AstUnparseAttribute::markerName = "AstUnparseAttribute";

void
AstUnparseAttribute::addString(string s, RelativePositionType inputlocation )
   {
  // Add the string location pair to the list.
     stringList.push_back(StringPositionPairType(s,inputlocation));
   }

AstUnparseAttribute::AstUnparseAttribute(string s, RelativePositionType inputlocation )
   {
  // Add the string location pair to the list.
     addString(s,inputlocation);
   }

AstUnparseAttribute::~AstUnparseAttribute()
   {
  // Nothing to do here!
   }

string
AstUnparseAttribute::toString()
   {
     string s;
     for (StringPositionPairListType::iterator i = stringList.begin(); i != stringList.end(); i++)
        {
          s += i->first;
        }

     return s;
   }

string
AstUnparseAttribute::toString( RelativePositionType inputlocation )
   {
     string s;
     for (StringPositionPairListType::iterator i = stringList.begin(); i != stringList.end(); i++)
        {
          if (i->second == inputlocation)
             {
               s += i->first;
             }
        }

     return s;
   }


bool
AstUnparseAttribute::replacementStringExists()
   {
     bool result = false;

  // Search the list for a string location marked as e_replace
     for (StringPositionPairListType::iterator i = stringList.begin(); i != stringList.end(); i++)
        {
          if (i->second == e_replace)
             {
               result = true;
             }
        }

     return result;
   }

AstUnparseAttribute::OwnershipPolicy
AstUnparseAttribute::getOwnershipPolicy() const {
  return CONTAINER_OWNERSHIP;
}
