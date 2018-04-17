#include "AbstractValue.h"

#include "AbstractValue.C"
#include "AType.C"
#define CT_IGNORE_COLORS_BOOLOPTIONS
#include "Miscellaneous.C"
#include <map>
#include <set>

using namespace std;
using namespace CodeThorn;

int main() {
  checkresult=true;
  typedef int VariableId;
  typedef int IndexType;
  typedef CodeThorn::AbstractMemoryLocation<VariableId,IndexType> TestType;
  TestType aml1(1);
  TestType aml2(1,2);
  TestType aml3();
  check("aml1==aml2",!((aml1!=aml2)==false));
  check("aml1!=aml2",(aml1!=aml2));
  check("aml1==aml1",(aml1==aml1));
  check("aml2==aml2",(aml2==aml2));

  check("aml1.isNotEqual(aml2).isTrue()",aml1.isNotEqual(aml2).isTrue());
  check("aml1.isEqual(aml1).isTrue()",aml1.isEqual(aml1).isTrue());
  check("aml1.isEqual(aml1).isTrue()",aml1.isEqual(aml1).isTrue());

  map<TestType,set< AbstractValue* > > test2;
  AbstractValue* aval1=new AbstractAddressValue<TestType>(TestType(1,2));
  set<AbstractValue*> avalset;
  avalset.insert(aval1);
  avalset.insert(aval1);
  cout<<"set-size:"<<avalset.size()<<endl;
  //test2[aml1]=avalset;
  map<TestType,int> t3;
  t3[aml1]=2;

  AbstractValueSurrogate<int,int > aval11();
  AbstractValueSurrogate<int,int> aval12(AbstractNumberValue<int>(1));
  VariableId vid1=1;
  AbstractValueSurrogate<int,int > aval13(AbstractAddressValue<int>(vid1));
  map<TestType,set< AbstractValueSurrogate<int,int> > > test3;

  if(checkresult) {
    cout<<"All tests PASS."<<endl;
    return 0;
  } else {
    cout<<"Some tests FAIL."<<endl;
    return 1;
  }
}
