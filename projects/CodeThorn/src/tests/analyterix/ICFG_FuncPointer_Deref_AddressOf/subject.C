
void func() {

}

int main() {    
  (*func)();  // SgFunctionCallExp::getAssociatedFunctionSymbol(...) finds func
  (*&func)(); // SgFunctionCallExp::getAssociatedFunctionSymbol(...) finds func now (previously it did not find func because of address-of operator)
}

