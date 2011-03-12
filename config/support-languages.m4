AC_DEFUN([ROSE_SUPPORT_LANGUAGES],
[
# TOO (3/10/2011):
# uppercase: sed 's/./\U&/g'
# DQ (4/15/2010): Added support to specify selected languages to support in ROSE.
echo "------------------------------------------------"
AC_MSG_CHECKING([for user-specified languages to support])
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

# Convert support-language-list to a space-separated list, stripping
# leading and trailing whitespace
LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | sed -e 's/,/ /g;s/^[ \t]*//;s/[ \t]*$//'`" 
#DEBUG#echo "LANGUAGES_TO_SUPPORT='$LANGUAGES_TO_SUPPORT'"
#########################################################################################
AC_ARG_ENABLE([binary-analysis],
#########################################################################################
               AS_HELP_STRING([--enable-binary-analysis],[Enable binary analysis support in ROSE (default=yes)]),
               ##########################################################################
                echo "$LANGUAGES_TO_SUPPORT" | grep --quiet "binaries"
                if test $? = 0 ; then 
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
                  	[AC_MSG_FAILURE([--enable-binary-analysis='$enableval' is unsupported. Use 'yes' or 'no'])]
                 	;;
                esac
               ##########################################################################
               ,)
#########################################################################################
AC_ARG_ENABLE([c],
#########################################################################################
               AS_HELP_STRING([--enable-c],[Enable C language support in ROSE (default=yes)]),
               ##########################################################################
                echo "$LANGUAGES_TO_SUPPORT" | grep --quiet "\bc\b"
                if test $? = 0 ; then 
                  list_has_c=yes
                fi
                case "$enableval" in
                  [yes)]
                  	if test "x$list_has_c" != "xyes" ; then
                          # --enable-languages does not include C, but --enable-c=yes
                  	  LANGUAGES_TO_SUPPORT+=" c"
                        fi
                  	;;
                  [no)]
                        # remove 'C' from support languages list
                        # TOO (3/11/2011): couldn't find a nice way to handle with sed, cases: "c", "c c++", ...
                  	LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | [awk '{for (i=1; i<=NF; i++) { if ($i != "c") { printf "%s ",$i; } } }']`" 
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-c='$enableval' is unsupported. Use 'yes' or 'no'])]
                 	;;
                esac
               ##########################################################################
               ,)
#########################################################################################
AC_ARG_ENABLE([cxx],
#########################################################################################
               AS_HELP_STRING([--enable-cxx],[Enable C++ language support in ROSE (default=yes)]),
               ##########################################################################
                echo "$LANGUAGES_TO_SUPPORT" | grep --quiet "c++"
                if test $? = 0 ; then 
                  list_has_cxx=yes
                fi
                case "$enableval" in
                  [yes)]
                  	if test "x$list_has_cxx" != "xyes" ; then
                          # --enable-languages does not include C++, but --enable-cxx=yes
                  	  LANGUAGES_TO_SUPPORT+=" c++"
                        fi
                  	;;
                  [no)]
                        # remove 'C++' from support languages list
                  	LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | sed 's/c++//g'`"
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-cxx='$enableval' is unsupported. Use 'yes' or 'no'])]
                 	;;
                esac
               ##########################################################################
               ,)
#########################################################################################
AC_ARG_ENABLE([cuda],
#########################################################################################
               AS_HELP_STRING([--enable-cuda],[Enable Cuda language support in ROSE (default=yes)]),
               ##########################################################################
                echo "$LANGUAGES_TO_SUPPORT" | grep --quiet "cuda"
                if test $? = 0 ; then 
                  list_has_cuda=yes
                fi
                case "$enableval" in
                  [yes)]
                  	if test "x$list_has_cuda" != "xyes" ; then
                          # --enable-languages does not include Cuda, but --enable-cuda=yes
                  	  LANGUAGES_TO_SUPPORT+=" cuda"
                        fi
                  	;;
                  [no)]
                        # remove 'Cuda' from support languages list
                  	LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | sed 's/cuda//g'`"
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-cuda='$enableval' is unsupported. Use 'yes' or 'no'])]
                 	;;
                esac
               ##########################################################################
               ,)
#########################################################################################
AC_ARG_ENABLE([fortran],
#########################################################################################
               AS_HELP_STRING([--enable-fortran],[Enable Fortran language support in ROSE (default=yes)]),
               ##########################################################################
                echo "$LANGUAGES_TO_SUPPORT" | grep --quiet "fortran"
                if test $? = 0 ; then 
                  list_has_fortran=yes
                fi
                case "$enableval" in
                  [yes)]
                  	if test "x$list_has_fortran" != "xyes" ; then
                          # --enable-languages does not include Fortran, but --enable-fortran=yes
                  	  LANGUAGES_TO_SUPPORT+=" fortran"
                        fi
                  	;;
                  [no)]
                        # remove 'Fortran' from support languages list
                  	LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | sed 's/fortran//g'`"
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-fortran='$enableval' is unsupported. Use 'yes' or 'no'])]
                 	;;
                esac
               ##########################################################################
               ,)
#########################################################################################
AC_ARG_ENABLE([java],
#########################################################################################
               AS_HELP_STRING([--enable-java],[Enable Java language support in ROSE (default=yes)]),
               ##########################################################################
                echo "$LANGUAGES_TO_SUPPORT" | grep --quiet "java"
                if test $? = 0 ; then 
                  list_has_java=yes
                fi
                case "$enableval" in
                  [yes)]
                  	if test "x$list_has_java" != "xyes" ; then
                          # --enable-languages does not include Java, but --enable-java=yes
                  	  LANGUAGES_TO_SUPPORT+=" java"
                        fi
                  	;;
                  [no)]
                        # remove 'Java' from support languages list
                  	LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | sed 's/java//g'`"
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-java='$enableval' is unsupported. Use 'yes' or 'no'])]
                 	;;
                esac
               ##########################################################################
               ,)
#########################################################################################
AC_ARG_ENABLE([php],
#########################################################################################
               AS_HELP_STRING([--enable-php],[Enable PHP language support in ROSE (default=yes)]),
               ##########################################################################
                echo "$LANGUAGES_TO_SUPPORT" | grep --quiet "php"
                if test $? = 0 ; then 
                  list_has_php=yes
                fi
                case "$enableval" in
                  [yes)]
                  	if test "x$list_has_php" != "xyes" ; then
                          # --enable-languages does not include PHP, but --enable-php=yes
                  	  LANGUAGES_TO_SUPPORT+=" php"
                        fi
                  	;;
                  [no)]
                        # remove 'PHP' from support languages list
                  	LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | sed 's/php//g'`"
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-php='$enableval' is unsupported. Use 'yes' or 'no'])]
                 	;;
                esac
               ##########################################################################
               ,)
#########################################################################################
AC_ARG_ENABLE([opencl],
#########################################################################################
               AS_HELP_STRING([--enable-opencl],[Enable OpenCL language support in ROSE (default=yes)]),
               ##########################################################################
                echo "$LANGUAGES_TO_SUPPORT" | grep --quiet "opencl"
                if test $? = 0 ; then 
                  list_has_opencl=yes
                fi
                case "$enableval" in
                  [yes)]
                  	if test "x$list_has_opencl" != "xyes" ; then
                          # --enable-languages does not include OpenCL, but --enable-opencl=yes
                  	  LANGUAGES_TO_SUPPORT+=" opencl"
                        fi
                  	;;
                  [no)]
                        # remove 'OpenCL' from support languages list
                  	LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | sed 's/opencl//g'`"
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-opencl='$enableval' is unsupported. Use 'yes' or 'no'])]
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
