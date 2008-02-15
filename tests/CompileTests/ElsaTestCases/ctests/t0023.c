typedef int x;
void f (y)
     register x/*type*/ (y)/*parenthesized declarator*/;
{
}


// hmm... gcc rejects this one, but I don't see why
//  typedef int y;
//  void g (z)
//       register /*implicit-int*/ z/*func-name*/ (y/*parameter type*/)
//  {}
