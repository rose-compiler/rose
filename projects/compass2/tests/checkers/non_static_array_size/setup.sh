GREP=/bin/grep
MYPARAMS="$(pwd)"/compass_parameters.xml

echo "$COMPASS_PARAMETERS" | "$GREP" -q "$MYPARAMS"
if [ "$?" -ne 0 ] ; then
        export COMPASS_PARAMETERS="$MYPARAMS"
fi

unset GREP
unset MYPARAMS

export PATH=/home/too1/local/workspace/rose/compass/install/bin:$PATH
export LD_LIBRARY_PATH=/home/too1/local/workspace/rose/compass/install/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/home/too1/local/boost/1_41/default-install/lib:$LD_LIBRARY_PATH
