#ifndef LOOP_INFO_ANALYSIS_H
#define LOOP_INFO_ANALYSIS_H

template <class Stmt, class Interface>
Stmt GetEnclosingLoop( Stmt n, Interface& ast_interface)
{
  while (true) {
    n = ast_interface.GetParent(n);
    if (n == ast_interface.getNULL())
      break;
    if (ast_interface.IsLoop(n))
         break;
  }
  return n;
}

template <class Stmt, class Interface>
int GetLoopLevel( Stmt s, Interface& ast_interface)
{
  int r = 0;
  s = GetEnclosingLoop(s,ast_interface);
  for ( ; s != ast_interface.getNULL(); ++r, s = GetEnclosingLoop(s,ast_interface));
  return r;
}

template <class Stmt, class Interface>
Stmt GetCommonLoop( Interface& ast_interface, Stmt b1, int dim1, 
                        Stmt b2, int dim2, int* level = 0)
{
  while (dim1 > dim2) {
     b1 = GetEnclosingLoop(b1, ast_interface); 
     --dim1;
  }
  while (dim2 > dim1) {
     b2 = GetEnclosingLoop(b2, ast_interface);
    --dim2;
  }
  while (dim1 > 0 && b1 != b2) {
     b1 = GetEnclosingLoop(b1, ast_interface);
     b2 = GetEnclosingLoop(b2, ast_interface);
     if (b1 != b2)
        --dim1;
  }
  if (level != 0)
      *level = dim1;
  if (b1 != b2)
      return ast_interface.getNULL();
  if (b1 != ast_interface.getNULL() && !ast_interface.IsLoop(b1))
     b1 = GetEnclosingLoop(b1, ast_interface);
  return b1;
}

template <class Stmt, class Interface>
Stmt GetCommonLoop( Interface& ast_interface, Stmt b1, Stmt b2, int* level = 0)
{
  int dim1 = GetLoopLevel(b1, ast_interface), dim2 = GetLoopLevel(b2, ast_interface);
  return GetCommonLoop(ast_interface, b1, dim1, b2, dim2, level);
}
#endif
