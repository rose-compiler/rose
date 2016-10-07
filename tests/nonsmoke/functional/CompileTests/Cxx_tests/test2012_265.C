typedef void BDInterpFunc();

extern "C" {
  BDInterpFunc fortbd;
  //void fortbd();
}

BDInterpFunc *bdfunc[1];
static void bdfunc_init() {
  bdfunc[0] = fortbd;
}

int main(int argc, char *argv[]) {
  bdfunc_init();
  return 0;
}
