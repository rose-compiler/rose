# DWARF debugging-format library and it's location.
AC_DEFUN([ROSE_SUPPORT_DWARF],
[
    AC_ARG_WITH(
        [dwarf],
        AS_HELP_STRING(
            [--with-dwarf=PREFIX],
            [Use the libdwarf library, which is necessary in order to parse debugging tables in ELF files.
             The PREFIX, if specified, should be the prefix used to install libdwarf, such as "/usr/local".
             The default is the empty prefix, in which case the headers and library must be installed in a
             place where they will be found. Saying "no" for the prefix is the same as saying
             "--without-dwarf". See also, --with-elf which is a prerequisite for --with-dwarf.]),
            [with_dwarf=$withval],
            [with_dwarf=yes])

    AC_ARG_WITH(
        [dwarf-include],
        AS_HELP_STRING(
            [--with-dwarf-include=PREFIX],
            [if the  dwarf include directory was specified,
            use this dwarf include directory]),
            [with_dwarf_include=$withval],
            [with_dwarf-include=no])

    AC_ARG_WITH(
        [dwarf-lib],
        AS_HELP_STRING(
            [--with-dwarf-lib=PREFIX],
            [if the  dwarf library directory was specified,
            use this dwarf library directory]),
            [with_dwarf_lib=$withval],
            [with_dwarf-lib=no])

    LIBDWARF_CPPFLAGS=""
    LIBDWARF_LDFLAGS=""
    LIBDWARF_LIBS=""
    dwarf_found=no

    # Set up the results
    if test "$with_dwarf" = "no"; then
      AC_MSG_RESULT([disabled])
      dwarf_found=no
    else
      #Priority 1: Use --with-dwarf-include and --with-dwarf-lib if both are provided
      if test -n "$with_dwarf_include" && test -n "$with_dwarf_lib"; then
        AC_MSG_RESULT([using specified include and lib paths])
        
        dnl Set up include path
        if test -d "$with_dwarf_include"; then
          LIBDWARF_CPPFLAGS="-I$with_dwarf_include"
        else
          AC_MSG_ERROR([DWARF include directory $with_dwarf_include does not exist])
        fi
        
        #Set up library path
        if test -d "$with_dwarf_lib"; then
          LIBDWARF_LDFLAGS="-L$with_dwarf_lib"
        else
          AC_MSG_ERROR([DWARF library directory $with_dwarf_lib does not exist])
        fi
        
        # Test for libdwarf
        save_CPPFLAGS="$CPPFLAGS"
        save_LDFLAGS="$LDFLAGS"
        CPPFLAGS="$CPPFLAGS $LIBDWARF_CPPFLAGS"
        LDFLAGS="$LDFLAGS $LIBDWARF_LDFLAGS"
        
        AC_CHECK_HEADER([libdwarf.h], [dwarf_header_found=yes], [dwarf_header_found=no])
        AC_CHECK_LIB([dwarf], [dwarf_init], [dwarf_lib_found=yes], [dwarf_lib_found=no])
        
        if test "$dwarf_header_found" = "yes" && test "$dwarf_lib_found" = "yes"; then
          LIBDWARF_LIBS="-ldwarf"
          dwarf_found=yes
        else
          AC_MSG_ERROR([Could not find libdwarf in specified include ($with_dwarf_include) and lib ($with_dwarf_lib) directories])
        fi
        
        CPPFLAGS="$save_CPPFLAGS"
        LDFLAGS="$save_LDFLAGS"
        
      #Priority 2: Use --with-dwarf if provided (default lookup behavior)
      else
        AC_MSG_RESULT([using default lookup with prefix])
        
        if test "$with_dwarf" = "yes"; then
          dwarf_search_paths="/usr /usr/local /opt/local"
        else
          dnl Use the specified prefix
          dwarf_search_paths="$with_dwarf"
        fi
        
        # Search for DWARF in the specified paths
        for dwarf_path in $dwarf_search_paths; do
          if test -d "$dwarf_path"; then
            for include_subdir in include include/libdwarf; do
              for lib_subdir in lib lib64; do
                if test -f "$dwarf_path/$include_subdir/libdwarf.h" && \
                   test -f "$dwarf_path/$lib_subdir/libdwarf.a" -o -f "$dwarf_path/$lib_subdir/libdwarf.so"; then
                  
                  LIBDWARF_CPPFLAGS="-I$dwarf_path/$include_subdir"
                  LIBDWARF_LDFLAGS="-L$dwarf_path/$lib_subdir"
                  
                  save_CPPFLAGS="$CPPFLAGS"
                  save_LDFLAGS="$LDFLAGS"
                  CPPFLAGS="$CPPFLAGS $LIBDWARF_CPPFLAGS"
                  LDFLAGS="$LDFLAGS $LIBDWARF_LDFLAGS"
                  
                  AC_CHECK_HEADER([libdwarf.h], [dwarf_header_found=yes], [dwarf_header_found=no])
                  AC_CHECK_LIB([dwarf], [dwarf_init], [dwarf_lib_found=yes], [dwarf_lib_found=no])
                  
                  if test "$dwarf_header_found" = "yes" && test "$dwarf_lib_found" = "yes"; then
                    LIBDWARF_LIBS="-ldwarf"
                    dwarf_found=yes
                    break 3  # Break out of all three loops
                  fi
                  
                  CPPFLAGS="$save_CPPFLAGS"
                  LDFLAGS="$save_LDFLAGS"
                fi
              done
            done
          fi
        done
        
        if test "$dwarf_found" = "no"; then
          if test "$with_dwarf" = "yes"; then
            AC_MSG_WARN([Could not find DWARF library in standard locations. DWARF support will be disabled.])
          else
            AC_MSG_ERROR([Could not find DWARF library in specified location: $with_dwarf])
          fi
        fi
      fi
    fi

    # Set up the results
    if test "$dwarf_found" = "yes"; then
      AC_DEFINE([ROSE_HAVE_LIBDWARF], [1], [Define if libdwarf is available])
      AC_MSG_NOTICE([DWARF support enabled])
      AC_MSG_NOTICE([  LIBDWARF_CPPFLAGS = $LIBDWARF_CPPFLAGS])
      AC_MSG_NOTICE([  LIBDWARF_LDFLAGS  = $LIBDWARF_LDFLAGS])
      AC_MSG_NOTICE([  LIBDWARF_LIBS     = $LIBDWARF_LIBS])
    else
      AC_MSG_NOTICE([DWARF support disabled])
    fi
    
    dnl Export the variables
    AC_SUBST(LIBDWARF_CPPFLAGS)
    AC_SUBST(LIBDWARF_LDFLAGS)
    AC_SUBST(LIBDWARF_LIBS)
    
    dnl Set up Automake conditional
    AM_CONDITIONAL([ROSE_HAVE_LIBDWARF], [test "$dwarf_found" = "yes"])
])
    



# OLD VARIABLES NO LONGER USED
#   ROSE_USE_DWARF         -- cmake variable to determine if libdwarf is present; use ROSE_HAVE_LIBDWARF instead
#   USE_ROSE_DWARF_SUPPORT -- CPP symbol to determine if libdwarf is present; use ROSE_HAVE_LIBDWARF instead
#   DWARF_INCLUDE          -- substitution for CPP flags; use LIBDWARF_CPPFLAGS instead
#   DWARF_LINK             -- substitution for loader flags; use LIBDWARF_LDFLAGS instead
#   dwarf_path             -- substitution for dwarf installation prefix; use LIBDWARF_PREFIX instead
