namespace CompassAnalyses
{
  int vfork(){ return 1; }

  int foo()
  {
    return vfork();
  }
}
