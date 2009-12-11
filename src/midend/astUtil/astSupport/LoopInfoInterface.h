#ifndef LOOP_INFO_ANALYSIS_H
#define LOOP_INFO_ANALYSIS_H

// tps (12/09/09) : I believe this is a MS compiler problem: It says : error C2332: 'struct' : missing tag name
// tps (12/09/09) : FIX : Changed the name "interface" to interfaces , as interface is a keyword in MSVC.
template <class Stmt, class Interface>
Stmt GetEnclosingLoop( Stmt n, Interface& interfaces)
{
  while (true) {
    n = interfaces.GetParent(n);
    if (n == interfaces.getNULL())
      break;
    if (interfaces.IsLoop(n))
         break;
  }
  return n;
}


template <class Stmt, class Interface>
int GetLoopLevel( Stmt s, Interface& interfaces)
{
  int r = 0;
  s = GetEnclosingLoop(s,interfaces);
  for ( ; s != interfaces.getNULL(); ++r, s = GetEnclosingLoop(s,interfaces));
  return r;
}

template <class Stmt, class Interface>
Stmt GetCommonLoop( Interface& interfaces, Stmt b1, int dim1, 
                        Stmt b2, int dim2, int* level = 0)
{
  while (dim1 > dim2) {
     b1 = GetEnclosingLoop(b1, interfaces); 
     --dim1;
  }
  while (dim2 > dim1) {
     b2 = GetEnclosingLoop(b2, interfaces);
    --dim2;
  }
  while (dim1 > 0 && b1 != b2) {
     b1 = GetEnclosingLoop(b1, interfaces);
     b2 = GetEnclosingLoop(b2, interfaces);
     if (b1 != b2)
        --dim1;
  }
  if (level != 0)
      *level = dim1;
  if (b1 != b2)
      return interfaces.getNULL();
  if (b1 != interfaces.getNULL() && !interfaces.IsLoop(b1))
     b1 = GetEnclosingLoop(b1, interfaces);
  return b1;
}

template <class Stmt, class Interface>
Stmt GetCommonLoop( Interface& interfaces, Stmt b1, Stmt b2, int* level = 0)
{
  int dim1 = GetLoopLevel(b1, interfaces), dim2 = GetLoopLevel(b2, interfaces);
  return GetCommonLoop(interfaces, b1, dim1, b2, dim2, level);
}
#endif

