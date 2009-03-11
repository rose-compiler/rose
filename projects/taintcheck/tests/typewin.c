// This test shows how SDG based IFC improves on
// type-based IFC.  A type based IFC will miss the
// public = 2; line, because it does not track context.

int main() {
  
  int confidential;
  int public;

#pragma leek classified
  confidential = 1;

  if(confidential) {
    public = 42;
  } else {
    public = 22;
  }

  public = 2;

  #pragma leek public
  return public;
}
