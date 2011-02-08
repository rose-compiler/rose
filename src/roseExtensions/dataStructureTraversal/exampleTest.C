//#include "GenRepresentation.h"
//using namespace std;
enum controlEnum
{
  BEGGINING_OF_ENUM = 0,
  typeInt = 1,
  typeChar = 2,
  typeTestStruct = 3,
  END_OF_ENUM
};

struct testStruct
{
  int i;
  int j;
  int k;
};

typedef struct io {
        int x;
        int y;
} io_def;

class testClass
{
public:

  testClass ()
  {
    var1 = new foo ();          /*test = NULL; */
    i = 0.0;
  }
  int control_var1;
  bool cond2;
  bool cond3;

  double i;
  int j;
  int k;
  testClass *test;
  testStruct *test2;
  testStruct test3;
  union foo
  {
    //pragma GenUnionControl_xit = (control_var1==typeInt)|(cond2==true)&(cond3==true)
    int xit;
    //pragma GenUnionControl_y = (control_var1==typeChar)
    char y;
    //pragma GenUnionControl_test = (control_var1==typeTestStruct)&(cond2!=true)
    testStruct *test;
  };
  foo *var1;

};

class testClassInh:testClass
{

public:
  testClassInh ()
  {
    i = 0;
  }
  int kuriositet;
  int i;

};
typedef testClass typeTest;
/*typedef struct testTypedef
{
  int x;
}
typeTest2;
*/
int
main (int argc, char *argv[])
{
  /*char kind;
  // typeTest2 *tingle = new typeTest2;
  typeTest* tingle = new typeTest;
 //pragma GenPrintCode_tingle = 

  testStruct *testStructObj = new testStruct ();
  testClass *testClassObj = new testClass ();
  testClassObj->var1->test = new testStruct;
  testClassObj->control_var1 = 3;
  testClassObj->cond2 = true;
  testClassObj->cond3 = true;
 //pragma GenPrintAllVariables =
 //pragma GenPrintCode_testStructObj= 
  //pragma GenPrintCode_testClassObj= 
*/
  return 0;



};
