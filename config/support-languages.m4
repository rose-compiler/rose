AC_DEFUN([ROSE_SUPPORT_LANGUAGES],
[
# TOO (3/10/2011):
# uppercase: sed 's/./\U&/g'
# DQ (4/15/2010): Added support to specify selected languages to support in ROSE.
echo "------------------------------------------------"
AC_MSG_CHECKING([user-specified languages to support])
echo ""

#########################################################################################
#
#  Manage language support command-line options:
#
#	--enable-languages(=args)
#	--enable-binary-analysis		TODO: make alias: --enable-binaries
#	--enable-c
#	--enable-cxx
#	--enable-cuda
#	--enable-fortran
#	--enable-java
#	--enable-php
#	--enable-opencl
#
#	TODO:
#	-Issue warning if user specifies conflicting language options, e.g.
#	         --enable-languages=binaries --disable-binaries
#
#########################################################################################

ALL_SUPPORTED_LANGUAGES="binaries c c++ cuda fortran java php opencl"
#########################################################################################
AC_ARG_ENABLE([languages],
#########################################################################################
               AS_HELP_STRING([--enable-languages=LIST],[Build specific languages: all,none,binary-analysis,c,c++,cuda,fortran,java,opencl,php (default=all)]),,
               [enableval=all])

	       # Default support for all languages
	       case "$enableval" in
 	         all|yes)
		 	LANGUAGES_TO_SUPPORT="$ALL_SUPPORTED_LANGUAGES"
		 	;;
                 *)
                 	LANGUAGES_TO_SUPPORT="$enableval"
                 	;;
	       esac
#########################################################################################
AC_ARG_ENABLE([binary-analysis],
#########################################################################################
               AS_HELP_STRING([--enable-binary-analysis],[Enable binary analysis support in ROSE (default=yes)]),
               ##########################################################################
                if [[[[ "$LANGUAGES_TO_SUPPORT" =~ "binaries" ]]]] ; then
                  list_has_binaries=yes
                fi
                case "$enableval" in
                  [yes)]
                  	if test "x$list_has_binaries" != "xyes" ; then
                          # --enable-languages does not include binaries, but --enable-binary-analysis=yes
                  	  LANGUAGES_TO_SUPPORT+=" binaries"
                        fi
                  	;;
                  [no)]
                        # remove 'binaries' from support languages list
                  	LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | sed 's/binaries//g'`"
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-binary-analysis='$enableval' is unsupported, use yes/no])]
                 	;;
                esac
               ##########################################################################
               ,)


#########################################################################################
#
#  Set flags to indicate which languages to support according to the
#  user specified command-line options 
#
#########################################################################################
# Convert support-language-list to a space-separated list, stripping
# leading and trailing whitespace
LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | sed -e 's/,/ /g;s/^[ \t]*//;s/[ \t]*$//'`" 
#DEBUG#echo "LANGUAGES_TO_SUPPORT='$LANGUAGES_TO_SUPPORT'"

if test "x$LANGUAGES_TO_SUPPORT" = "x" ; then
  LANGUAGES_TO_SUPPORT=none
fi
for a_language in $LANGUAGES_TO_SUPPORT ; do
case "$a_language" in 

none|no)
	support_binaries=no
	support_c_language=no
	support_cxx_language=no
	support_cuda_language=no
	support_fortran_language=no
	support_java_language=no
	support_php_language=no
	support_opencl_language=no
	AC_MSG_WARN([you did not enable any language support])
	;;
binaries)
	support_binaries=yes
	;;
c)
	support_c_language=yes
	;;
c++)
	support_cxx_language=yes
	;;
cuda)
	support_cuda_language=yes
	;;
fortran)
           if test "x$USE_JAVA" = x1; then
              if test "x$GFORTRAN_PATH" != "x"; then
                 echo "Build separate: found a valid gfortran, java is available, OK to support Fortran"
                 support_fortran_language=yes
              else
                 echo "Build separate: gfortran not found and required for syntax checking and semantic analysis, fortran disabled"
              fi
           else
            # echo "Specified to build all, but internal java support is unavalable so we can't support Fortran or Java languages in ROSE."
              AC_MSG_RESULT(Specified to build fortran, but internal java support is unavalable so we can't support Fortran or Java languages in ROSE.)
              disable_languages=yes
              support_fortran_language=no
           fi
	;;
java)
           if test "x$USE_JAVA" = x1; then
              support_java_language=yes
           else
              AC_MSG_RESULT(Specified to build java, but internal java support is unavalable so we can't support Fortran or Java languages in ROSE.)
              disable_languages=yes
              support_java_language=no
           fi
	;;
php)
	support_php_language=yes
	;;
opencl)
	support_opencl_language=yes
	;;
*)
	AC_MSG_FAILURE([unrecognized language '$a_language'])
	;;
esac
done


#########################################################################################
#
#  Output whether or not a specific language is supported
#
#########################################################################################
function print_isLanguageSupported() {
  if test ${#} != 2 ; then
   echo "Usage: print_isLanguageSupported <language> <is_supported>"
   exit 1
  else 
   local language=${1}
   local is_supported=${2}
  fi

  if test "x$is_supported" = "xyes" ; then
    echo "  + $language"
  else
    echo "  - $language"
  fi
}
echo ""
print_isLanguageSupported "Binary analysis" "$support_binaries"
print_isLanguageSupported "C" "$support_c_language"
print_isLanguageSupported "C++" "$support_cxx_language"
print_isLanguageSupported "Cuda" "$support_cuda_language"
print_isLanguageSupported "Fortran" "$support_fortran_language"
print_isLanguageSupported "Java" "$support_java_language"
print_isLanguageSupported "PHP" "$support_php_language"
print_isLanguageSupported "OpenCL" "$support_opencl_language"
echo ""
echo "(+)enabled (-)disabled"
#AC_MSG_RESULT($LANGUAGES_TO_SUPPORT)
echo "------------------------------------------------"

# End macro ROSE_SUPPORT_LANGUAGES.
])
