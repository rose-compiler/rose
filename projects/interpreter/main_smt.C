#include <rose.h>
#include <interp_smt.h>

using namespace std;
using namespace Interp;
using namespace Interp::smtBV;
using namespace smtlib;
using namespace smtlib::QF_BV;

int main(int argc, char **argv)
   {
     SMTInterpretation interp;
     try
        {
          vector<string> argvList(argv, argv+argc);
          interp.parseCommandLine(argvList);

          SgProject *prj = frontend(argvList);
          SgFunctionSymbol *testSym = prjFindGlobalFunction(prj, "test");
          ROSE_ASSERT(testSym != NULL);

          StackFrameP head(new SMTStackFrame(&interp, testSym));
          head->initializeGlobals(prj);
          bvvarP x (new bvvar(Bits32, "x"));
          ValueP xVal (new BVValue(bvbaseP(new bvname(x)), PTemp, head));
          ValueP rv = head->interpFunction(vector<ValueP>(1, xVal));
          cout << "Returned " << (rv.get() ? rv->show() : "<<nothing>>") << endl;
        }
     catch (InterpError &ie)
        {
          cerr << "Interpreter error occurred: " << ie.err << endl;
          ie.dumpCallStack(cerr);
          return 1;
        }
     catch (solveerror &se)
        {
          cerr << "Solver error occurred: " << se.err << endl;
          return 1;
        }
     return 0;
   }
