#include <rose.h>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <interp_core.h>

using namespace std;
using namespace Interp;

int main(int argc, char **argv)
   {
     vector<string> argvList(argv, argv+argc);
     string expectedReturnValue;
     CommandlineProcessing::isOptionWithParameter(argvList, "-interp:", "expectedReturnValue", expectedReturnValue, true);
     try
        {
          Interpretation interp;
          interp.parseCommandLine(argvList);

          SgProject *prj = frontend(argvList);
          SgSourceFile *file = isSgSourceFile((*prj)[0]);
          ROSE_ASSERT(file != NULL);
          SgGlobal *global = file->get_globalScope();
          SgFunctionSymbol *testSym = global->lookup_function_symbol("test");

          StackFrameP head(new StackFrame(&interp, testSym));
          ValueP rv = head->interpFunction(vector<ValueP>(1, ValueP(new IntValue(1, PTemp, head))));
          if (expectedReturnValue != "")
             {
               int rvInt = rv->prim()->getConcreteValueInt();
               stringstream ss;
               ss << rvInt;
               if (ss.str() != expectedReturnValue)
                  {
                    cerr << "Return value expected to be " << expectedReturnValue << ", got " << rv->show() << endl;
                    return 1;
                  }
             }
          cout << "Returned " << (rv.get() ? rv->show() : "<<nothing>>") << endl;
          return 0;
        }
     catch (InterpError &ie)
        {
          cerr << "Interpreter error occurred: " << ie.err << endl;
          ie.dumpCallStack(cerr);
          return 1;
        }
        return 0;
   }
