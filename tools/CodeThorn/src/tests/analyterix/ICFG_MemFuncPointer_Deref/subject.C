
class A {
 public:
  static void mf() {
  
  }
};

int main() {    
  A::mf();     // SgFunctionCallExp::getAssociatedFunctionSymbol(...) finds mf
  (*A::mf)();  // TODO: SgFunctionCallExp::getAssociatedFunctionSymbol(...) does not find mf
}

