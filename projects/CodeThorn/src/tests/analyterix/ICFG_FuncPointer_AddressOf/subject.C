
void func(int) {

}

int main() {    
  (&func)(2); // SgAddressOfOp not supported error in SgFunctionCallExp::getAssociatedFunctionSymbol while creating CFG 
}

