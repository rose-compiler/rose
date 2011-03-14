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
count_of_languages_to_support=0
for a_language in $LANGUAGES_TO_SUPPORT ; do

count_of_languages_to_support=`expr $count_of_languages_to_support + 1`
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
	  if test "x$GFORTRAN_PATH" = "x"; then
            AC_MSG_FAILURE([[[Fortran support]] gfortran not found: required for syntax checking and semantic analysis.
                           Do you need to explicitly specify gfortran using the "--with-gfortran=path/to/gfortran" configure-option? (See ./configure --help)])
          else
     	    support_fortran_language=yes
          fi
	else
	  AC_MSG_FAILURE([[[Fortran support]] Java Virtual Machine (JVM) not found: required by the Open Fortran Parser (OFP).
                         Do you need to explicitly specify Java (javac, JDk,...) using the "--with-java" configure-option? (See ./configure --help)])
	fi
	;;
java)
        if test "x$USE_JAVA" = x1; then
	  support_java_language=yes
        else
          AC_MSG_FAILURE([[[Java support]] Java dependencies not found: required for parser support in ROSE -- uses the  Eclipse Compiler for Java (ECJ).
                         Do you need to explicitly specify Java (javac, JDk,...) using the "--with-java" configure-option? (See ./configure --help)])
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

#########################################################################################
#
#  Enabled only one language  
#
#########################################################################################
if test $count_of_languages_to_support = 1 ; then
  support_only_one_language=yes
  AC_MSG_CHECKING([$LANGUAGES_TO_SUPPORT-only specific configurations])
  echo ""

  #
  # Only C
  #
  if test "x$support_c_language" = "xyes" ; then
    if test "x$support_cxx_language" = "xno" ; then
      support_cxx_language=yes
      LANGUAGES_TO_SUPPORT+=" c++"
      echo "[[C-only support:warning]] turning on C++ support (currently required)"
    fi
  fi

  #
  # Only C++
  #
  if test "x$support_cxx_language" = "xyes" ; then
    if test "x$support_c_language" = "xno" ; then
      support_c_language=yes
      LANGUAGES_TO_SUPPORT+=" c"
      echo "[[C++-only support:warning]] turning on C support (currently required)"
    fi
  fi

  #
  # Options for a minimal C/C++ configuration
  #
  if test "x$support_c_language" = "xyes" ; then
    with_haskell=no
    enable_binary_analysis_tests=no
    enable_projects_directory=no
    enable_tutorial_directory=no
  fi

  #
  # Only Java 
  #
  if test "x$support_java_language" = "xyes" ; then
    # When using fortran only assume that we are not interested in java language support in ROSE.
    # However, currently the --with-java option controls the use of java support for both Fortran
    # and Java language support. Now that we have added Java language support to ROSE this is
    # unintentionally confusing. So we can't turn this off since the Fortran support requires
    # internal java (JVM) support.
    # with_java=no

    with_haskell=no
    with_php=no
    enable_binary_analysis_tests=no

    # Allow tests directory to be run so that we can run the Fortran tests.
    # enable_tests_directory=no

    enable_projects_directory=no
    enable_tutorial_directory=no
  fi

  #
  # Only Fortran
  # requested by Rice University and LANL 
  #
  if test "x$support_fortran_language" = "xyes" ; then
    # Scott appears to require CPPFLAGS to be set...
    #debug#echo "Before setting CPPFLAGS: CPPFLAGS = $CPPFLAGS"
    CPPFLAGS="$CPPFLAGS $JAVA_JVM_INCLUDE"
    echo "[[Fortran-only support]] added JAVA_JVM_INCLUDE ($JAVA_JVM_INCLUDE) to CPPFLAGS = $CPPFLAGS"

    # DQ: I think that we have to express this option in terms of the "with_"
    # version of the macro instead of the "without_" version of the macro.
    # without_haskell=yes
    with_haskell=no

    # When using fortran only assume that we are not interested in java language support in ROSE.
    # However, currently the --with-java option controls the use of java support for both Fortran
    # and Java language support. Now that we have added Java language support to ROSE this is
    # unintentionally confusing. So we can't turn this off since the Fortran support requires
    # internal java (JVM) support.
    # with_java=no

    # So these should be expressed in terms of the "with" and "enable" versions of each option's macro.
    # without_php=yes
    with_php=no

    # disable_binary_analysis_tests=yes
    enable_binary_analysis_tests=no

    # Allow tests directory to be run so that we can run the Fortran tests.
    # enable_tests_directory=no

    enable_projects_directory=no
    enable_tutorial_directory=no

    # Test disabling a configure test that is on by default
    # I can't verify that the disable version of the variable effects the option.
    # Where as the enable version of the macro WILL control the setting of the option.
    # disable_language_only_restriction_test=yes
    # disable_language_only_restriction_test=no
    # enable_language_only_restriction_test=yes
    # enable_language_only_restriction_test=no

    # This allows testing this mechanism to set configure options from within the configure script...
    # enable_edg_version=4.5
  fi

  #
  # Only PHP 
  #
  if test "x$support_php_language" = "xyes" ; then
    with_haskell=no
    enable_binary_analysis_tests=no
    enable_projects_directory=no
    enable_tutorial_directory=no
  fi

  #
  # Only binary analysis 
  #
  if test "x$support_binaries" = "xyes" ; then
    with_haskell=no
    enable_binary_analysis_tests=yes
    enable_projects_directory=no
    enable_tutorial_directory=no
  fi


###
  # Output language-only configuration
  #
  #

  #
  # Haskell
  #
  if test "x$with_haskell" = "xyes" ; then
    echo "[[$LANGUAGES_TO_SUPPORT-only support]] with haskell"
  else
    echo "[[$LANGUAGES_TO_SUPPORT-only support]] without haskell"
  fi

  #
  # PHP
  #
  if test "x$with_php" = "xyes" ; then
    echo "[[$LANGUAGES_TO_SUPPORT-only support]] with PHP"
  else
    echo "[[$LANGUAGES_TO_SUPPORT-only support]] without PHP"
  fi

  #
  # Binary analysis tests
  #
  if test "x$enable_binary_analysis_tests" = "xyes" ; then
    echo "[[$LANGUAGES_TO_SUPPORT-only support]] enabling binary analysis tests"
  else
    echo "[[$LANGUAGES_TO_SUPPORT-only support]] disabling binary analysis tests"
  fi

  #
  # Projects/ directory 
  #
  if test "x$enable_projects_directory" = "xyes" ; then
    echo "[[$LANGUAGES_TO_SUPPORT-only support]] enabling ROSE/projects directory"
  else
    echo "[[$LANGUAGES_TO_SUPPORT-only support]] disabling ROSE/projects directory"
  fi

  #
  # Tutorial/ directory 
  #
  if test "x$enable_tutorial_directory" = "xyes" ; then
    echo "[[$LANGUAGES_TO_SUPPORT-only support]] enabling ROSE/tutorial directory"
  else
    echo "[[$LANGUAGES_TO_SUPPORT-only support]] disabling ROSE/tutorial directory"
  fi
#end-if $count_of_languages==1 (enable-only-language)
fi

#########################################################################################
#
# Language specific options 
#
#########################################################################################
AC_MSG_CHECKING([for language specific options to generate a minimal configuration of ROSE])



























# End macro ROSE_SUPPORT_LANGUAGES.
])
