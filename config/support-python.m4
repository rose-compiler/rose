AC_DEFUN([ROSE_SUPPORT_PYTHON],
[
# Begin macro ROSE_SUPPORT_PYTHON.

# Inclusion of test for Python version 2.4 and it's location.

AC_MSG_CHECKING(for python >= 2.4.0)
AC_ARG_WITH(python,
[  --with-python=PATH       Specify the path to the Python 2.4 interpreter (the binary itself)],
,
)

echo "In ROSE SUPPORT MACRO: with_python $with_python"

if test "$with_python" = yes;
then
    with_python=""
    cat 1>&2 << EOF
Incorrect use of option --with-python ($with_python).
Please use: --with-python=/path/to/python/bin/python
EOF
fi

PYTHONGUESSES="$with_python python2.4 python2.5 python2.6 python2.7 python"
echo "Python interpreter guesses: $PYTHONGUESSES"

goodPy()
{
    if which $[1] > /dev/null 2>&1;
    then
        # DQ (12/28/2006): Can't use VERSION name (changed to PYTHON_VERSION), since it conflects with autoconf VERSION macro.
        PYTHON_VERSION=`$[1] -V 2>&1 | cut -f 2 -d " "`
        MAJOR=`echo $PYTHON_VERSION | cut -f 1 -d '.'`
        MINOR=`echo $PYTHON_VERSION | cut -f 2 -d '.'`
        RELEASE=`echo $PYTHON_VERSION | cut -f 3 -d '.'`
        if (test $MAJOR -eq 2) && (test $MINOR -ge 4);
        then
            return 0;
        fi
    fi
    return 1
}

python_path=''
for PYTHON in $PYTHONGUESSES
do
    if goodPy $PYTHON;
    then
        cat << EOF
Found a compatibale version of Python: $PYTHON
EOF
        python_path=$PYTHON
        break
    fi
done

if test -z "$python_path";
then
    echo "Could not find Python >=2.4.0!"
fi

AC_SUBST(python_path)

# End macro ROSE_SUPPORT_PYTHON.
]
)

