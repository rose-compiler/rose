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
    echo "[Polyopt] $ERRMESSAGE";
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

echo "*** Installation of Polyopt ***"
echo "[PolyOpt] The environment variable ROSE_ROOT must be set and point to a valid ROSE install tree; the variable BOOST_ROOT must be set and point to the BOOST install tree used to build ROSE";
echo "[PolyOpt] Proceed? [Y/n]";
read n;
if [ "$n" = n ] || [ "$n" = N ]; then exit 1; fi;
echo "[Polyopt] Install PoCC..."
if ! [ -z "$ROSE_SRC" ]; then
    POCC_INSTALL_PREFIX="$POCC_INSTALL_ROOT" $ROSE_SRC/projects/PolyOpt2/pocc-installer.sh
else
    POCC_INSTALL_PREFIX="$POCC_INSTALL_ROOT" ./pocc-installer.sh
fi;
check_error "$?" "Installation of PoCC failed";
echo "[Polyopt] Bootstrap..."
if ! [ -f configure ]; then
    if ! [ -z "$ROSE_SRC" ]; then
	cd $ROSE_SRC/projects/PolyOpt2 && ./bootstrap.sh; cd -
    else
	./bootstrap.sh
    fi;
    check_error "$?" "Bootstrap failed";
fi;
echo "[Polyopt] Configure..."
if ! [ -f Makefile ]; then
    if ! [ -z "$ROSE_SRC" ]; then
	$ROSE_SRC/projects/PolyOpt2/configure --with-boost=${BOOST_ROOT} --with-rose=${ROSE_ROOT} ${INSTALL_ROOT} --with-pocc-prefix=$POCC_INSTALL_ROOT
    else
	./configure --with-boost=${BOOST_ROOT} --with-rose=${ROSE_ROOT} ${INSTALL_ROOT} --with-pocc-prefix=$POCC_INSTALL_ROOT
    fi;
    check_error "$?" "Configure failed";
fi;
echo "[Polyopt] Make..."
make
check_error "$?" "Build failed";

if [ -n "${INSTALL_ROOT}" ]; then
	make install
	check_error "$?" "make install failed";
fi

echo "[Polyopt] Installation complete."
echo "

* Usage
-------

$> src/Polyopt <filename.c>

To use tiling with pluto:
$> src/Polyopt --polyopt-pluto-tile <filename.c>

Options are:
--polyopt-high-order-stencil
--polyopt-fixed-tiling
--polyopt-help


* Troubleshoot
--------------
For any error please email polyhedral@cse.ohio-state.edu
";
