using System;

public class Fib
{
  static public int bval(bool b)
  {
    if (b) return 1;

    return 0;
  }

  static public int fib(int x)
  {
    //~ not C#
    //~ if (var y = x < 2) return bval(y);

    if (x < 2) return bval(true);

    return fib(x-2) + fib(x-1);
  }

  static public void Main ()
  {
    // Console.WriteLine ("fib(3) = " + fib(3));
  }
}
