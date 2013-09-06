      program test5
c
c   This tests the declaration list in Fortran
c   In Fortran AST, the variables from the same declaration
c   line will shared a SgVariableDeclaration.
c   However, C AST doesn't allow it in unparser.
c
      real*4 a1,a2,a3,a4
      real*4 a(4),b(4)
      dimension c(4),d(4)
      end
