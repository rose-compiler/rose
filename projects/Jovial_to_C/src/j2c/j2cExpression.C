#include "j2c.h"
#include "AstInterface.h"
#include "AstInterface_ROSE.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace Jovial_to_C;

extern stack<SgStatement*> insertList;

// NOTE: Used as a placeholder only for now
//
void Jovial_to_C::translateDoubleVal(SgFloatVal* floatVal)
{
  string valString = floatVal->get_valueString();
  if(valString.find('d',0) != string::npos)
  {
    replace(valString.begin(),valString.end(),'d','e');
    SgDoubleVal* doubleVal = buildDoubleVal(atof(valString.c_str()));
    doubleVal->set_valueString(valString);
    replaceExpression(floatVal, doubleVal, false);
  }
  else if(valString.find('D',0) != string::npos)
  {
    replace(valString.begin(),valString.end(),'D','E');
    SgDoubleVal* doubleVal = buildDoubleVal(atof(valString.c_str()));
    doubleVal->set_valueString(valString);
    replaceExpression(floatVal, doubleVal, false);
  }
  else
  {
    valString = valString.append("f");
    floatVal->set_valueString(valString);
  }
}
