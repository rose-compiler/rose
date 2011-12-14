#! /bin/sh

check_error()
{
    if [ $# -gt 2 ]; then
	echo "Error in check_error call";
	exit 1;
    fi;
    ERRCODE="$1";
    if [ "$ERRCODE" = "0" ]; then
	return 0;
    fi;
    if [ $# -eq 2 ]; then
	ERRMESSAGE="$2";
    else
	ERRMESSAGE="Error";
    fi;
    echo "[PolyRose] $ERRMESSAGE";
    exit $ERRCODE;
}


if [ -z "${BOOST_ROOT}" ]; then
	BOOST_ROOT=/usr/local/boost
fi

if [ -z "${ROSE_ROOT}" ]; then
	ROSE_ROOT=/usr/local/rose-current
fi

if [ -n "${INSTALL_ROOT}" ]; then
	POCC_INSTALL_ROOT="${INSTALL_ROOT}/pocc"
	INSTALL_ROOT="--prefix=${INSTALL_ROOT}"
else
	POCC_INSTALL_ROOT="`pwd`/pocc"
fi

echo "*** Installation of PolyRose ***"
echo "If using plooto@cse, you should have exported LD_LIBRARY_PATH=/usr/local/lib64/gcj-4.3.0-9:/usr/local/boost/lib"
echo "[PolyRose] Install PoCC..."
POCC_INSTALL_PREFIX="$POCC_INSTALL_ROOT" ./pocc-installer.sh devel
check_error "$?" "Installation of PoCC failed";
echo "[PolyRose] Bootstrap..."
if ! [ -f configure ]; then
    ./bootstrap.sh
    check_error "$?" "Bootstrap failed";
fi;
echo "[PolyRose] Configure..."
if ! [ -f Makefile ]; then
    ./configure --with-boost=${BOOST_ROOT} --with-rose=${ROSE_ROOT} ${INSTALL_ROOT} --with-pocc-prefix=$POCC_INSTALL_ROOT
    check_error "$?" "Configure failed";
fi;
echo "[PolyRose] Make..."
make
check_error "$?" "Build failed";

if [ -n "${INSTALL_ROOT}" ]; then
	make install
	check_error "$?" "make install failed";
fi

echo "[PolyRose] Installation complete."
echo "

* Usage
-------

$> src/PolyRose <filename.c>

To use tiling with pluto:
$> src/PolyRose --polyopt-pluto-tile <filename.c>

Options are:
--polyopt-pluto-tile
--polyopt-pluto-parallel (do not generate the OpenMP pragmas at this time)
--polyopt-pluto-prevector



* Troubleshoot
--------------
For any error please email polyhedral@cse.ohio-state.edu
";
