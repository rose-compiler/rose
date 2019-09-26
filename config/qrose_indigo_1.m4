dnl ------------------------------------------------------------------------
dnl Find a file (or one of more files in a list of dirs)
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_FIND_FILE],
[
$3=NONE
for i in $2;
do
  for j in $1;
  do    
    if test -r "$i/$j"; then
      $3=$i
      break 2
    fi
  done
done
])

AC_DEFUN([AC_FIND_FILE_EXT],
[
$3=NONE

for i in $2;
do
  for j in $1;
  do
    try="ls -l $i/$j*"
    if test=`eval $try 2> /dev/null`; then
      $3=$i
      break 2
    fi
  done
done
])


dnl ------------------------------------------------------------------------
dnl Try to find ROSE headers and libraries.
dnl ------------------------------------------------------------------------
dnl 
dnl AC_DEFUN([AC_PATH_ROSE],
dnl [
dnl    AC_MSG_CHECKING([for ROSE])
dnl 
dnl    AC_ARG_WITH(rose,
dnl     [  --with-rose=DIR   where the ROSE includes are. ],
dnl     [  ac_rose="$withval" ]
dnl    )
dnl 
dnl dnl ---------------------------[check ROSE headers]------------
dnl   rose_incdirs="$ac_rose/include"
dnl   AC_FIND_FILE(rose.h, $rose_incdirs, rose_incdir)
dnl   if test "$rose_incdir" = NONE; then
dnl      AC_MSG_ERROR([ ROSE headers not found,  use --with-rose=DIR ])
dnl   fi
dnl   ROSE_INCLUDES="-I$rose_incdir"
dnl   AC_SUBST(ROSE_INCLUDES)
dnl   
dnl dnl ---------------------------[check ROSE libraries]----------
dnl   rose_libdirs="$ac_rose/lib"
dnl   AC_FIND_FILE_EXT(librose, $rose_libdirs, rose_libdir)
dnl   if test "$rose_libdir" = NONE; then
dnl      AC_MSG_ERROR([ ROSE libraries not found,  use --with-rose=DIR ])
dnl   fi
dnl   LIB_ROSE="-lrose"
dnl   ROSE_LDFLAGS="-L$rose_libdir"
dnl   AC_SUBST(LIB_ROSE)
dnl   AC_SUBST(ROSE_LDFLAGS)
dnl   AC_MSG_RESULT([ headers $rose_incdir, libraries $rose_libdir ])
dnl ])
dnl 

dnl ------------------------------------------------------------------------
dnl Try to find the QT headers and libraries.
dnl ------------------------------------------------------------------------
AC_DEFUN([AC_PATH_QT],
[
AC_MSG_CHECKING([for Qt])

dnl DQ (9/11/2008): Modified to set the "n" so that tests without QT would work.
AC_ARG_WITH(qt,
    [  --with-qt=DIR where the QT root directory is. ],
    [  ac_qt_path="$withval"
       if test "x$with_qt" = xyes; then
          AC_MSG_FAILURE([--with-qt=PATH must be specified to use option --with-qt (a valid Qt intallation)])
       fi
       if test "x$withval" = x; then
          AC_MSG_FAILURE([empty path used in --with-qt=PATH must be specified to use option --with-qt (a valid Qt intallation)])
       fi
    ],
    [  with_qt=no ])

AC_ARG_WITH(qt-inc,
    [  --with-qt-inc=DIR   where the QT includes are. ],
    [  ac_qt_includes="$withval"
    ])

AC_ARG_WITH(qt-lib,
    [  --with-qt-lib=DIR where the QT libraries are. ],
    [  ac_qt_libraries="$withval"
    ])

AC_ARG_WITH(roseQt,
    [  --with-roseQt=Build with roseQt. yes or no. defaults to no. ],
    [  AC_MSG_NOTICE([in test action given: with_roseQt = "$with_roseQt"])
       if test "x$with_roseQt" != xyes; then
          AC_MSG_FAILURE([no option should be specified with --with-roseQt...(usage: --with-roseQt or --with-roseQt=yes only)])
       fi
       if test "x$with_qt" = xno; then
          AC_MSG_FAILURE([--with-qt=PATH must be specified to use option --with-roseQt (roseQt requires valid Qt intallation)])
       fi
    ],
    [  with_roseQt=no ])
  
dnl ---------------------------[check Qt headers]------------
  qt_incdirs="$ac_qt_includes $ac_qt_path/include /usr/include/qt4"
  AC_FIND_FILE(QtGui/qapplication.h, $qt_incdirs, qt_incdir)

dnl echo "qt_incdir = $qt_incdir"
dnl echo "with_QRose = $with_QRose"

dnl DQ (9/12/2008): Only test for failure if we have configured for QRose use.
dnl This prevents failing when we find Qt3 and we have not configured
dnl ROSE to use QRose. Also fixed in code below.
dnl if test "x$with_QRose" != xno && test "$qt_incdir" = NONE; then

dnl Gabriel (7/7/2009): Added Qt3 support for the ROSE components - we need to define QT3_SUPPORT , 
dnl                     but I am not sure where to put it, so I have placed it below with the includes
  if test "x$with_QRose" != xno; then
     if test "$qt_incdir" = NONE; then
        AC_MSG_ERROR([Qt headers not found; use --with-qt=DIR or --with-qt-includes=DIR])
     fi
  fi
  QT_INCLUDES="-DQT3_SUPPORT -I$qt_incdir/QtCore -I$qt_incdir/QtGui -I$qt_incdir/Qt3Support -I$qt_incdir/QtXml -I$qt_incdir"
  AC_SUBST(QT_INCLUDES)

dnl ---------------------------[check Qt Libraries]------------
  qt_libdirs="$ac_qt_libraries $ac_qt_path/lib /usr/lib"
  AC_FIND_FILE_EXT(libQtGui, $qt_libdirs, qt_libdir)
dnl if test "$with_QRose" != no && test "$qt_libdir" = NONE; then
  if test "$with_QRose" != no; then
     if test "$qt_libdir" = NONE; then
        AC_MSG_ERROR([Qt libraries not found; use --with-qt=DIR or --with-qt-libraries=DIR])
     fi
  fi
dnl  LIB_QT="-lQtCore -lQtGui -lQt3Support"
  QT_LDFLAGS="-L$qt_libdir -lQtCore -lQtGui -lQt3Support -lQtXml -lQtUiTools"
dnl  AC_SUBST(LIB_QT)

  AM_CONDITIONAL(ROSE_USE_QT,test "$with_qt" != no)

  AC_SUBST(QT_LDFLAGS)
  AC_MSG_RESULT([ headers $qt_incdir, libraries $qt_libdir ])
])

dnl ------------------------------------------------------------------------
dnl Find Qt meta object compiler (moc)
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_PATH_QT_MOC],
[
   AC_REQUIRE([AC_CHECK_WINDOWS])
   AC_REQUIRE([AC_PATH_QT])

   AC_ARG_WITH(qt-bin,
    [  --with-qt-bin=DIR   where the QT binaries are. ],
    [  ac_qt_bin="$withval"
    ])

dnl ------------------------[get binary]-----------------

   moc_dirs="$ac_qt_bin $ac_qt_path/bin `echo $PATH | sed s/:/\ /g`"
   if test $USING_WINDOWS = 1; then
      moc_binary="moc.exe"
   else
      moc_binary="moc"
   fi

dnl ------------------------[check if moc exists]-----------------
   AC_MSG_CHECKING([for Qt meta-object compiler])
   AC_FIND_FILE($moc_binary, $moc_dirs, moc_dir)
dnl if test "$with_QRose" != no && test "$moc_dir" = NONE; then
   if test "$with_QRose" != no; then
      if test "$moc_dir" = NONE; then
         AC_MSG_ERROR([no Qt meta object compiler ($moc_binary) found; should be found in PATH, or --with-qt=DIR-qt_root, or --with-qt-bin=DIR-qt_bin])
      fi
   fi
   MOC=$moc_dir/$moc_binary
   AC_MSG_RESULT([$MOC])
dnl ------------------------[check if moc is version 4]-----------------
   AC_MSG_CHECKING([Qt meta-object compiler version])

dnl DQ (9/12/2008): Added to test for use of QRose to prevent error if Qt3 is found.
   if test "$with_QRose" != no; then
      if test "$moc_dir" != NONE; then
         try=`$MOC -v 2>&1 | grep "Qt 4."`
         if test -z "$try"; then
            AC_MSG_ERROR([invalid version - "$MOC" must be version 4.x.x])
         else
            AC_MSG_RESULT([passed])
         fi
     fi
     AC_SUBST(MOC)
   fi
])


dnl MB:  05/14/09  copied and adapted MOC finding to find also uic and rcc

dnl ------------------------------------------------------------------------
dnl Find Qt ui compiler (uic)
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_PATH_QT_UIC],
[
   AC_REQUIRE([AC_CHECK_WINDOWS])
   AC_REQUIRE([AC_PATH_QT])

   AC_ARG_WITH(qt-bin,
    [  --with-qt-bin=DIR   where the QT binaries are. ],
    [  ac_qt_bin="$withval"
    ])

dnl ------------------------[get binary]-----------------

   uic_dirs="$ac_qt_bin $ac_qt_path/bin `echo $PATH | sed s/:/\ /g`"
   if test $USING_WINDOWS = 1; then
      uic_binary="uic.exe"
   else
      uic_binary="uic"
   fi

dnl ------------------------[check if uic exists]-----------------
   AC_MSG_CHECKING([for Qt uic compiler])
   AC_FIND_FILE($uic_binary, $uic_dirs, uic_dir)
dnl if test "$with_QRose" != no && test "$uic_dir" = NONE; then
   if test "$with_QRose" != no; then
      if test "$uic_dir" = NONE; then
         AC_MSG_ERROR([no Qt ui compiler ($uic_binary) found; should be found in PATH, or --with-qt=DIR-qt_root, or --with-qt-bin=DIR-qt_bin])
      fi
   fi
   UIC=$uic_dir/$uic_binary
   AC_MSG_RESULT([$UIC])
dnl ------------------------[check if uic is version 4]-----------------
   AC_MSG_CHECKING([Qt uic compiler version])

dnl DQ (9/12/2008): Added to test for use of QRose to prevent error if Qt3 is found.
   if test "$with_QRose" != no; then
      if test "$uic_dir" != NONE; then
#        try=`$UIC -v 2>&1 | grep "Qt 4."`
         try=`$UIC -v 2>&1 | grep "Interface Compiler version 4."`
         if test -z "$try"; then
            AC_MSG_ERROR([invalid version - "$UIC" must be version 4.x.x])
         else
            AC_MSG_RESULT([passed])
         fi
     fi
     AC_SUBST(UIC)
   fi
])


dnl ------------------------------------------------------------------------
dnl Find Qt resource resource compiler
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_PATH_QT_RCC],
[
   AC_REQUIRE([AC_CHECK_WINDOWS])
   AC_REQUIRE([AC_PATH_QT])

   AC_ARG_WITH(qt-bin,
    [  --with-qt-bin=DIR   where the QT binaries are. ],
    [  ac_qt_bin="$withval"
    ])

dnl ------------------------[get binary]-----------------

   rcc_dirs="$ac_qt_bin $ac_qt_path/bin `echo $PATH | sed s/:/\ /g`"
   if test $USING_WINDOWS = 1; then
      rcc_binary="rcc.exe"
   else
      rcc_binary="rcc"
   fi

dnl ------------------------[check if rcc exists]-----------------
   AC_MSG_CHECKING([for Qt rcc compiler])
   AC_FIND_FILE($rcc_binary, $rcc_dirs, rcc_dir)
dnl if test "$with_QRose" != no && test "$rcc_dir" = NONE; then
   if test "$with_QRose" != no; then
      if test "$rcc_dir" = NONE; then
         AC_MSG_ERROR([no Qt rcc compiler ($rcc_binary) found; should be found in PATH, or --with-qt=DIR-qt_root, or --with-qt-bin=DIR-qt_bin])
      fi
   fi
   RCC=$rcc_dir/$rcc_binary
   AC_MSG_RESULT([$RCC])
dnl ------------------------[check if rcc is version 4]-----------------
   AC_MSG_CHECKING([Qt resource compiler version])

dnl DQ (9/12/2008): Added to test for use of QRose to prevent error if Qt3 is found.
   if test "$with_QRose" != no; then
      if test "$rcc_dir" != NONE; then
#        try=`$RCC -v 2>&1 | grep "Qt 4."`
         try=`$RCC -v 2>&1 | grep "Qt Resource Compiler version 4."`
         if test -z "$try"; then
            AC_MSG_ERROR([invalid version - "$RCC" must be version 4.x.x])
         else
            AC_MSG_RESULT([passed])
         fi
     fi
     AC_SUBST(RCC)
   fi
])

dnl --------------------------------------------------------------------
dnl check qt version
dnl --------------------------------------------------------------------

AC_DEFUN([AC_PATH_QT_VERSION],
[
    AC_REQUIRE([AC_PATH_QT])

    if test $with_qt != no
    then
        dnl get complete version string ...
        QT_VERSION=`grep QT_VERSION_STR $qt_incdir/QtCore/qglobal.h | awk '{print $ 3}' | sed -e 's/\"//g'`
        dnl get major version number
        QT_VERSION_MAJOR=`echo $QT_VERSION | awk -F . '{print $ 1}'`
        QT_VERSION_MINOR=`echo $QT_VERSION | awk -F . '{print $ 2}'`

        dnl test for empty variables
        if test -z "$QT_VERSION_MAJOR"
        then
            QT_VERSION_MAJOR=0
        fi
        if test -z "$QT_VERSION_MINOR"
        then
            QT_VERSION_MINOR=0
        fi

        if test x"$with_roseQt" != x"no"
        then
            AC_MSG_NOTICE([in qrose_indigi_1 -- qt version macro: with_roseQt = "$with_roseQt", QT_VERSION_MAJOR = "$QT_VERSION_MAJOR", QT_VERSION_MINOR = "$QT_VERSION_MINOR"])
            AC_MSG_CHECKING([Qt version compatible with roseQt])
            if test $QT_VERSION_MAJOR -ge 4 && test $QT_VERSION_MINOR -ge 4
            then
                with_roseQt=yes
                AC_MSG_RESULT([passed])
            else
                AC_MSG_ERROR([invalid version - qt must be at least version 4.4.x in order to build with roseQt])
            fi
        dnl Comment the following lines in, to have roseQt automatically enabled if qt>=4.4.0
        dnl else
            dnl if test $QT_VERSION_MAJOR -lt 4 && test $QT_VERSION_MINOR -lt 4
            dnl then
            dnl    AC_MSG_ERROR([ invalid version - qt must be at least version 4.4.x in order to build with roseQt])
            dnl else
            dnl    with_roseQt=yes
            dnl    AC_MSG_RESULT([passed])
            dnl fi
        fi
        AC_SUBST(QT_VERSION)
        AC_SUBST(QT_VERSION_MAJOR)
        AC_SUBST(QT_VERSION_MINOR)
    fi

    AC_MSG_NOTICE([with_roseQt = "$with_roseQt"])
    AM_CONDITIONAL(ROSE_WITH_ROSEQT,test x"$with_roseQt" != x"no")
])
            
