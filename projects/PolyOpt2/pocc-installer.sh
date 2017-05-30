#!/bin/sh
## pocc-installer.sh for pace in /Users/pouchet/osu/projects/svnbarista/branches/poccrose
##
## Made by Louis-Noel Pouchet
## Contact: <louis-noel.pouchet@inria.fr>
##
## Started on  Mon Jan  4 16:37:23 2010 Louis-Noel Pouchet
## Last update Tue May 30 08:37:09 2017 Louis-Noel Pouchet
##


## Install the svn version of pocc.
if [ $# -eq 1 ]; then
    OPT="$1";
    if [ "$OPT" = "devel" ]; then
	echo "[Polyopt] Installing PoCC devel version";
	svn co --username lppcc --password lppcc https://alchemy.saclay.inria.fr/svn/users/pouchet/projects/pocc;
	if [ $? -ne 0 ]; then exit 1; fi;
	if [ -f "pocc/bin/pocc" ]; then
	    ## Don't force systematic recompilation
	    cd pocc && POCC_INSTALL_PREFIX="$POCC_INSTALL_PREFIX" bin/pocc-util buildall;
	    exit $?;
	else
	    ## Install the High Order STencil mode (HOST)
	    cd pocc && POCC_INSTALL_PREFIX="$POCC_INSTALL_PREFIX" ./install.sh host;
	    exit $?;
	fi;
    fi;
fi;


## Install the release version of PoCC.
ADDRESS="http://www.cse.ohio-state.edu/~pouchet/software/pocc/download/1.3-histencil"
SOFTVERSION="pocc-1.3-histencil"

new_file=0;
getfile=`wget --no-check-certificate -N $ADDRESS/$SOFTVERSION.tar.gz 2>&1 | grep "not retrieving"`;
mustupdate=`wget --no-check-certificate -N $ADDRESS/$SOFTVERSION.tar.gz.updatestamp 2>&1 | grep "not retrieving"`;
if [ -z "$getfile" ]; then
    new_file=1;
    tar xzf $SOFTVERSION.tar.gz
fi
if ! [ -d "pocc" ]; then
    mv $SOFTVERSION pocc
    cd pocc && POCC_INSTALL_PREFIX="$POCC_INSTALL_PREFIX" ./install.sh
else
    if [ "$new_file" = 1 ]; then
	rm -rf pocc;
	mv $SOFTVERSION pocc;
	cd pocc && POCC_INSTALL_PREFIX="$POCC_INSTALL_PREFIX" ./install.sh;
    else
	mustupdate="no";
	if [ -z "$mustupdate" ]; then
	    cd pocc && ./bin/pocc-util upgrade
	else
	    if [ -f "pocc/bin/pocc" ]; then
		echo "[Polyopt] PoCC already installed";
	    else
		echo "[Polyopt] Re-running install of PoCC";
		cd pocc && POCC_INSTALL_PREFIX="$POCC_INSTALL_PREFIX" ./install.sh
	    fi;
	fi;
    fi;
fi;

