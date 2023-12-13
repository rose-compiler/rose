- Obtain classes used in Flang's parse tree

c++ -std=c++17 -E -I $FLANG_HOME/include $FLANG_HOME/include/flang/Parser/parse-tree.h -o parse-tree.h.preproc

-- Then delete everything up to "namespace Fortran::"