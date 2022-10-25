int main() {
  int i_var = 1;
  try {
    try {
      throw i_var; // creates new temporary int-variable with i_var's value
    }
    catch(int& i) {
      i = 2; // Does not change i_var
      throw;
    }
    catch(... /* <- SgVariableDeclaration with type SgTypeEllipse, empty init name, and no symbol.*/) {

    }
  }
  catch(int& i2) {
    // i2 is an alias for i
  }
}
