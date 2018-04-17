#!/bin/sh
## pocc-installer.sh for pace in /Users/pouchet/osu/projects/svnbarista/branches/poccrose
##
## Made by Louis-Noel Pouchet
## Contact: <louis-noel.pouchet@inria.fr>
##
## Started on  Mon Jan  4 16:37:23 2010 Louis-Noel Pouchet
## Last update Wed May 31 12:20:16 2017 Louis-Noel Pouchet
##



## Install the release version of PoCC.
ADDRESS="https://downloads.sourceforge.net/project/pocc/1.4"
SOFTVERSION="pocc-1.4"
ARCHVERSION="-netinstall"

new_file=0;
getfile=`wget --no-check-certificate -N $ADDRESS/$SOFTVERSION$ARCHVERSION.tar.gz 2>&1 | grep "not retrieving"`;
mustupdate=`wget --no-check-certificate -N $ADDRESS/$SOFTVERSION$ARCHVERSION.tar.gz.updatestamp 2>&1 | grep "not retrieving"`;
if [ -z "$getfile" ]; then
    new_file=1;
    tar xzf $SOFTVERSION$ARCHVERSION.tar.gz
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

