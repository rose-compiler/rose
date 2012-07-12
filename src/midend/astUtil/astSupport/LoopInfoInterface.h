#ifndef LOOP_INFO_ANALYSIS_H
#define LOOP_INFO_ANALYSIS_H

template <class Stmt, class Interface>
Stmt GetEnclosingLoop( Stmt n, Interface& interface)
{
  while (true) {
    n = interface.GetParent(n);
    if (n == interface.getNULL())
      break;
    if (interface.IsLoop(n))
         break;
  }
  return n;
}

template <class Stmt, class Interface>
int GetLoopLevel( Stmt s, Interface& interface)
{
  int r = 0;
  s = GetEnclosingLoop(s,interface);
  for ( ; s != interface.getNULL(); ++r, s = GetEnclosingLoop(s,interface));
  return r;
}

template <class Stmt, class Interface>
Stmt GetCommonLoop( Interface& interface, Stmt b1, int dim1, 
                        Stmt b2, int dim2, int* level = 0)
{
  while (dim1 > dim2) {
     b1 = GetEnclosingLoop(b1, interface); 
     --dim1;
  }
  while (dim2 > dim1) {
     b2 = GetEnclosingLoop(b2, interface);
    --dim2;
  }
  while (dim1 > 0 && b1 != b2) {
     b1 = GetEnclosingLoop(b1, interface);
     b2 = GetEnclosingLoop(b2, interface);
     if (b1 != b2)
        --dim1;
  }
  if (level != 0)
      *level = dim1;
  if (b1 != b2)
      return interface.getNULL();
  if (b1 != interface.getNULL() && !interface.IsLoop(b1))
     b1 = GetEnclosingLoop(b1, interface);
  return b1;
}

template <class Stmt, class Interface>
Stmt GetCommonLoop( Interface& interface, Stmt b1, Stmt b2, int* level = 0)
{
  int dim1 = GetLoopLevel(b1, interface), dim2 = GetLoopLevel(b2, interface);
  return GetCommonLoop(interface, b1, dim1, b2, dim2, level);
}
#endif
