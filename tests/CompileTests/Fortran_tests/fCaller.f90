#if 0
Hi Dan,

now  (500 years later :-)  I am doing the follow up...
Attached is a test example  for the iso_c_binding.
I compile it like this:
gcc -c cCallee.c
gfortran fCaller.f95 cCallee.o

IOW - somehow the ISO_C_BINDING is recognized as a compiler  "internal" module
that I just need to use.
 
With the current Rose  I get:
----------
/disks/utke/Apps/rose_inst/bin/testTranslator -rose:Fortran -rose:skipfinalCompileStep C_Interop/fCaller.f95
/nfs/mcs-homes26/utke/HG_Main/Code/ForTests/C_Interop/fCaller.f95:2.20:

  use iso_c_binding
                   1
Fatal Error: Can't open module file 'iso_c_binding.mod' for reading at (1): No such file or directory
Syntax errors detected in input fortran program ...
---------

The rose doxygen shows the following todo:
http://*www.*rosecompiler.org/ROSE_HTML_Reference/todo.html#_todo000092

Class SgStatement
    Fortran support requires statements in section 15 (modifiers for ISO_C_BINDING).

Please let me know if you need more info.

Jean
#endif

program fCaller
  use iso_c_binding 
  real(kind=C_FLOAT) :: r
  r=3.14
  call cCallee(r)
  print *,r
end program
