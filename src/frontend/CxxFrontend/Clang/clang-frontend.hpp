
#ifndef _CLANG_FRONTEND_HPP_
# define _CLANG_FRONTEND_HPP_

class SgSourceFile;

int clang_main(int argc, char* argv[], SgSourceFile& sageFile);

// DQ (11/1/2020): Adding DOD graph support.
int clang_to_dot_main(int argc, char* argv[]);

#endif /* _CLANG_FRONTEND_HPP_ */

