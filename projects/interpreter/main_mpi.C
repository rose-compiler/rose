#include <rose.h>
#include <interp_mpi.h>

using namespace std;
using namespace Interp;
using namespace Interp::mpi;

int main(int argc, char **argv)
   {
     Interpretation interp;
     try
        {
          vector<string> argvList(argv, argv+argc);
          interp.parseCommandLine(argvList);

          SgProject *prj = frontend(argvList);
          SgFunctionSymbol *testSym = prjFindGlobalFunction(prj, "test");
          ROSE_ASSERT(testSym != NULL);

          StackFrameP head(new MPIStackFrame(&interp, testSym));
          head->initializeGlobals(prj);
          ValueP x (new IntValue(1, PTemp, head));
          ValueP rv = head->interpFunction(vector<ValueP>(1, x));
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

