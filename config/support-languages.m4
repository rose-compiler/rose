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
ROSE_CONFIGURE_SECTION([Checking analyzable languages])

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
  ALL_SUPPORTED_LANGUAGES="binaries c c++ cuda fortran java php         opencl"
  #ALL_SUPPORTED_LANGUAGES="binaries c c++ cuda fortran java php        opencl"
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
AC_ARG_ENABLE([languages],
               AS_HELP_STRING([--enable-languages=LIST],[Build specific languages: all,none,binaries,c,c++,cuda,fortran,java,opencl,php,python (default=all)]),,
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

AC_ARG_ENABLE([binary-analysis],
               AS_HELP_STRING([--enable-binary-analysis],[Enable binary analysis support in ROSE (default=yes)]),
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
                  	[AC_MSG_FAILURE([--enable-binary-analysis='$enableval' is not supported; use 'yes' or 'no'])]
                 	;;
                esac
               ,)
AC_ARG_ENABLE([c],
               AS_HELP_STRING([--enable-c],[Enable C language support in ROSE (default=yes). Note:  C++ support must currently be simultaneously enabled/disabled]),
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
                  	  [AC_MSG_FAILURE([cannot disable 'C' language support because 'C++' language support is enabled -- currently both are required to be supported together. If you really don't want 'C' language support, please also disable 'C++' language support (see ./configure --help)])]
                        else
                          # remove 'C' from support languages list
                          # TOO (3/11/2011): couldn't find a nice way to handle with sed, cases: "c", "c c++", ...
                  	  LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | [awk '{for (i=1; i<=NF; i++) { if ($i != "c") { printf "%s ",$i; } } }']`" 
                        fi
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-c="$enableval" is not supported; use "yes" or "no"])]
                 	;;
                esac
               ,)
AC_ARG_ENABLE([cxx],
               AS_HELP_STRING([--enable-cxx],[Enable C++ language support in ROSE (default=yes). Note: C support must currently be simultaneously enabled/disabled]),
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
                  	  [AC_MSG_FAILURE([cannot disable C++ language support because C language support is enabled -- currently both are required to be supported together. If you really don't want C++ language support, please also disable C language support (see ./configure --help)])]
                        else
                          # remove 'C++' from support languages list
                  	  LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | sed 's/c++//g'`"
                        fi
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-cxx="$enableval" is not supported; use "yes" or "no"])]
                 	;;
                esac
               ,)
AC_ARG_ENABLE([cuda],
               AS_HELP_STRING([--enable-cuda],[Enable Cuda language support in ROSE (default=yes)]),
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
                  	[AC_MSG_FAILURE([--enable-cuda="$enableval" is not supported; use "yes" or "no"])]
                 	;;
                esac
               ,)
AC_ARG_ENABLE([fortran],
               AS_HELP_STRING([--enable-fortran],[Enable Fortran language support in ROSE (default=yes)]),
                echo "$LANGUAGES_TO_SUPPORT" | grep --quiet "fortran"
                if test $? = 0 ; then 
                  list_has_fortran=yes
                fi
                case "$enableval" in
                  [yes)]
                        if test "x$with_java" = "xno" ; then
                          [AC_MSG_FAILURE([[[Fortran Support]] you specified conflicting configure flags: --enable-fortran="$enableval" enables Fortran-language support, which requires Java, and --with-java="$with_java" disables Java])]
                        fi
                        if test "x$USE_JAVA" = "x0" ; then
                          [AC_MSG_FAILURE([[[Fortran Support]] you requested to build Fortran language support with --enable-fortran="$enableval", which requires Java, but Java was not found. Do you need to explicitly specify your Java using the --with-java configure-switch? (See ./configure --help)])]
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
                  	[AC_MSG_FAILURE([--enable-fortran="$enableval" is not supported; use "yes" or "no"])]
                 	;;
                esac
               ,
                if test "x$with_java" = "xno" ; then
		  if test "$enable_fortran" != no; then
                    enable_fortran=no
                    LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | sed 's/fortran//g'`"
                    [echo "[[Fortran support]] disabling Fortran language support, which requires Java, because you specified --with-java='$with_java'"]
		  fi
                fi)
AC_ARG_ENABLE([java],
               AS_HELP_STRING([--enable-java],[Enable Java language support in ROSE (default=yes). Note: --without-java turns off support for ALL components in ROSE that depend on Java, including Java language support]),
                echo "$LANGUAGES_TO_SUPPORT" | grep --quiet "java"
                if test $? = 0 ; then 
                  list_has_java=yes
                fi
                case "$enableval" in
                  [yes)]
                        if test "x$with_java" = "xno" ; then
                          [AC_MSG_FAILURE([[[Java Support]] you specified conflicting configure flags: --enable-java="$enableval" enables Java-language support, but --with-java="$with_java" disables it])]
                        fi
                        if test "x$USE_JAVA" = "x0" ; then
                          [AC_MSG_FAILURE([[[Java Support]] you requested to build Java language support with --enable-java="$enableval", which requires Java, but Java was not found. Do you need to explicitly specify your Java using the --with-java configure-option? (See ./configure --help)])]
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
                  	[AC_MSG_FAILURE([--enable-java="$enableval" is not supported; use "yes" or "no"])]
                 	;;
                esac
               ,
                if test "x$with_java" = "xno" ; then
		  if test "$enable_java" != no; then
                    enable_java=no
                    LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | sed 's/java//g'`"
                    [echo "[[Java support]] disabling Java language support, which requires Java, because you specified --with-java='$with_java'"]
		  fi
                fi)
AC_ARG_ENABLE([php],
               AS_HELP_STRING([--enable-php],[Enable PHP language support in ROSE (default=yes)]),
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
                  	[AC_MSG_FAILURE([--enable-php="$enableval" is not supported; use "yes" or "no"])]
                 	;;
                esac
               ,)
AC_ARG_ENABLE([python],
               AS_HELP_STRING([--enable-python],[Enable Python language support in ROSE (default=no)]),
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
                    # Fail if --with-python is specified
                    if test "x$with_python" != "x"; then
                        AC_MSG_FAILURE([cannot mix flags --with-python and --enable-python=no])
                    fi

                    # remove 'python' from support languages list
                    LANGUAGES_TO_SUPPORT="`echo $LANGUAGES_TO_SUPPORT | sed 's/python//g'`"
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-python="$enableval" is not supported; use "yes" or "no"])]
                 	;;
                esac
               ,

                dnl  Warn if --with-python is specified and --enable-python is not specified. The "--with-python=PATH"
		dnl  switch is used to specify a non-standard location for the Python interpreter, while the "--enable-python"
		dnl  switch causes librose to contain a python interpreter as part of its frontend.  Do not automatically
		dnl  turn on "--enable-python" because then there's no way for users to specify the name of the python
		dnl  interpreter (which might be needed by some ROSE projects) without also enabling the python parser
		dnl  in ROSE's frontend (which has stricter version requirements and might not compile).
                if test "$with_python" != "" -a "$with_python" != "no"; then
                    AC_MSG_WARN([--with-python specified without --enable-python; ROSE will not have a python frontend parser])
                fi
               )
AC_ARG_ENABLE([opencl],
               AS_HELP_STRING([--enable-opencl],[Enable OpenCL language support in ROSE (default=yes)]),
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
                  	[AC_MSG_FAILURE([--enable-opencl="$enableval" is not supported; use "yes" or "no"])]
                 	;;
                esac
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
	support_binaries_frontend=no
	support_c_frontend=no
	support_cxx_frontend=no
	support_cuda_frontend=no
	support_fortran_frontend=no
	support_java_frontend=no
	support_php_frontend=no
	support_python_frontend=no
	support_opencl_frontend=no
        with_haskell=no
        enable_binary_analysis_tests=no
        enable_projects_directory=no
        enable_tutorial_directory=no
	AC_MSG_WARN([you did not enable any language support])
	;;
binaries)
	support_binaries_frontend=yes
	AC_DEFINE([ROSE_BUILD_BINARY_ANALYSIS_SUPPORT], [], [Build ROSE to support the Binary Analysis])
	;;
c)
	support_c_frontend=yes
	AC_DEFINE([ROSE_BUILD_C_LANGUAGE_SUPPORT], [], [Build ROSE to support the C langauge])
	;;
c++)
	support_cxx_frontend=yes
	AC_DEFINE([ROSE_BUILD_CXX_LANGUAGE_SUPPORT], [], [Build ROSE to support the C++ langauge])
	;;
cuda)
	support_cuda_frontend=yes
	AC_DEFINE([ROSE_BUILD_CUDA_LANGUAGE_SUPPORT], [], [Build ROSE to support the CUDA langauge])
        GENERATE_CUDA_SPECIFIC_HEADERS
	;;
fortran)
	if test "x$USE_JAVA" = x1; then
	  if test "x$GFORTRAN_PATH" = "x" -o "x$GFORTRAN_PATH" = "xno"; then
            AC_MSG_FAILURE([[[Fortran support]] gfortran not found: required for syntax checking and semantic analysis.
                           Do you need to explicitly specify gfortran using the --with-gfortran=path/to/gfortran configure-switch? (See ./configure --help)])
          else
     	    support_fortran_frontend=yes
	    AC_DEFINE([ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT], [], [Build ROSE to support the Fortran langauge])
          fi
        elif test "x$with_java" = "xno" ; then
	  AC_MSG_FAILURE([[[Fortran support]] cannot support the Fortran language because you specified --with-java="$with_java". You can turn off Fortran support with --disable-fortran (See ./configure --help)]) 
	else
	  AC_MSG_FAILURE([[[Fortran support]] Java Virtual Machine (JVM) not found: required by the Open Fortran Parser (OFP).
	                 Do you need to explicitly specify Java using the --with-java configure-switch? (See ./configure --help)])
	fi
	;;
java)
        if test "x$USE_JAVA" = x1; then
	  support_java_frontend=yes
	  AC_DEFINE([ROSE_BUILD_JAVA_LANGUAGE_SUPPORT], [], [Build ROSE to support the Java langauge])
        else
          AC_MSG_FAILURE([[[Java support]] Java dependencies not found: required for parser support in ROSE -- uses the  Eclipse Compiler for Java (ECJ).
                         Do you need to explicitly specify Java (javac, JDk,...) using the --with-java configure-switch? (See ./configure --help)])
        fi
	;;
php)
	support_php_frontend=yes
	AC_DEFINE([ROSE_BUILD_PHP_LANGUAGE_SUPPORT], [], [Build ROSE to support the PHP langauge])
	;;
python)
	support_python_frontend=yes
	AC_DEFINE([ROSE_BUILD_PYTHON_LANGUAGE_SUPPORT], [], [Build ROSE to support the Python langauge])
	;;
opencl)
	support_opencl_frontend=yes
	AC_DEFINE([ROSE_BUILD_OPENCL_LANGUAGE_SUPPORT], [], [Build ROSE to support the OpenCL langauge])
        GENERATE_OPENCL_SPECIFIC_HEADERS
	;;
*)
	AC_MSG_FAILURE([unrecognized language "$a_language"])
	;;
esac
done


#
# Check Java version for Fortran front-end support
#
# Versions greater than 1.8 may work but haven't been tested [Rasmussen, 2019.02.27]
#
#if test "x$support_fortran_frontend" = "xyes" ; then
#    if test "x$JAVA_VERSION_MAJOR" != x13; then
#	AC_MSG_FAILURE([Java version for Fortran front-end must be openjdk version 13.0.x [major version not 13]])
#    fi
#    if test "x$JAVA_VERSION_MINOR" != x0; then
#	AC_MSG_FAILURE([Java version for Fortran front-end must be openjdk version 13.0.x [minor version not 0]])
#    fi
#fi


#########################################################################################
#
#  Output language support
#
#########################################################################################
#########################################################################################
#
#  Enabled only one language: set specific configurations for minimal build of ROSE 
#
#########################################################################################
if test $count_of_languages_to_support = 2 ; then
  #
  # Only C/C++ (currently required to be supported simultaneously) 
  #
  if test "x$support_c_frontend" = "xyes" && test "x$support_cxx_frontend" = "xyes" ; then
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
  if test "x$support_java_frontend" = "xyes" ; then
    # When using fortran only assume that we are not interested in java language support in ROSE.
    # However, currently the --with-java option controls the use of java support for both Fortran
    # and Java language support. Now that we have added Java language support to ROSE this is
    # unintentionally confusing. So we can't turn this off since the Fortran support requires
    # internal java (JVM) support.
    # with_java=no

    with_haskell=no
    with_php=no
    enable_python=no
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
  if test "x$support_fortran_frontend" = "xyes" ; then
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
    enable_python=no

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
  if test "x$support_php_frontend" = "xyes" ; then
    with_haskell=no
    enable_binary_analysis_tests=no
    enable_projects_directory=no
    enable_tutorial_directory=no
  fi

  #
  # Only Python
  #
  if test "x$support_python_frontend" = "xyes" ; then
    with_haskell=no
    enable_binary_analysis_tests=no
    enable_projects_directory=no
    enable_tutorial_directory=no
  fi

  #
  # Only binary analysis 
  #
  if test "x$support_binaries_frontend" = "xyes" ; then
    with_haskell=no
    enable_projects_directory=no
    enable_tutorial_directory=no
  fi

  #
  # Only cuda  
  #
  if test "x$support_cuda_frontend" = "xyes" ; then
    with_haskell=no
    enable_binary_analysis_tests=no
    enable_projects_directory=no
    enable_tutorial_directory=no
  fi

  #
  # Only opencl  
  #
  if test "x$support_opencl_frontend" = "xyes" ; then
    with_haskell=no
    enable_binary_analysis_tests=no
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
    AC_MSG_NOTICE([$LANGUAGES_TO_SUPPORT-only support with haskell])
  elif test "x$with_haskell" = "xno" ; then
    AC_MSG_NOTICE([$LANGUAGES_TO_SUPPORT-only support without haskell])
  fi

  #
  # PHP
  #
  if test "x$with_php" = "xyes" ; then
    AC_MSG_NOTICE([$LANGUAGES_TO_SUPPORT-only support with PHP])
  elif test "x$with_php" = "xno" ; then
    AC_MSG_NOTICE([$LANGUAGES_TO_SUPPORT-only support without PHP])
  fi

  #
  # Python
  #
  if test "x$enable_python" = "xyes" ; then
    AC_MSG_NOTICE([$LANGUAGES_TO_SUPPORT-only support with Python])
  elif test "x$enable_python" = "xno" ; then
    AC_MSG_NOTICE([$LANGUAGES_TO_SUPPORT-only support without Python])
  fi

  #
  # Binary analysis tests
  #
  if test "x$enable_binary_analysis_tests" = "xyes" ; then
    AC_MSG_NOTICE([$LANGUAGES_TO_SUPPORT-only support enabling binary analysis tests])
  elif test "x$enable_binary_analysis_tests" = "xno" ; then
    AC_MSG_NOTICE([$LANGUAGES_TO_SUPPORT-only support disabling binary analysis tests])
  fi

  #
  # Projects/ directory 
  #
  if test "x$enable_projects_directory" = "xyes" ; then
    AC_MSG_NOTICE([$LANGUAGES_TO_SUPPORT-only support enabling ROSE/projects directory])
  elif test "x$enable_projects_directory" = "xno" ; then
    AC_MSG_NOTICE([$LANGUAGES_TO_SUPPORT-only support disabling ROSE/projects directory])
  fi

  #
  # Tutorial/ directory 
  #
  if test "x$enable_tutorial_directory" = "xyes" ; then
    AC_MSG_NOTICE([$LANGUAGES_TO_SUPPORT-only support enabling ROSE/tutorial directory])
  elif test "x$enable_tutorial_directory" = "xno" ; then
    AC_MSG_NOTICE([$LANGUAGES_TO_SUPPORT-only support disabling ROSE/tutorial directory])
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
AM_CONDITIONAL(ROSE_BUILD_C_LANGUAGE_SUPPORT, [test "x$support_c_frontend" = xyes])
AM_CONDITIONAL(ROSE_BUILD_CXX_LANGUAGE_SUPPORT, [test "x$support_cxx_frontend" = xyes])
AM_CONDITIONAL(ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT, [test "x$support_fortran_frontend" = xyes])
AM_CONDITIONAL(ROSE_BUILD_JAVA_LANGUAGE_SUPPORT, [test "x$support_java_frontend" = xyes])
AM_CONDITIONAL(ROSE_BUILD_X10_LANGUAGE_SUPPORT, [false])
AM_CONDITIONAL(ROSE_BUILD_PHP_LANGUAGE_SUPPORT, [test "x$support_php_frontend" = xyes])
AM_CONDITIONAL(ROSE_BUILD_PYTHON_LANGUAGE_SUPPORT, [test "x$support_python_frontend" = xyes])
AM_CONDITIONAL(ROSE_BUILD_BINARY_ANALYSIS_SUPPORT, [test "x$support_binaries_frontend" = xyes])
AM_CONDITIONAL(ROSE_BUILD_CUDA_LANGUAGE_SUPPORT, [test "x$support_cuda_frontend" = xyes])
AM_CONDITIONAL(ROSE_BUILD_OPENCL_LANGUAGE_SUPPORT, [test "x$support_opencl_frontend" = xyes])

########################################################################################################################
# Automake doesn't support arbitrary logic expressions in "if" statements, so we have to define a boolean variable here for each
# logic expression that we need somewhere in a makefile. Per custom, we also define the corresponding C preprocessor symbol.

# The C preprocessor is supported when any of C, C++, or Fortran are supported.
if test "$support_c_frontend" = yes -o "$support_cxx_frontend" = yes -o "$support_fortran_frontend" = yes; then
    support_cpp_frontend=yes
else
    support_cpp_frontend=no
fi
AM_CONDITIONAL(ROSE_BUILD_CPP_LANGUAGE_SUPPORT, [test "$support_cpp_frontend" = yes])
if test "$support_cpp_frontend" = yes; then
    AC_DEFINE(ROSE_BUILD_CPP_LANGUAGE_SUPPORT, [], [Build ROSE to support the C preprocessor language])
fi

########################################################################################################################
# Summary output
AC_MSG_CHECKING([if the Binary Analysis frontend is enabled])
if test "x$support_binaries_frontend" = "xyes"; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
fi

AC_MSG_CHECKING([if the C frontend is enabled])
if test "x$support_c_frontend" = "xyes"; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
fi

AC_MSG_CHECKING([if the C++ frontend is enabled])
if test "x$support_cxx_frontend" = "xyes"; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
fi

AC_MSG_CHECKING([if the Cuda frontend is enabled])
if test "x$support_cuda_frontend" = "xyes"; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
fi

AC_MSG_CHECKING([if the Fortran frontend is enabled])
if test "x$support_fortran_frontend" = "xyes"; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
fi

AC_MSG_CHECKING([if the Java frontend is enabled])
if test "x$support_java_frontend" = "xyes"; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
fi

AC_MSG_CHECKING([if the PHP frontend is enabled])
if test "x$support_php_frontend" = "xyes"; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
fi

AC_MSG_CHECKING([if the Python frontend is enabled])
if test "x$support_python_frontend" = "xyes"; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
fi

AC_MSG_CHECKING([if the OpenCL frontend is enabled])
if test "x$support_opencl_frontend" = "xyes"; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
fi

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

USER_GAVE_ENABLE_ONLY_LANGUAGE_CONFIG_OPTION=no
AC_ARG_ENABLE([only-binary-analysis],
              AS_HELP_STRING([--enable-only-binary-analysis(=yes)],
                             [Enable ONLY binary support in ROSE (Warning: '--enable-only-binary-analysis=no' and '--disable-only-binary-analysis' are no longer supported)]),
                case "$enableval" in
                  [yes)]
                  	LANGUAGES_TO_SUPPORT="binaries"
                        USER_GAVE_ENABLE_ONLY_LANGUAGE_CONFIG_OPTION=yes
                  	;;
                  [no)]
                  	[AC_MSG_FAILURE([--enable-only-binary-analysis="$enableval" and --disable-only-binary-analysis are no longer supported; use --disable-binary-analysis (see ./configure --help)])]
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-only-binary-analysis="$enableval" is not supported; use --enable-only-binary-analysis (see ./configure --help)])]
                 	;;
                esac
               ,)

AC_ARG_ENABLE([only-c],
              AS_HELP_STRING([--enable-only-c(=yes)],
                             [Enable ONLY C support in ROSE (Warning: '--enable-only-c=no' and '--disable-only-c' are no longer supported)]),
                case "$enableval" in
                  [yes)]
                  	LANGUAGES_TO_SUPPORT="c"
                        USER_GAVE_ENABLE_ONLY_LANGUAGE_CONFIG_OPTION=yes
                  	;;
                  [no)]
                  	[AC_MSG_FAILURE([--enable-only-c="$enableval" and --disable-only-c are no longer supported; use --disable-c (see ./configure --help)])]
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-only-c="$enableval" is not supported; use --enable-only-c (see ./configure --help)])]
                 	;;
                esac
               ,)

AC_ARG_ENABLE([only-cxx],
              AS_HELP_STRING([--enable-only-cxx(=yes)],
                             [Enable ONLY C++ support in ROSE (Warning: '--enable-only-cxx=no' and '--disable-only-cxx' are no longer supported)]),
                case "$enableval" in
                  [yes)]
                  	LANGUAGES_TO_SUPPORT="c++"
                        USER_GAVE_ENABLE_ONLY_LANGUAGE_CONFIG_OPTION=yes
                  	;;
                  [no)]
                  	[AC_MSG_FAILURE([--enable-only-cxx="$enableval" and --disable-only-cxx are no longer supported; use --disable-cxx (see ./configure --help)])]
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-only-cxx="$enableval" is not supported; use --enable-only-cxx (see ./configure --help)])]
                 	;;
                esac
               ,)

AC_ARG_ENABLE([only-fortran],
              AS_HELP_STRING([--enable-only-fortran(=yes)],
                             [Enable ONLY Fortran support in ROSE (Warning: '--enable-only-fortran=no' and '--disable-only-fortran' are no longer supported)]),
                case "$enableval" in
                  [yes)]
                        if test "x$with_java" = "xno" ; then
                          [AC_MSG_FAILURE([[[Fortran Support]] you specified conflicting configure flags: --enable-only-fortran="$enableval" enables Fortran-language support, which requires Java support, and --with-java="$with_java" disables Java support])]
                        else  
                  	  LANGUAGES_TO_SUPPORT="fortran"
                          USER_GAVE_ENABLE_ONLY_LANGUAGE_CONFIG_OPTION=yes
                        fi
                  	;;
                  [no)]
                  	[AC_MSG_FAILURE([--enable-only-fortran="$enableval" and --disable-only-fortran are no longer supported; use --disable-fortran (see ./configure --help)])]
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-only-fortran="$enableval" is not supported; use --enable-only-fortran (see ./configure --help)])]
                 	;;
                esac
               ,)

AC_ARG_ENABLE([only-java],
              AS_HELP_STRING([--enable-only-java(=yes)],
                             [Enable ONLY Java support in ROSE (Warning: '--enable-only-java=no' and '--disable-only-java' are no longer supported)]),
                case "$enableval" in
                  [yes)]
                        if test "x$with_java" = "xno" ; then
                          [AC_MSG_FAILURE([[[Java Support]] you specified conflicting configure flags: --enable-only-java="$enableval" enables Java-language support, but --with-java="$with_java" disables it])]
                        else  
                  	  LANGUAGES_TO_SUPPORT="java"
                          USER_GAVE_ENABLE_ONLY_LANGUAGE_CONFIG_OPTION=yes
                        fi
                  	;;
                  [no)]
                  	[AC_MSG_FAILURE([--enable-only-java="$enableval" and --disable-only-java are no longer supported; use --disable-java (see ./configure --help)])]
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-only-java="$enableval" is not supported; use --enable-only-java (see ./configure --help)])]
                 	;;
                esac
               ,)

AC_ARG_ENABLE([only-php],
              AS_HELP_STRING([--enable-only-php(=yes)],
                             [Enable ONLY PHP support in ROSE (Warning: '--enable-only-php=no' and '--disable-only-php' are no longer supported)]),
                case "$enableval" in
                  [yes)]
                  	LANGUAGES_TO_SUPPORT="php"
                        USER_GAVE_ENABLE_ONLY_LANGUAGE_CONFIG_OPTION=yes
                  	;;
                  [no)]
                  	[AC_MSG_FAILURE([--enable-only-php="$enableval" and --disable-only-php are no longer supported; use --disable-php (see ./configure --help)])]
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-only-php="$enableval" is not supported; use --enable-only-php (see ./configure --help)])]
                 	;;
                esac
               ,)

AC_ARG_ENABLE([only-python],
              AS_HELP_STRING([--enable-only-python(=yes)],
                             [Enable ONLY Python support in ROSE (Warning: '--enable-only-python=no' and '--disable-only-python' are no longer supported)]),
                case "$enableval" in
                  [yes)]
                        LANGUAGES_TO_SUPPORT="python"
                        USER_GAVE_ENABLE_ONLY_LANGUAGE_CONFIG_OPTION=yes
                  	;;
                  [no)]
                  	[AC_MSG_FAILURE([--enable-only-python="$enableval" and --disable-only-python are no longer supported; use --disable-python (see ./configure --help)])]
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-only-python="$enableval" is not supported; use --enable-only-python (see ./configure --help)])]
                 	;;
                esac
               ,)

AC_ARG_ENABLE([only-cuda],
              AS_HELP_STRING([--enable-only-cuda(=yes)],
                             [Enable ONLY Cuda support in ROSE (Warning: '--enable-only-cuda=no' and '--disable-only-cuda' are no longer supported)]),
                case "$enableval" in
                  [yes)]
                  	LANGUAGES_TO_SUPPORT="cuda"
                        USER_GAVE_ENABLE_ONLY_LANGUAGE_CONFIG_OPTION=yes
                  	;;
                  [no)]
                  	[AC_MSG_FAILURE([--enable-only-cuda="$enableval" and --disable-only-cuda are no longer supported; use --disable-cuda (see ./configure --help)])]
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-only-cuda="$enableval" is not supported; use --enable-only-cuda (see ./configure --help)])]
                 	;;
                esac
               ,)

AC_ARG_ENABLE([only-opencl],
              AS_HELP_STRING([--enable-only-opencl(=yes)],
                             [Enable ONLY OpenCL support in ROSE (Warning: '--enable-only-opencl=no' and '--disable-only-opencl' are no longer supported)]),
                case "$enableval" in
                  [yes)]
                  	LANGUAGES_TO_SUPPORT="opencl"
                        USER_GAVE_ENABLE_ONLY_LANGUAGE_CONFIG_OPTION=yes
                  	;;
                  [no)]
                  	[AC_MSG_FAILURE([--enable-only-opencl="$enableval" and --disable-only-opencl are no longer supported; use --disable-opencl (see ./configure --help)])]
                  	;;
                  [*)]
                  	[AC_MSG_FAILURE([--enable-only-opencl="$enableval" is not supported; use --enable-only-opencl (see ./configure --help)])]
                 	;;
                esac
               ,)

#echo "Finished configuring deprecated user-specified language configuration options"
# End macro ROSE_SUPPORT_LANGUAGE_CONFIG_OPTIONS_DEPRECATED
])
