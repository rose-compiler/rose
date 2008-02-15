// "and" and "or" keywords

// originally found in package battleball

//  Parse error (state 910) at <name>: and

// ERR-MATCH: Parse error.*at.* and$

int main()
{
    return (true and false) or (false and true);
}

// use all the alternative tokens
void f(int x, int y, int *p)
<%                   // {

  x = p <: 3 :>;     //   x = p[3];

  // skipping %: and %:%: because they are "#" and "##", for cpp only

  x and y;           //   x && y;
  x bitor y;         //   x | y;
  x or y;            //   x || y;
  x xor y;           //   x ^ y;
  compl x;           //   ~ x;
  x bitand y;        //   x & y;
  x and_eq y;        //   x &= y;
  x or_eq y;         //   x |= y;
  x xor_eq y;        //   x ^= y;
  not x;             //   ! x;
  x not_eq y;        //   x != y;

%>                   // }


// EOF
