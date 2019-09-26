# ===========================================================================
#      http://www.gnu.org/software/autoconf-archive/ax_python_devel.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PYTHON_DEVEL([version])
#
# DESCRIPTION
#
#   Note: This script has been modified for ROSE.
#
#   Note: Defines as a precious variable "PYTHON_VERSION". Don't override it
#   in your configure.ac.
#
#   This macro checks for Python and tries to get the include path to
#   'Python.h'. It provides the $(PYTHON_CPPFLAGS) and $(PYTHON_LDFLAGS)
#   output variables. It also exports $(PYTHON_EXTRA_LIBS) and
#   $(PYTHON_EXTRA_LDFLAGS) for embedding Python in your code.
#
#   You can search for some particular version of Python by passing a
#   parameter to this macro, for example ">= '2.3.1'", or "== '2.4'". Please
#   note that you *have* to pass also an operator along with the version to
#   match, and pay special attention to the single quotes surrounding the
#   version number. Don't use "PYTHON_VERSION" for this: that environment
#   variable is declared as precious and thus reserved for the end-user.
#
#   This macro should work for all versions of Python >= 2.1.0. As an end
#   user, you can disable the check for the python version by setting the
#   PYTHON_NOVERSIONCHECK environment variable to something else than the
#   empty string.
#
#   If you need to use this macro for an older Python version, please
#   contact the authors. We're always open for feedback.
#
# LICENSE
#
#   Copyright (c) 2009 Sebastian Huber <sebastian-huber@web.de>
#   Copyright (c) 2009 Alan W. Irwin <irwin@beluga.phys.uvic.ca>
#   Copyright (c) 2009 Rafael Laboissiere <rafael@laboissiere.net>
#   Copyright (c) 2009 Andrew Collier <colliera@ukzn.ac.za>
#   Copyright (c) 2009 Matteo Settenvini <matteo@member.fsf.org>
#   Copyright (c) 2009 Horst Knorr <hk_classes@knoda.org>
#
#   This program is free software: you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation, either version 3 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <http://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Archive. When you make and distribute a
#   modified version of the Autoconf Macro, you may extend this special
#   exception to the GPL to apply to your modified version as well.

AU_ALIAS([AC_PYTHON_DEVEL], [AX_PYTHON_DEVEL])
AC_DEFUN([AX_PYTHON_DEVEL],[
    #
    # Allow the use of a (user set) custom python version
    #
    AC_ARG_WITH(python,
        [  --with-python=PATH   Specify the path to the Python interpreter])

    if test "$with_python" = "no"; then
        # User does not want us to use python even if it's available.
        PYTHON=
    elif test "$with_python" = "yes"; then
        # User wants python, but we should find it ourselves
        AC_PATH_PROG([PYTHON], [python])
        if test "$PYTHON" = ""; then
            AC_MSG_FAILURE(["python" executable not found but "--with-python=yes" was specified])
        fi
    elif test "$with_python" = ""; then
        # User doesn't care if we use python or not
        AC_PATH_PROG([PYTHON], [python])
    else
        # User is telling us where to find the python interpreter
        PYTHON="$with_python"
    fi


    if test "$PYTHON" != ""; then
        AC_SUBST([PYTHON], $PYTHON)

        #
        # determine if python is the right version
        #
        AC_MSG_CHECKING([for a version of Python >= $1, < $2])
        ac_python_version=`$PYTHON -c "import sys; print(sys.version.split()[[0]])"`
        ac_supports_python_ver=`$PYTHON -c "print ('$1' <= '$ac_python_version' < '$2')"`
        if test "$ac_supports_python_ver" = "True"; then
            AC_MSG_RESULT([yes ($ac_python_version)])
            AC_DEFINE([PYTHON_VERSION], ($ac_python_version), "Version of Python selected when building ROSE.")
        else
            AC_MSG_RESULT([no ($ac_python_version)])
            AC_MSG_ERROR([this package requires Python >= $1 and < $2. If you have it installed, but it isn't the default Python interpreter in your system path, please configure --with-python=PATH to select the correct interpreter. See ``configure --help'' for reference.])
        fi

        #
        # Python installation prefix
        #
        AC_MSG_CHECKING([for python instllation prefix])
        PYTHON_ROOT=`$PYTHON -c 'import sys; print(sys.prefix);'`
        AC_MSG_RESULT([$PYTHON_ROOT])
        AC_SUBST(PYTHON_ROOT)

        #
        # Python major.minor version
        #
        AC_MSG_CHECKING([for python two-part version])
        PYTHON_VERSION_MM=`$PYTHON -c 'import sys; print(str.join(".", sys.version.split()[[0]].split(".")[[0:2]]));'`
        AC_MSG_RESULT([$PYTHON_VERSION_MM])
        AC_SUBST(PYTHON_VERSION_MM)
        
        #
        # Check if you have distutils, else fail
        #
        AC_MSG_CHECKING([for the distutils Python package])
        ac_distutils_result=`$PYTHON -c "import distutils" 2>&1`
        if test -z "$ac_distutils_result"; then
            AC_MSG_RESULT([yes])
        else
            AC_MSG_RESULT([no])
            AC_MSG_ERROR([cannot import Python module "distutils". Please check your Python installation. The error was: $ac_distutils_result])
        fi

        #
        # Check for Python include path
        #
        AC_MSG_CHECKING([for Python include path])
        if test -z "$PYTHON_CPPFLAGS"; then
            python_path=`$PYTHON -c "import distutils.sysconfig; \
                print (distutils.sysconfig.get_python_inc ());"`
            if test -n "${python_path}"; then
                python_path="-I$python_path"
            fi
            PYTHON_CPPFLAGS=$python_path
        fi
        AC_MSG_RESULT([$PYTHON_CPPFLAGS])

        #
        # Check for Python library path
        #
        AC_MSG_CHECKING([for Python library path])
        if test -z "$PYTHON_LDFLAGS"; then
            # (makes two attempts to ensure we've got a version number from the interpreter)
            ac_python_version=`cat<<EOD | $PYTHON -

# join all versioning strings, on some systems
# major/minor numbers could be in different list elements
from distutils.sysconfig import *
ret = ''
for e in get_config_vars ('VERSION'):
        if (e != None):
                ret += e
print (ret)
EOD`

            if test -z "$ac_python_version"; then
                ac_python_version=`$PYTHON -c "import sys; \
                    print (sys.version[[:3]])"`
            fi

            # Make the versioning information available to the compiler
            AC_DEFINE_UNQUOTED([HAVE_PYTHON], ["$ac_python_version"],
                               [If available, contains the Python version number currently in use.])

            # First, the library directory:
            ac_python_libdir=`cat<<EOD | $PYTHON -

# There should be only one
import distutils.sysconfig
for e in distutils.sysconfig.get_config_vars ('LIBDIR'):
    if e != None:
        print (e)
        break
EOD`

            # Before checking for libpythonX.Y, we need to know
            # the extension the OS we're on uses for libraries
            # (we take the first one, if there's more than one fix me!):
            ac_python_soext=`$PYTHON -c \
              "import distutils.sysconfig; \
               print (distutils.sysconfig.get_config_vars('SO')[[0]])"`

            # Now, for the library:
            ac_python_soname=`$PYTHON -c \
                "import distutils.sysconfig; \
                 print (distutils.sysconfig.get_config_vars('LDLIBRARY')[[0]])"`

            # Strip away extension from the end to canonicalize its name:
            ac_python_library=`echo "$ac_python_soname" | sed "s/${ac_python_soext}$//"`

            # This small piece shamelessly adapted from PostgreSQL python macro;
            # credits goes to momjian, I think. I'd like to put the right name
            # in the credits, if someone can point me in the right direction... ?
            if test -n "$ac_python_libdir" -a -n "$ac_python_library" -a x"$ac_python_library" != x"$ac_python_soname"; then
                # use the official shared library
                ac_python_library=`echo "$ac_python_library" | sed "s/^lib//"`
                PYTHON_LDFLAGS="-L$ac_python_libdir -l$ac_python_library"
            else
                # old way: use libpython from python_configdir
                ac_python_libdir=`$PYTHON -c \
                    "from distutils.sysconfig import get_python_lib as f; \
                     import os; \
                     print (os.path.join(f(plat_specific=1, standard_lib=1), 'config'));"`
                PYTHON_LDFLAGS="-L$ac_python_libdir -lpython$ac_python_version"
            fi

            if test -z "PYTHON_LDFLAGS"; then
                AC_MSG_ERROR([cannot determine location of your Python DSO. Please check it was installed with dynamic libraries enabled, or try setting PYTHON_LDFLAGS by hand.])
            fi

            # At least on Robb's systems, where python 3 is installed from source code using autoconf's
            # "configure --prefix=$PYTHON_ROOT" followed by "make install", the python library is named
            # $PYTHON_ROOT/lib/libpython3.6m.so and there is no $PYTHON_ROOT/lib/python3.6/config/python3.6
            # even though the previous paragraph "detects" it.
            if test -f "$PYTHON_ROOT/lib/libpython${PYTHON_VERSION_MM}m.so"; then
                PYTHON_LDFLAGS="-L$PYTHON_ROOT/lib -lpython${PYTHON_VERSION_MM}m"
            fi
        fi
        AC_MSG_RESULT([$PYTHON_LDFLAGS])

        #
        # Check for site packages
        #
        AC_MSG_CHECKING([for Python site-packages path])
        if test -z "$PYTHON_SITE_PKG"; then
            PYTHON_SITE_PKG=`$PYTHON -c "import distutils.sysconfig; \
                print (distutils.sysconfig.get_python_lib(0,0));"`
        fi
        AC_MSG_RESULT([$PYTHON_SITE_PKG])
        AC_SUBST([PYTHON_SITE_PKG])

        #
        # libraries which must be linked in when embedding
        #
        AC_MSG_CHECKING(python extra libraries)
        if test -z "$PYTHON_EXTRA_LIBS"; then
           PYTHON_EXTRA_LIBS=`$PYTHON -c "import distutils.sysconfig; \
               conf = distutils.sysconfig.get_config_var; \
               print (conf('LOCALMODLIBS') + ' ' + conf('LIBS'))"`
        fi
        AC_MSG_RESULT([$PYTHON_EXTRA_LIBS])

        #
        # linking flags needed when embedding
        #
        AC_MSG_CHECKING(python extra linking flags)
        if test -z "$PYTHON_EXTRA_LDFLAGS"; then
            PYTHON_EXTRA_LDFLAGS=`$PYTHON -c "import distutils.sysconfig; \
                conf = distutils.sysconfig.get_config_var; \
                print (conf('LINKFORSHARED'))"`
        fi
        AC_MSG_RESULT([$PYTHON_EXTRA_LDFLAGS])

        #
        # final check to see if everything compiles alright
        #
        AC_MSG_CHECKING([consistency of all components of python development environment])
        # save current global flags
        ac_save_LIBS="$LIBS"
        ac_save_CPPFLAGS="$CPPFLAGS"
        LIBS="$ac_save_LIBS $PYTHON_LDFLAGS $PYTHON_EXTRA_LDFLAGS $PYTHON_EXTRA_LIBS"
        CPPFLAGS="$ac_save_CPPFLAGS $PYTHON_CPPFLAGS"
        AC_LANG_PUSH([C])
        AC_LINK_IFELSE([
            AC_LANG_PROGRAM([[#include <Python.h>]],
                            [[Py_Initialize();]])],
            [PYTHON_DEV_EXISTS=yes],
            [PYTHON_DEV_EXISTS=no])
        AC_LANG_POP([C])
        AC_MSG_RESULT([$PYTHON_DEV_EXISTS])


        if test "$OS_vendor" = "apple"; then
            if echo "$PYTHON_EXTRA_LIBS" |grep CoreFoundation >/dev/null; then
                AC_MSG_NOTICE([python API is not supported on macOS (CoreFoundation problems)])
                PYTHON_DEV_EXISTS=no
            fi
        fi      
                
        # If the python development environment exists and seems to work, then keep the
        # compiler flags we detected above, otherwise discard them.
        if test "$PYTHON_DEV_EXISTS" = no; then
            AC_MSG_NOTICE([python development environment is broken; clearing compiler flags])
            CPPFLAGS="$ac_save_CPPFLAGS"
            LIBS="$ac_save_LIBS"
            PYTHON_CPPFLAGS=
            PYTHON_LDFLAGS=
            PYTHON_EXTRA_LIBS=
            PYTHON_EXTRA_LDFLAGS=
        fi

        AC_SUBST([PYTHON_CPPFLAGS])
        AC_SUBST([PYTHON_LDFLAGS])
        AC_SUBST(PYTHON_EXTRA_LIBS)
        AC_SUBST(PYTHON_EXTRA_LDFLAGS)
    fi
])
