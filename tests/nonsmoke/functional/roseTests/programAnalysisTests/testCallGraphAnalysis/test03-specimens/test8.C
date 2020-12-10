
struct A_nodef;  //Forward decl, not actually defined

typedef struct {
  char* aaa;
  int bbb;
} B_struct;

class Z {
public:
  Z() : a(), x() { }

private:

  //No A_nodef is actually constructed here, the pointer is just set to NULL
  //But the CallGraph builder used to segfault because it couldn't find a constuctor.
  A_nodef *a;  
  int x;
};

int main() {
  Z z;
  //This used to cause CallGraphBuilder to crash, because B_struct has an 
  //anonymous parent class with no constructor.  CFB used to look for the
  //parent class constructor an not be able to find it.
  //There's no actual constructor to call here.
  B_struct b_struct = B_struct();

}
