BASE=`pwd`
pushd /raid0/ebner/all-times/tools/clang
svn diff >$BASE/clang-termite.patch
popd
