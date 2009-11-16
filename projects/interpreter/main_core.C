#include <rose.h>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <interp_core.h>

using namespace std;
using namespace Interp;

int main(int argc, char **argv)
   {
     Interpretation interp;
     try
        {
          vector<string> argvList(argv, argv+argc);
          interp.parseCommandLine(argvList);

          SgProject *prj = frontend(argvList);
          SgFunctionSymbol *testSym = prjFindGlobalFunction(prj, "test");

          StackFrameP head(new StackFrame(&interp, testSym));
          head->initializeGlobals(prj);
          ROSE_ASSERT(testSym != NULL);
          ValueP rv = head->interpFunction(vector<ValueP>(1, ValueP(new IntValue(1, PTemp, head))));
          cout << "Returned " << (rv.get() ? rv->show() : "<<nothing>>") << endl;
        }
     catch (InterpError &ie)
        {
          cerr << "Interpreter error occurred: " << ie.err << endl;
          ie.dumpCallStack(cerr);
          return 1;
        }
     return 0;
   }
