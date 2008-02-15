namespace aCompassAnalyses
{
  int vfork(){ return 1; }

  int foo()
  {
    return vfork();
  }
}
