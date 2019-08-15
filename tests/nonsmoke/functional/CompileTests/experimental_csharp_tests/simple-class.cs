using System;

public class Fib
{
  static public int fib(int x)
  {
    if (always_true && x < 2) return 1; // bval();

    return fib(x-2) + fib(x-1);
  }

  //~ static public int bval(bool b = always_true)
  //~ {
    //~ if (b) return 1;

    //~ return 0;
  //~ }

  static public void Main ()
  {
    // Console.WriteLine ("fib(3) = " + fib(3));
  }

  const bool always_true = true;
}
