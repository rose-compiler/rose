AC_DEFUN([USE_EDG_SOURCE_CODE],
dnl Control use of EDG source code within ROSE.  This option allows the user
dnl to use the EDG source code within ROSE.  By default ROSE distributions
dnl are built to use binaries of anything from the EDG source code so as to
dnl not publish any part of the EDG interface.  This is consistant with the
dnl requirements of the free EDG research license.
[
  AC_ARG_WITH(edg_source_code,
    [  --with-edg_source_code=true                Specify use of EDG source code (the default is NO EDG source code in the distribution)],
    [ if test "$with_edg_source_code" = true ; then
           with_edg_source_code=true
      else
           with_edg_source_code=false
      fi
    ] ,
    [ 
      with_edg_source_code=false
    ])

  # DQ (4/23/2005): Test for use of "--with-edg_source_code=true" configure option
  # Look for a representative binary (if it does not exist then edg_source_code must be true, else error!)

    pathToSourceDir=`cd $srcdir && pwd`

  # Control output of additional debuging information 
  # (expression "0 = 1" will fail and the debug information will NOT be output)
    if test 1 = 1; then
       # edg_source_code=$with_edg_source_code
       # echo "edg_source_code = $with_edg_source_code"
         echo "srcdir         = $srcdir"
         echo "pathToBuildDir = $pathToBuildDir"
         echo "pathToSourceDir = $pathToSourceDir"

         if test -r $pathToSourceDir/src/frontend/EDG/EDG_3.3/src/libedg33.la; then
              echo "Found $pathToSourceDir/src/frontend/EDG/EDG_3.3/src/libedg33.la"
           else
              echo "Could not find $pathToSourceDir/src/frontend/EDG/EDG_3.3/src/libedg33.la"
         fi

         if test -r src/frontend/EDG/EDG_3.3/src/libedg33.la; then
              echo "Found src/frontend/EDG/EDG_3.3/src/libedg33.la"
           else
              echo "Could not find src/frontend/EDG/EDG_3.3/src/libedg33.la"
         fi
    fi

  # The binaries are either in the source tree or have been built in the compile tree previously
    if ! (test -r $pathToSourceDir/src/frontend/EDG/EDG_3.3/src/libedg33.la || test -r src/frontend/EDG/EDG_3.3/src/libedg33.la); then
         if test "$with_edg_source_code" = true; then
              echo "ROSE configured to use EDG source code, this is a development version (no binaries present in compile tree)"
           else
              echo "**********************************************************************************************************************"
              echo "$pathToSourceDir/src/frontend/EDG/EDG_3.3/src/libedg33.la not found, rerun configure using --with-edg_source_code=true"
              echo ""
              echo "     *** This is a development version, checked out from the ROSE CVS repository ***"
              echo "     Binaries not found in source tree, EDG source is required to build EDG binaries"
              echo "     ROSE must be configured to use the EDG source (EDG research license is required)"
              echo ""
              echo "               Exiting ... (rerun configure with correct option, specifically \"--with-edg_source_code=true\")"
              echo "**********************************************************************************************************************"
              echo ""

            # Reset the value of "$with_edg_source_code"
              with_edg_source_code=true

            # Exit rather than resetting the value and continuing
              exit 1;
         fi
    fi

  # Output the current value
    echo "with_edg_source_code = $with_edg_source_code"

  # Make this available to the environment
    export with_edg_source_code

  # Now put it into the makefile to be used there when we rename the distribution
    AC_SUBST(with_edg_source_code)
])

