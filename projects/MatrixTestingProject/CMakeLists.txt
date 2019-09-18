# This project is not intended to be built as part of ROSE. Instead, it serves as an example for how to compile an
# external project that depends on an installed version of ROSE. Therefore, instead of building this project, we should
# install its source code.  Matrix testing depends on this project.
install(FILES
  Makefile
  README
  test-rose.C
  DESTINATION share/test-project)
