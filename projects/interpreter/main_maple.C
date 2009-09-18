#include <rose.h>
#include <interp_maple.h>

using namespace std;
using namespace MaplePP;
using namespace Interp;
using namespace Interp::maple;

int main(int argc, char **argv)
   {
     try
        {
          Maple maple = vector<string>();
          mpl = &maple;

          vector<string> argvList(argv, argv+argc);
          Interpretation interp;
          interp.parseCommandLine(argvList);

          SgProject *prj = frontend(argvList);
          SgSourceFile *file = isSgSourceFile((*prj)[0]);
          ROSE_ASSERT(file != NULL);
          SgGlobal *global = file->get_globalScope();
          SgFunctionSymbol *testSym = global->lookup_function_symbol("test");

          StackFrameP head(new SymStackFrame(&interp, testSym));
          ValueP x (new AlgebValue(mpl->mkName("x", false), PTemp, head));
       // ValueP x (new AlgebValue(mpl->fromInteger64(1), PTemp, head));
          ValueP rv = head->interpFunction(vector<ValueP>(1, x));
          cout << "Returned " << (rv.get() ? rv->show() : "<<nothing>>") << endl;
          maple.evalStatement("gc():");
        }
     catch (MapleError &me)
        {
          cerr << "Maple error occurred: " << me.err << endl;
          return 1;
        }
     catch (InterpError &ie)
        {
          cerr << "Interpreter error occurred: " << ie.err << endl;
          ie.dumpCallStack(cerr);
          return 1;
        }
   }

