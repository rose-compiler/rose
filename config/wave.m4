AC_DEFUN([ROSE_SUPPORT_LIBWAVE],
[
# Begin macro ROSE_SUPPORT_ QROSE.

AC_ARG_WITH(wave, [  --with_boost-wave=yes Specify if wave is to be used (to use numbers as string and introduce unexpanded macros into the AST)])

# Test if it is defined and if not then set it to be defined as "no"
# if test ! "$with_boost_wave" ; then
#   with_wave=no
#fi

# echo "with_wave = $with_wave"
# echo "with_boost_wave = $with_boost_wave"

# echo "In lib wave macro: compiler CC = $CC version GCC_MAJOR = $GCC_MAJOR GCC_MINOR = $GCC_MINOR"

# Only query for Wave library if we are trying to build ROSE with wave Andreass preprocessor work
# Not clear if this is sufficently powerful of a serach mechanism to find teh required libraries.
if (test "$with_boost_wave" = yes); then
   VAR_GCC=1
 # echo "Calling wave macro ..."
   AC_PATH_WAVE

 # DQ (4/7/2006): build a macro to use in source code to know when WAVE is to be used.
   AC_DEFINE([USE_ROSE_BOOST_WAVE_SUPPORT],1,[Controls use of BOOST WAVE support in ROSE.])
fi

# End macro ROSE_SUPPORT_ LIBWAVE.
])



dnl ------------------------------------------------------------------------
dnl Try to find the Open GL headers and libraries.
dnl $(GL_LDFLAGS) will be -Lglliblocation (if needed)
dnl and $(GL_INCLUDES) will be -Iglheaderlocation (if needed)
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_PATH_WAVE],
[

echo "In path wave macro: compiler CC = $CC version GCC_MAJOR = $GCC_MAJOR GCC_MINOR = $GCC_MINOR"
# BOOST_LIB_SUFFIX="$CC""$GCC_MAJOR""$GCC_MINOR"
#BOOST_LIB_SUFFIX="$CC$GCC_MAJOR$GCC_MINOR"
BOOST_LIB_SUFFIX="$CC"
echo "BOOST_LIB_SUFFIX = $BOOST_LIB_SUFFIX"
AC_SUBST(BOOST_LIB_SUFFIX)

if test ${VAR_GCC} = 1
then
# LIB_WAVE="-lboost_wave-gcc"
LIB_WAVE="-lboost_wave-$BOOST_LIB_SUFFIX"
else
LIB_WAVE="-lboost_wave"
fi
AC_MSG_CHECKING([for Wave])

ac_wave_includes="" ac_wave_libraries=""

AC_ARG_WITH(boost-includes,
    [  --with-boost-includes=DIR   where the BOOST includes are. ],
    [  ac_wave_includes="$withval"
    ])

AC_ARG_WITH(boost-libraries,
    [  --with-boost-libraries=DIR where the Wave libraries are. ],
    [  ac_wave_libraries="$withval"
    ])

AC_CACHE_VAL(ac_cv_have_wave,
[
wave_incdirs=" $ac_wave_includes $BOOSTDIR /usr/include "
AC_FIND_FILE(boost/wave.hpp, $wave_incdirs, wave_incdir)
ac_wave_includes="$wave_incdir"

wave_libdirs="$ac_wave_libraries $BOOSTLIB /usr/lib /usr/local/lib "

test -n "$BOOSTDIR" && wave_libdirs="$BOOSTDIR $wave_libdirs"
test=NONE
wave_libdir=NONE
for dir in $wave_libdirs; do
  try="ls -1 $dir/libboost*"
  if test=`eval $try 2> /dev/null`; then wave_libdir=$dir; break; else echo "tried $dir" >&AC_FD_CC ; fi
done
ac_wave_libraries="$wave_libdir"

 WAVE_INCLUDES="-I$ac_wave_includes"
 WAVE_LDFLAGS="-L$wave_gl_libraries"
])

eval "$ac_cv_have_wave"
if test "$ac_wave_libraries" = NONE; then
  AC_MSG_ERROR([ Not found , put your BOOSTDIR environnement variable to the BOOST directory ]);
else
  ac_cv_have_wave="have_wave=yes \
  ac_wave_includes=$ac_wave_includes ac_wave_libraries=$ac_wave_libraries"
  AC_MSG_RESULT([ libraries $ac_wave_libraries, headers $ac_wave_includes ])  
  WAVE_LIBRARIES="-L$ac_wave_libraries"
  wave_includes="$ac_wave_includes"
  WAVE_INCLUDES="-I$ac_wave_includes"	
  WAVE_LDFLAGS="-rpath $ac_wave_libraries"

fi

echo "WAVE_LIBRARIES = $WAVE_LIBRARIES"
echo "wave_includes  = $wave_includes"
echo "WAVE_INCLUDES  = $WAVE_INCLUDES"
echo "WAVE_LDFLAGS   = $WAVE_LDFLAGS"

AC_SUBST(WAVE_LIBRARIES)
AC_SUBST(wave_includes)
AC_SUBST(WAVE_INCLUDES)
AC_SUBST(WAVE_LDFLAGS)

LIB_WAVE="-lboost_wave-$BOOST_LIB_SUFFIX -lboost_filesystem-$BOOST_LIB_SUFFIX -lboost_regex-$BOOST_LIB_SUFFIX"

#if test ${VAR_GCC} = 1
#then
#LIB_WAVE="-lboost_wave-$BOOST_LIB_SUFFIX -lboost_filesystem-$BOOST_LIB_SUFFIX"
#LIB_WAVE="-lboost_wave"
#else
#LIB_WAVE="-lboost_wave-$BOOST_LIB_SUFFIX -lboost_filesystem-$BOOST_LIB_SUFFIX "
#fi
AC_SUBST(LIB_WAVE)
])

