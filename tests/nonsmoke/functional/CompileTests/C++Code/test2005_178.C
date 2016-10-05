#if 0
The loop body is already a SgBasicBlock
The loop body is already a SgBasicBlock
At end of prelink loop prelinkIterationCounter = 0
listOfTemplateDeclarationsToOutput.size() = 0
Inside of backend(SgProject*): SgProject::get_verbose() = 0
Inside of backend(SgProject*): project->numberOfFiles() = 1
sourceFilenames.size() = 1
rose_iteratorFail.C:5: warning: all member functions in class `Base' are
   private
rose_iteratorFail.C: In function `void
   visitWithAstNodePointersList(std::vector<Base*, std::allocator<Base*> >)':
rose_iteratorFail.C:25: error: parse error before `;' token

which results in a rose_iteratorBug.C which has one ';' too much, and that
is within the if-statement:
include <vector>
using namespace std;

class Base
{
     private: virtual inline void dummy()
{  }
};

class Derived : public Base {};

void visitWithAstNodePointersList(class std::vector< Base * ,
std::allocator< Base * >  > l)
{
  for (std::vector < Base * , std::allocator< Base * > > ::iterator i =
l.begin(); i!=(l.end()); i++) {
    if (class Derived *dotNode = dynamic_cast < class Derived * > /* part
of cast */ ((*i));) {
    }
    else {
    }
  }
}
#endif

#if 0
#include <vector>
using namespace std;
class Base { virtual void dummy(){}; };
class Derived : public Base { };
void
visitWithAstNodePointersList ( vector<Base*> l )
   {
     for (vector<Base*>::iterator i=l.begin(); i!=l.end(); i++)
          if (Derived* dotNode=dynamic_cast<Derived*>(*i))
             {}
   }
#endif

// This is the simpler case of the bug:
void foo()
   {
     if (int x = 7) 
        {
        }
   }
