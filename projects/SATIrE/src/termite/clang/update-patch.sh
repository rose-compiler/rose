BASE=`pwd`
pushd /raid0/ebner/satire/clang/llvm/tools/clang
svn diff >$BASE/clang-termite.patch
popd
