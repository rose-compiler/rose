AC_DEFUN([ROSE_SUPPORT_LANGUAGE_CONFIG_OPTIONS],
[
#  TOO (3/10/2011):
#
#	1. Manage language support command-line options
#	2. Set flags to indicate which languages to support 
#	3. Output language support (debugging)
#	4. Enabled only one language
#	5. Set the automake conditional macros that will be used in Makefiles
#
#  DQ (4/15/2010): Added support to specify selected languages to support in ROSE.
#########################################################################################
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
#	--enable-python
#	--enable-opencl
#
#	TODO:
#	-Issue warning if user specifies conflicting language options, e.g.
#	         --enable-languages=binaries --disable-binaries
#
#########################################################################################

#########################################################################################
#
##
  #ALL_SUPPORTED_LANGUAGES="binaries c c++ cuda fortran java php python opencl"
   ALL_SUPPORTED_LANGUAGES="binaries c c++ cuda fortran java php        opencl"
##
#
#########################################################################################

#########################################################################################
#
##
   ROSE_SUPPORT_LANGUAGE_CONFIG_OPTIONS_DEPRECATED
##
#
#########################################################################################



#  TOO (3/18/2011): For now, the '--enable-only-LANGUAGE' options will take precedence over
#  all other language options. It might be useful, however, to add additional testing to
#  issue warnings/errors when the user mistakenly specifies multiple language options,
#  especially if they are conflicting.
if test "x$USER_GAVE_ENABLE_ONLY_LANGUAGE_CONFIG_OPTION" = "xno" ; then
#########################################################################################
AC_ARG_ENABLE([languages],
#########################################################################################
               AS_HELP_STRING([--enable-languages=LIST],[Build specific languages: all,none,binary-analysis,c,c++,cuda,fortran,java,opencl,php,python (default=all)]),,
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
                  	[AC_MSG_FAILURE([--enable-binary-analysis='$enableval' is not supported. Use 'yes' or 'no'])]
                 	;;
                esac
               ##########################################################################
               ,)
#########################################################################################
AC_ARG_ENABLE([c],
#########################################################################################
               AS_HELP_STRING([--enable-c],[Enable C language support in ROSE (default=yes). Note:  C++ support must currently be simultaneously enabled/disabled]),
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
                        list_has_cxx="`echo $LANGUAGES_TO_SUPPORT | [awk '{for (i=1; i<=NF; i++) { if ($i == "c++") { printf "yes"; } } }']`"
                        if test "x$list_has_cxx" = "xyes" && test "x$enable_cxx" != "xno" ; then
                  	  [AC_MSG_FAILURE([Can't disable 'C' language support because 'C++' language support is enabled -- currently both are required to be supported together. If you really don't want 'C' language support, please also disable 'C++' language support (see ./configure --help)])]
                        else
                          # remove 'C' from support languages list
                          # TOO (3/11/2011): couldn't find a nice way to handle with sed, cases: "c", "c c++", ...
                  	  LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | [awk '{for (i=1; i<=NF; i++) { if ($i != "c") { printf "%s ",$i; } } }']`" 
                        fi
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-c='$enableval' is not supported. Use 'yes' or 'no'])]
                 	;;
                esac
               ##########################################################################
               ,)
#########################################################################################
AC_ARG_ENABLE([cxx],
#########################################################################################
               AS_HELP_STRING([--enable-cxx],[Enable C++ language support in ROSE (default=yes). Note: C support must currently be simultaneously enabled/disabled]),
               ##########################################################################
                case "$enableval" in
                  [yes)]
                  	if test "x$list_has_cxx" != "xyes" ; then
                          # --enable-languages does not include C++, but --enable-cxx=yes
                  	  LANGUAGES_TO_SUPPORT+=" c++"
                        fi
                  	;;
                  [no)]
                        list_has_c="`echo $LANGUAGES_TO_SUPPORT | [awk '{for (i=1; i<=NF; i++) { if ($i == "c") { printf "yes"; } } }']`"
                        if test "x$list_has_c" = "xyes" && test "x$enable_c" != "xno" ; then
                  	  [AC_MSG_FAILURE([Can't disable 'C++' language support because 'C' language support is enabled -- currently both are required to be supported together. If you really don't want 'C++' language support, please also disable 'C' language support (see ./configure --help)])]
                        else
                          # remove 'C++' from support languages list
                  	  LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | sed 's/c++//g'`"
                        fi
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-cxx='$enableval' is not supported. Use 'yes' or 'no'])]
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
                  	[AC_MSG_FAILURE([--enable-cuda='$enableval' is not supported. Use 'yes' or 'no'])]
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
                        if test "x$with_java" = "xno" ; then
                          [AC_MSG_FAILURE([[[Fortran Support]] you specified conflicting configure flags: '--enable-fortran=$enableval' enables Fortran-language support, which requires Java, and '--with-java=$with_java' disables Java])]
                        fi
                        if test "x$USE_JAVA" = "x0" ; then
                          [AC_MSG_FAILURE([[[Fortran Support]] you requested to build Fortran language support with '--enable-fortran=$enableval', which requires Java, but Java was not found. Do you need to explicitly specify your Java using the "--with-java" configure-option? (See ./configure --help)])]
                        fi

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
                  	[AC_MSG_FAILURE([--enable-fortran='$enableval' is not supported. Use 'yes' or 'no'])]
                 	;;
                esac
               ##########################################################################
               ,
                if test "x$with_java" = "xno" ; then
                  enable_fortran=no
                  LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | sed 's/fortran//g'`"
                  [echo "[[Fortran support]] disabling Fortran language support, which requires Java, because you specified --with-java='$with_java'"] 
                fi)
#########################################################################################
AC_ARG_ENABLE([java],
#########################################################################################
               AS_HELP_STRING([--enable-java],[Enable Java language support in ROSE (default=yes). Note: --without-java turns off support for ALL components in ROSE that depend on Java, including Java language support]),
               ##########################################################################
                echo "$LANGUAGES_TO_SUPPORT" | grep --quiet "java"
                if test $? = 0 ; then 
                  list_has_java=yes
                fi
                case "$enableval" in
                  [yes)]
                        if test "x$with_java" = "xno" ; then
                          [AC_MSG_FAILURE([[[Java Support]] you specified conflicting configure flags: '--enable-java=$enableval' enables Java-language support, but '--with-java=$with_java' disables it])]
                        fi
                        if test "x$USE_JAVA" = "x0" ; then
                          [AC_MSG_FAILURE([[[Java Support]] you requested to build Java language support with '--enable-java=$enableval', which requires Java, but Java was not found. Do you need to explicitly specify your Java using the "--with-java" configure-option? (See ./configure --help)])]
                        fi

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
                  	[AC_MSG_FAILURE([--enable-java='$enableval' is not supported. Use 'yes' or 'no'])]
                 	;;
                esac
               ##########################################################################
               ,
                if test "x$with_java" = "xno" ; then
                  enable_java=no
                  LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | sed 's/java//g'`"
                  [echo "[[Java support]] disabling Java language support, which requires Java, because you specified --with-java='$with_java'"] 
                fi)
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
                  	[AC_MSG_FAILURE([--enable-php='$enableval' is not supported. Use 'yes' or 'no'])]
                 	;;
                esac
               ##########################################################################
               ,)
#########################################################################################
AC_ARG_ENABLE([python],
#########################################################################################
               AS_HELP_STRING([--enable-python],[Enable Python language support in ROSE (default=no)]),
               ##########################################################################
                echo "$LANGUAGES_TO_SUPPORT" | grep --quiet "python"
                if test $? = 0 ; then 
                  list_has_python=yes
                fi
                case "$enableval" in
                  [yes)]
                  	if test "x$list_has_python" != "xyes" ; then
                          # --enable-languages does not include python, but --enable-python=yes
                  	  LANGUAGES_TO_SUPPORT+=" python"
                        fi
                  	;;
                  [no)]
                        # remove 'python' from support languages list
                  	LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | sed 's/python//g'`"
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-python='$enableval' is not supported. Use 'yes' or 'no'])]
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
                  	[AC_MSG_FAILURE([--enable-opencl='$enableval' is not supported. Use 'yes' or 'no'])]
                 	;;
                esac
               ##########################################################################
               ,)
else
  echo "[[enable-only-$LANGUAGES_TO_SUPPORT support:warning]] ignoring any other language-support configuration options."
fi #end-if test "x$USER_GAVE_ENABLE_ONLY_LANGUAGE_CONFIG_OPTION" = "xno" ;


#
# C and C++ are currently required to be supported simultaneously 
#
list_has_c="`echo $LANGUAGES_TO_SUPPORT | [awk '{for (i=1; i<=NF; i++) { if ($i == "c") { printf "yes"; } } }']`"
list_has_cxx="`echo $LANGUAGES_TO_SUPPORT | [awk '{for (i=1; i<=NF; i++) { if ($i == "c++") { printf "yes"; } } }']`"

if test "x$list_has_c" = "xyes" && test "x$list_has_cxx" != "xyes"; then
  LANGUAGES_TO_SUPPORT+=" c++"
  echo "[[C language support:warning]] turning on C++ support (currently required)"
fi

if test "x$list_has_cxx" = "xyes" && test "x$list_has_c" != "xyes"; then
  LANGUAGES_TO_SUPPORT+=" c"
  echo "[[C++-only support:warning]] turning on C support (currently required)"
fi

#########################################################################################
#
#  Set flags to indicate which languages to support according to the
#  user specified command-line options; including macros that will be
#  contained in BUILD_TREE/rose_config.h 
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
	support_python_language=no
	support_opencl_language=no
	AC_MSG_WARN([you did not enable any language support])
	;;
binaries)
	support_binaries=yes
	AC_DEFINE([ROSE_BUILD_BINARY_ANALYSIS_SUPPORT], [], [Build ROSE to support the Binary Analysis])
	;;
c)
	support_c_language=yes
	AC_DEFINE([ROSE_BUILD_C_LANGUAGE_SUPPORT], [], [Build ROSE to support the C langauge])
	;;
c++)
	support_cxx_language=yes
	AC_DEFINE([ROSE_BUILD_CXX_LANGUAGE_SUPPORT], [], [Build ROSE to support the C++ langauge])
	;;
cuda)
	support_cuda_language=yes
	AC_DEFINE([ROSE_BUILD_CUDA_LANGUAGE_SUPPORT], [], [Build ROSE to support the CUDA langauge])
	;;
fortran)
	if test "x$USE_JAVA" = x1; then
	  if test "x$GFORTRAN_PATH" = "x"; then
            AC_MSG_FAILURE([[[Fortran support]] gfortran not found: required for syntax checking and semantic analysis.
                           Do you need to explicitly specify gfortran using the "--with-gfortran=path/to/gfortran" configure-option? (See ./configure --help)])
          else
     	    support_fortran_language=yes
	    AC_DEFINE([ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT], [], [Build ROSE to support the Fortran langauge])
          fi
        elif test "x$with_java" = "xno" ; then
	  AC_MSG_FAILURE([[[Fortran support]] can't support the Fortran language because you specified --with-java='$with_java'. You can turn off Fortran support with --disable-fortran (See ./configure --help)]) 
	else
	  AC_MSG_FAILURE([[[Fortran support]] Java Virtual Machine (JVM) not found: required by the Open Fortran Parser (OFP).
                         Do you need to explicitly specify Java using the "--with-java" configure-option? (See ./configure --help)])
	fi
	;;
java)
        if test "x$USE_JAVA" = x1; then
	  support_java_language=yes
	  AC_DEFINE([ROSE_BUILD_JAVA_LANGUAGE_SUPPORT], [], [Build ROSE to support the Java langauge])
        else
          AC_MSG_FAILURE([[[Java support]] Java dependencies not found: required for parser support in ROSE -- uses the  Eclipse Compiler for Java (ECJ).
                         Do you need to explicitly specify Java (javac, JDk,...) using the "--with-java" configure-option? (See ./configure --help)])
        fi
	;;
php)
	support_php_language=yes
	AC_DEFINE([ROSE_BUILD_PHP_LANGUAGE_SUPPORT], [], [Build ROSE to support the PHP langauge])
	;;
python)
	support_python_language=yes
	AC_DEFINE([ROSE_BUILD_PYTHON_LANGUAGE_SUPPORT], [], [Build ROSE to support the Python langauge])
	;;
opencl)
	support_opencl_language=yes
	AC_DEFINE([ROSE_BUILD_OPENCL_LANGUAGE_SUPPORT], [], [Build ROSE to support the OpenCL langauge])
	;;
*)
	AC_MSG_FAILURE([unrecognized language '$a_language'])
	;;
esac
done


#########################################################################################
#
#  Output language support 
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
#########################################################################################
#
#  Enabled only one language: set specific configurations for minimal build of ROSE 
#
#########################################################################################
if test $count_of_languages_to_support = 2 ; then
  #
  # Only C/C++ (currently required to be supported simultaneously) 
  #
  if test "x$support_c_language" = "xyes" && test "x$support_cxx_language" = "xyes" ; then
    with_haskell=no
    enable_binary_analysis_tests=no
    enable_projects_directory=no
    enable_tutorial_directory=no
  fi
elif test $count_of_languages_to_support = 1 ; then
  support_only_one_language=yes
  AC_MSG_CHECKING([$LANGUAGES_TO_SUPPORT-only specific configurations])
  echo ""

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
    with_python=no
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
    echo "[[Fortran-only support]] added JAVA_JVM_INCLUDE ($JAVA_JVM_INCLUDE) to CPPFLAGS ($CPPFLAGS)"

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
    with_python=no

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
  # Only Python
  #
  if test "x$support_python_language" = "xyes" ; then
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
    enable_projects_directory=no
    enable_tutorial_directory=no
  fi

#AC_MSG_RESULT([done])
#end-if $count_of_languages==1 (enable-only-language)

###
  # Output language-only configuration
  #
  #

  #
  # Haskell
  #
  if test "x$with_haskell" = "xyes" ; then
    echo "[[$LANGUAGES_TO_SUPPORT-only support]] with haskell"
  elif test "x$with_haskell" = "xno" ; then
    echo "[[$LANGUAGES_TO_SUPPORT-only support]] without haskell"
  fi

  #
  # PHP
  #
  if test "x$with_php" = "xyes" ; then
    echo "[[$LANGUAGES_TO_SUPPORT-only support]] with PHP"
  elif test "x$with_php" = "xno" ; then
    echo "[[$LANGUAGES_TO_SUPPORT-only support]] without PHP"
  fi

  #
  # Python
  #
  if test "x$with_python" = "xyes" ; then
    echo "[[$LANGUAGES_TO_SUPPORT-only support]] with Python"
  elif test "x$with_python" = "xno" ; then
    echo "[[$LANGUAGES_TO_SUPPORT-only support]] without Python"
  fi

  #
  # Binary analysis tests
  #
  if test "x$enable_binary_analysis_tests" = "xyes" ; then
    echo "[[$LANGUAGES_TO_SUPPORT-only support]] enabling binary analysis tests"
  elif test "x$enable_binary_analysis_tests" = "xno" ; then
    echo "[[$LANGUAGES_TO_SUPPORT-only support]] disabling binary analysis tests"
  fi

  #
  # Projects/ directory 
  #
  if test "x$enable_projects_directory" = "xyes" ; then
    echo "[[$LANGUAGES_TO_SUPPORT-only support]] enabling ROSE/projects directory"
  elif test "x$enable_projects_directory" = "xno" ; then
    echo "[[$LANGUAGES_TO_SUPPORT-only support]] disabling ROSE/projects directory"
  fi

  #
  # Tutorial/ directory 
  #
  if test "x$enable_tutorial_directory" = "xyes" ; then
    echo "[[$LANGUAGES_TO_SUPPORT-only support]] enabling ROSE/tutorial directory"
  elif test "x$enable_tutorial_directory" = "xno" ; then
    echo "[[$LANGUAGES_TO_SUPPORT-only support]] disabling ROSE/tutorial directory"
  fi
fi
#########################################################################################
#
# TOO (3/14/2011): not used anymore?
#
#########################################################################################
#AC_ARG_ENABLE([language-only-restriction-test],AS_HELP_STRING([--enable-language-only-restriction-test],[Support language only restriction test]),[],[])
#if test "x$enableval" = "xyes"; then
#   echo "Setting: language-only-restriction-test option IS yes"
#else
#   echo "Setting: language-only-restriction-test option is NOT yes"
#fi
#if test "x$enable_language_only_restriction_test" = "xyes"; then
#   echo "Value of: language-only-restriction-test option macro IS yes"
#else
#   echo "Value of: language-only-restriction-test option macro is NOT yes"
#fi
#echo "enable_languages = $enable_languages"
#########################################################################################
#
# Set the automake conditional macros that will be used in Makefiles.
#
#########################################################################################
echo -n "Creating Automake conditional flags for language support in Makefiles... "
AM_CONDITIONAL(ROSE_BUILD_C_LANGUAGE_SUPPORT, [test "x$support_c_language" = xyes])
AM_CONDITIONAL(ROSE_BUILD_CXX_LANGUAGE_SUPPORT, [test "x$support_cxx_language" = xyes])
AM_CONDITIONAL(ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT, [test "x$support_fortran_language" = xyes])
AM_CONDITIONAL(ROSE_BUILD_JAVA_LANGUAGE_SUPPORT, [test "x$support_java_language" = xyes])
AM_CONDITIONAL(ROSE_BUILD_PHP_LANGUAGE_SUPPORT, [test "x$support_php_language" = xyes])
AM_CONDITIONAL(ROSE_BUILD_PYTHON_LANGUAGE_SUPPORT, [test "x$support_python_language" = xyes])
AM_CONDITIONAL(ROSE_BUILD_BINARY_ANALYSIS_SUPPORT, [test "x$support_binaries" = xyes])
AM_CONDITIONAL(ROSE_BUILD_CUDA_LANGUAGE_SUPPORT, [test "x$support_cuda_language" = xyes])
AM_CONDITIONAL(ROSE_BUILD_OPENCL_LANGUAGE_SUPPORT, [test "x$support_opencl_language" = xyes])
echo "done"

echo ""
print_isLanguageSupported "Binary analysis" "$support_binaries"
print_isLanguageSupported "C" "$support_c_language"
print_isLanguageSupported "C++" "$support_cxx_language"
print_isLanguageSupported "Cuda" "$support_cuda_language"
print_isLanguageSupported "Fortran" "$support_fortran_language"
print_isLanguageSupported "Java" "$support_java_language"
print_isLanguageSupported "PHP" "$support_php_language"
print_isLanguageSupported "Python" "$support_python_language"
print_isLanguageSupported "OpenCL" "$support_opencl_language"
echo ""
echo "(+)enabled (-)disabled"
#AC_MSG_RESULT($LANGUAGES_TO_SUPPORT)
echo "------------------------------------------------"

echo "Finished configuring user-specified languages to support"
# End macro ROSE_SUPPORT_LANGUAGES.
])






AC_DEFUN([ROSE_SUPPORT_LANGUAGE_CONFIG_OPTIONS_DEPRECATED],
[
#########################################################################################
#
#  TOO (3/18/2011):
#
#  Out variables:
#  USER_GAVE_ENABLE_ONLY_LANGUAGE_CONFIG_OPTION(=yes/no)
#
#########################################################################################

echo "------------------------------------------------"
AC_MSG_CHECKING([for deprecated language configuration options])


USER_GAVE_ENABLE_ONLY_LANGUAGE_CONFIG_OPTION=no
#########################################################################################
AC_ARG_ENABLE([only-binary-analysis],
#########################################################################################
              AS_HELP_STRING([--enable-only-binary-analysis(=yes)],
                             [Enable ONLY Java support in ROSE (Warning: '--enable-only-binary-analysis=no' and '--disable-only-binary-analysis' are no longer supported)]),
               ##########################################################################
                case "$enableval" in
                  [yes)]
                  	LANGUAGES_TO_SUPPORT="binaries"
                        USER_GAVE_ENABLE_ONLY_LANGUAGE_CONFIG_OPTION=yes
                  	;;
                  [no)]
                  	[AC_MSG_FAILURE([--enable-only-binary-analysis='$enableval' and --disable-only-binary-analysis are no longer supported. Use '--disable-binary-analysis' (see ./configure --help)])]
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-only-binary-analysis='$enableval' is not supported. Use '--enable-only-binary-analysis(=yes)' (see ./configure --help)])]
                 	;;
                esac
               ##########################################################################
               ,)

#########################################################################################
AC_ARG_ENABLE([only-c],
#########################################################################################
              AS_HELP_STRING([--enable-only-c(=yes)],
                             [Enable ONLY C support in ROSE (Warning: '--enable-only-c=no' and '--disable-only-c' are no longer supported)]),
               ##########################################################################
                case "$enableval" in
                  [yes)]
                  	LANGUAGES_TO_SUPPORT="c"
                        USER_GAVE_ENABLE_ONLY_LANGUAGE_CONFIG_OPTION=yes
                  	;;
                  [no)]
                  	[AC_MSG_FAILURE([--enable-only-c='$enableval' and --disable-only-c are no longer supported. Use '--disable-c' (see ./configure --help)])]
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-only-c='$enableval' is not supported. Use '--enable-only-c(=yes)' (see ./configure --help)])]
                 	;;
                esac
               ##########################################################################
               ,)

#########################################################################################
AC_ARG_ENABLE([only-cxx],
#########################################################################################
              AS_HELP_STRING([--enable-only-cxx(=yes)],
                             [Enable ONLY C++ support in ROSE (Warning: '--enable-only-cxx=no' and '--disable-only-cxx' are no longer supported)]),
               ##########################################################################
                case "$enableval" in
                  [yes)]
                  	LANGUAGES_TO_SUPPORT="c++"
                        USER_GAVE_ENABLE_ONLY_LANGUAGE_CONFIG_OPTION=yes
                  	;;
                  [no)]
                  	[AC_MSG_FAILURE([--enable-only-cxx='$enableval' and --disable-only-cxx are no longer supported. Use '--disable-cxx' (see ./configure --help)])]
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-only-cxx='$enableval' is not supported. Use '--enable-only-cxx(=yes)' (see ./configure --help)])]
                 	;;
                esac
               ##########################################################################
               ,)

#########################################################################################
AC_ARG_ENABLE([only-fortran],
#########################################################################################
              AS_HELP_STRING([--enable-only-fortran(=yes)],
                             [Enable ONLY Fortran support in ROSE (Warning: '--enable-only-fortran=no' and '--disable-only-fortran' are no longer supported)]),
               ##########################################################################
                case "$enableval" in
                  [yes)]
                        if test "x$with_java" = "xno" ; then
                          [AC_MSG_FAILURE([[[Fortran Support]] you specified conflicting configure flags: '--enable-only-fortran=$enableval' enables Fortran-language support, which requires Java support, and '--with-java=$with_java' disables Java support])]
                        else  
                  	  LANGUAGES_TO_SUPPORT="fortran"
                          USER_GAVE_ENABLE_ONLY_LANGUAGE_CONFIG_OPTION=yes
                        fi
                  	;;
                  [no)]
                  	[AC_MSG_FAILURE([--enable-only-fortran='$enableval' and --disable-only-fortran are no longer supported. Use '--disable-fortran' (see ./configure --help)])]
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-only-fortran='$enableval' is not supported. Use '--enable-only-fortran(=yes)' (see ./configure --help)])]
                 	;;
                esac
               ##########################################################################
               ,)

#########################################################################################
AC_ARG_ENABLE([only-java],
#########################################################################################
              AS_HELP_STRING([--enable-only-java(=yes)],
                             [Enable ONLY Java support in ROSE (Warning: '--enable-only-java=no' and '--disable-only-java' are no longer supported)]),
               ##########################################################################
                case "$enableval" in
                  [yes)]
                        if test "x$with_java" = "xno" ; then
                          [AC_MSG_FAILURE([[[Java Support]] you specified conflicting configure flags: '--enable-only-java=$enableval' enables Java-language support, but '--with-java=$with_java' disables it])]
                        else  
                  	  LANGUAGES_TO_SUPPORT="java"
                          USER_GAVE_ENABLE_ONLY_LANGUAGE_CONFIG_OPTION=yes
                        fi
                  	;;
                  [no)]
                  	[AC_MSG_FAILURE([--enable-only-java='$enableval' and --disable-only-java are no longer supported. Use '--disable-java' (see ./configure --help)])]
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-only-java='$enableval' is not supported. Use '--enable-only-java(=yes)' (see ./configure --help)])]
                 	;;
                esac
               ##########################################################################
               ,)

#########################################################################################
AC_ARG_ENABLE([only-php],
#########################################################################################
              AS_HELP_STRING([--enable-only-php(=yes)],
                             [Enable ONLY PHP support in ROSE (Warning: '--enable-only-php=no' and '--disable-only-php' are no longer supported)]),
               ##########################################################################
                case "$enableval" in
                  [yes)]
                  	LANGUAGES_TO_SUPPORT="php"
                        USER_GAVE_ENABLE_ONLY_LANGUAGE_CONFIG_OPTION=yes
                  	;;
                  [no)]
                  	[AC_MSG_FAILURE([--enable-only-php='$enableval' and --disable-only-php are no longer supported. Use '--disable-php' (see ./configure --help)])]
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-only-php='$enableval' is not supported. Use '--enable-only-php(=yes)' (see ./configure --help)])]
                 	;;
                esac
               ##########################################################################
               ,)

#########################################################################################
AC_ARG_ENABLE([only-python],
#########################################################################################
              AS_HELP_STRING([--enable-only-python(=yes)],
                             [Enable ONLY Python support in ROSE (Warning: '--enable-only-python=no' and '--disable-only-python' are no longer supported)]),
               ##########################################################################
                case "$enableval" in
                  [yes)]
                  	LANGUAGES_TO_SUPPORT="python"
                        USER_GAVE_ENABLE_ONLY_LANGUAGE_CONFIG_OPTION=yes
                  	;;
                  [no)]
                  	[AC_MSG_FAILURE([--enable-only-python='$enableval' and --disable-only-python are no longer supported. Use '--disable-python' (see ./configure --help)])]
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-only-python='$enableval' is not supported. Use '--enable-only-python(=yes)' (see ./configure --help)])]
                 	;;
                esac
               ##########################################################################
               ,)

#########################################################################################
AC_ARG_ENABLE([only-cuda],
#########################################################################################
              AS_HELP_STRING([--enable-only-cuda(=yes)],
                             [Enable ONLY Cuda support in ROSE (Warning: '--enable-only-cuda=no' and '--disable-only-cuda' are no longer supported)]),
               ##########################################################################
                case "$enableval" in
                  [yes)]
                  	LANGUAGES_TO_SUPPORT="cuda"
                        USER_GAVE_ENABLE_ONLY_LANGUAGE_CONFIG_OPTION=yes
                  	;;
                  [no)]
                  	[AC_MSG_FAILURE([--enable-only-cuda='$enableval' and --disable-only-cuda are no longer supported. Use '--disable-cuda' (see ./configure --help)])]
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-only-cuda='$enableval' is not supported. Use '--enable-only-cuda(=yes)' (see ./configure --help)])]
                 	;;
                esac
               ##########################################################################
               ,)

#########################################################################################
AC_ARG_ENABLE([only-opencl],
#########################################################################################
              AS_HELP_STRING([--enable-only-opencl(=yes)],
                             [Enable ONLY OpenCL support in ROSE (Warning: '--enable-only-opencl=no' and '--disable-only-opencl' are no longer supported)]),
               ##########################################################################
                case "$enableval" in
                  [yes)]
                  	LANGUAGES_TO_SUPPORT="opencl"
                        USER_GAVE_ENABLE_ONLY_LANGUAGE_CONFIG_OPTION=yes
                  	;;
                  [no)]
                  	[AC_MSG_FAILURE([--enable-only-opencl='$enableval' and --disable-only-opencl are no longer supported. Use '--disable-opencl' (see ./configure --help)])]
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-only-opencl='$enableval' is not supported. Use '--enable-only-opencl(=yes)' (see ./configure --help)])]
                 	;;
                esac
               ##########################################################################
               ,)

AC_MSG_RESULT([done])
#echo "Finished configuring deprecated user-specified language configuration options"
# End macro ROSE_SUPPORT_LANGUAGE_CONFIG_OPTIONS_DEPRECATED
])
