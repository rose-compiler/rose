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
