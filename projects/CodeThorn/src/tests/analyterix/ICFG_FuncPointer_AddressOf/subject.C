
void func(int) {

}

int main() {    
  (&func)(2); // Works now (previously there was a "SgAddressOfOp not supported error" in SgFunctionCallExp::getAssociatedFunctionSymbol while creating the CFG)
}

