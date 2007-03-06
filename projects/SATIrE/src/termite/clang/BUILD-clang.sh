# See also http://clang.llvm.org/get_started.html

# Checkout and build LLVM from SVN head:
svn co http://llvm.org/svn/llvm-project/llvm/trunk llvm || exit 1
cd llvm
./configure --prefix=/usr/local/mstools || exit 1
make || exit 1

# Checkout clang:
cd tools
svn co http://llvm.org/svn/llvm-project/cfe/trunk clang || exit 1

# Non-mac users: Paths to system header files are currently hard coded
# into clang; as a result, if clang can't find your system headers,
# please follow these instructions:
#
# touch empty.c; gcc -v empty.c -fsyntax-only
#
# to get the path.  Look for the comment "FIXME: temporary hack:
# hard-coded paths" in clang/lib/Driver/InitHeaderSearch.cpp and
# change the lines below to include that path.

# Apply patch
patch -p1 <../../clang-termite.patch || exit 1

#Build clang:
cd clang 
make || exit 1 # (this will give you a debug build)
make install || exit 1
