BASE=`pwd`
pushd /nfstmp/adrian/llvm/tools/clang
svn diff >$BASE/clang-termite.patch
popd
