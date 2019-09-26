using System;
 
public class VarUse
{
  static public void Main ()
  {
    int i = 0 + 1;
    int j = i + 3 * i;
    int k = i / j + j % i;
    
    int x = i^j;
    int y = (i|j) & j;
    int z = i << (j>>1);
          
    //~ Console.WriteLine (j);
  }
}

