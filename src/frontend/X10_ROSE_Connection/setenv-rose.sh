#
# bash script to set environment variables for setup-rose.sh
#

#
# This script assumes the following installation directories.
# They can be changed as needed.
#
_BOOST_ROOT="$HOME/opt/boost"
_ROSE_ROOT="$HOME/rose"
_X10_VER=2.4.3.2
_X10_DIST="$HOME/x10/x10-$_X10_VER/x10.dist"


#
# This script will set the following environment variables that are required for building and running ROSE with X10 support
#
# set JAVA_HOME to Oracle JDK 64-Bit Server VM
#   add $JAVA_HOME/lib:$JAVA_HOME/jre/lib/amd64/server to LD_LIBRARY_PATH
#   add $JAVA_HOME/bin to PATH
#
# set ANT_HOME to Apache Ant
#   add $ANT_HOME/bin to PATH
#
# add $_ROSE_ROOT/install/latest/bin:$_X10_DIST/bin to PATH
#
# add $_ROSE_ROOT/install/latest/lib:$_BOOST_ROOT/latest/lib to LD_LIBRARY_PATH
#


################################################################
# set JAVA_HOME to Oracle JDK 64-Bit Server VM
################################################################

#
# NOTE: keep the followings consistent
#
_JDK_VER=1.7.0
_JDK_DIR=jdk1.7.0_67
_JDK_TAR=jdk-7u67-linux-x64.tar.gz
_JDK_URL=http://download.oracle.com/otn-pub/java/jdk/7u67-b01/$_JDK_TAR

function check_jdk() {
  [[ ("$1" =~ "HotSpot") && ("$1" =~ "64-Bit Server VM") && ("$1" =~ "$_JDK_VER") ]] && return 0
  return 1
}


unset _HAS_JDK
if [ -z "$JAVA_HOME" ]; then

####if [ -d "/usr/java/default" ]; then
####_JAVA_VERSION=$("/usr/java/default/bin/java" -version 2>&1)
####check_jdk "$_JAVA_VERSION"
####if [ $? -eq 0 ]; then
####export JAVA_HOME="/usr/java/default"
####_HAS_JDK=1
####fi
####fi
if [[ ( -z "$_HAS_JDK" ) && ( -d "$HOME/opt/$_JDK_DIR" ) ]]; then
_JAVA_VERSION=$("$HOME/opt/$_JDK_DIR/bin/java" -version 2>&1)
check_jdk "$_JAVA_VERSION"
if [ $? -eq 0 ]; then
export JAVA_HOME="$HOME/opt/$_JDK_DIR"
_HAS_JDK=1
fi
fi

if [ -z "$_HAS_JDK" ]; then
if [ -d "$HOME/opt/$_JDK_DIR" ]; then
echo "========================================================================"
echo "  $HOME/opt/$_JDK_DIR exists but not sure it's compatible with ROSE.    "
echo "  Will back it up and install Oracle JDK to the same path.              "
echo "========================================================================"
_DATE=$(date +%Y%m%d-%H%M)
mv "$HOME/opt/$_JDK_DIR" "$HOME/opt/${_JDK_DIR}-backup_for_rose-$_DATE"
fi
if [ ! -f "/tmp/$_JDK_TAR" ]; then
echo "========================================================================"
echo "  Downloading Oracle JDK ${_JDK_VER}...                                 "
echo "========================================================================"
wget --no-check-certificate --no-cookies --header "Cookie: oraclelicense=accept-securebackup-cookie" $_JDK_URL -O "/tmp/$_JDK_TAR"
echo "========================================================================"
echo "  Downloading Oracle JDK $_JDK_VER is DONE                              "
echo "========================================================================"
fi
echo "========================================================================"
echo "  Installing Oracle JDK $_JDK_VER to $HOME/opt/${_JDK_DIR}...           "
echo "========================================================================"
mkdir -p $HOME/opt
(cd $HOME/opt; tar xfz /tmp/$_JDK_TAR)
[ "$(whoami)" = "root" ] && chown -R root:root "$HOME/opt/$_JDK_DIR"
#echo "Removing temporary files..."
#rm /tmp/$_JDK_TAR
echo "========================================================================"
echo "  Installing Oracle JDK $_JDK_VER to $HOME/opt/$_JDK_DIR is DONE        "
echo "========================================================================"
export JAVA_HOME="$HOME/opt/$_JDK_DIR"
_HAS_JDK=1
fi

else

#
# JAVA_HOME is set
# Check if JAVA_HOME points to a ROSE compatible version
#
_JAVA_VERSION=$("$JAVA_HOME/bin/java" -version 2>&1)
check_jdk "$_JAVA_VERSION"
[ $? -eq 0 ] && _HAS_JDK=1

fi


if [ -z "$_HAS_JDK" ]; then

#
# _HAS_JDK isn't set. Will skip everything.
#
echo "JAVA_HOME is set but not sure it's compatible with ROSE. Please unset JAVA_HOME and run again."

else

#
# _HAS_JDK is set
#

if [ -n "$LD_LIBRARY_PATH" ]; then
export LD_LIBRARY_PATH="$JAVA_HOME/lib:$JAVA_HOME/jre/lib/amd64/server:/usr/lib:$LD_LIBRARY_PATH"
else
export LD_LIBRARY_PATH="$JAVA_HOME/lib:$JAVA_HOME/jre/lib/amd64/server:/usr/lib"
fi
export PATH="$JAVA_HOME/bin:$PATH"


################################################################
# add Apache Ant to PATH (for building X10)
################################################################

#
# NOTE: keep the followings consistent
#
_ANT_VER=1.9.4
_ANT_DIR=apache-ant-${_ANT_VER}
_ANT_TAR=${_ANT_DIR}-bin.tar.gz
_ANT_URL=http://www.apache.org/dist/ant/binaries/$_ANT_TAR

unset _ANT_VERSION
[ $(which ant 2>/dev/null) ] && _ANT_VERSION=$(ant -version)

if [[ ! ("$_ANT_VERSION" =~ "$_ANT_VER") ]]; then
if [ ! -d "$HOME/opt/$_ANT_DIR" ]; then
if [ ! -f "/tmp/$_ANT_TAR" ]; then
echo "========================================================================"
echo "  Downloading Apache Ant...                                             "
echo "========================================================================"
wget $_ANT_URL -O /tmp/$_ANT_TAR
echo "========================================================================"
echo "  Downloading Apache Ant is DONE                                        "
echo "========================================================================"
fi
echo "========================================================================"
echo "  Installing Apache Ant to $HOME/opt/${_ANT_DIR}...                     "
echo "========================================================================"
mkdir -p $HOME/opt
(cd $HOME/opt; tar xfz /tmp/$_ANT_TAR)
[ "$(whoami)" = "root" ] && chown -R root:root "$HOME/opt/$_ANT_DIR"
#echo "Removing temporary files..."
#rm /tmp/$_ANT_TAR
echo "========================================================================"
echo "  Installing Apache Ant to $HOME/opt/$_ANT_DIR is DONE                  "
echo "========================================================================"
fi
export ANT_HOME="$HOME/opt/$_ANT_DIR"
export PATH="$ANT_HOME/bin:$PATH"
fi
#export ANT_OPTS="-Duser.language=en"


################################################################
# add ROSE and X10 to PATH
# add BOOST (and ROSE) to LD_LIBRARY_PATH
################################################################

#_LD_LIBRARY_PATH="$_BOOST_ROOT/latest/lib"
_LD_LIBRARY_PATH="$_ROSE_ROOT/install/latest/lib:$_BOOST_ROOT/latest/lib"
if [ -n "$LD_LIBRARY_PATH" ]; then
export LD_LIBRARY_PATH="$_LD_LIBRARY_PATH:$LD_LIBRARY_PATH"
else
export LD_LIBRARY_PATH="$_LD_LIBRARY_PATH"
fi
export PATH="$_ROSE_ROOT/install/latest/bin:$_X10_DIST/bin:$PATH"

#
# The end of "_HAS_JDK is set"
#
fi
