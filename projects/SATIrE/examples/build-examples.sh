TESTDIR=executable-examples
BASEDIR=`pwd`

mkdir -p $TESTDIR
echo "Testing sl1rd"
cp -ur sl1rd $TESTDIR
chmod u+rwx -R $TESTDIR/sl1rd
cd $TESTDIR ; newanalysis -u sl1rd
cd $BASEDIR
make check -C $TESTDIR/sl1rd

echo "Testing sl2rd"
cp -ur sl2rd $TESTDIR
chmod u+rwx -R $TESTDIR/sl2rd
cd $TESTDIR ; newanalysis -u sl2rd
cd $BASEDIR
make check -C $TESTDIR/sl2rd

echo "Testing constprop"
cp -ur constprop $TESTDIR
chmod u+rwx -R $TESTDIR/constprop
cd $TESTDIR ; newanalysis constprop 
cd $BASEDIR
make check -C $TESTDIR/constprop

echo "Testing nielson99_shape"
cp -ur nielson99_shape $TESTDIR
chmod u+rwx -R $TESTDIR/nielson99_shape
cd $TESTDIR ; newanalysis -u nielson99_shape 
cd $BASEDIR
make check -C $TESTDIR/nielson99_shape CUSTOM_DFI_WRITE='-DDFI_WRITE'

echo "Testing srw98_shape"
cp -ur srw98_shape $TESTDIR
chmod u+rwx -R $TESTDIR/srw98_shape
cd $TESTDIR ; newanalysis -u srw98_shape 
cd $BASEDIR
make check -C $TESTDIR/srw98_shape CUSTOM_DFI_WRITE='-DDFI_WRITE' 

