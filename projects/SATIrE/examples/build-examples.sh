TESTDIR=executable-examples
BASEDIR=`pwd`

mkdir $TESTDIR
echo "Testing sl1rd"
cp -ur sl1rd $TESTDIR
chmod u+rwx -R $TESTDIR/sl1rd
cd $TESTDIR ; newanalysis sl1rd
cd $BASEDIR
make check -C $TESTDIR/sl1rd

echo "Testing sl2rd"
cp -ur sl2rd $TESTDIR
chmod u+rwx -R $TESTDIR/sl2rd
cd $TESTDIR ; newanalysis sl2rd
cd $BASEDIR
make check -C $TESTDIR/sl2rd

echo "Testing constprop"
cp -ur constprop $TESTDIR
chmod u+rwx -R $TESTDIR/constprop
cd $TESTDIR ; newanalysis constprop 
cd $BASEDIR
make check -C $TESTDIR/constprop

