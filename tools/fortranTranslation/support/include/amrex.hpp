
#ifndef _FAKE_AMREX_HPP

#define _FAKE_AMREX_HPP 1

namespace amrex
{
  struct Dim { int x, y, z; };

  struct Box;

  int length(const Box&);
  Dim lbound(const Box&);
 
  struct Box {};
  
  struct FArrayBox 
  {
    int view(Dim x);
  };
}

#endif /* _FAKE_AMREX_HPP */
