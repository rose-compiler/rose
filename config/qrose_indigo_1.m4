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

AC_ARG_WITH(qt,
    [  --with-qt=DIR where the QT root directory is. ],
    [  ac_qt_path="$withval"
    ])    

AC_ARG_WITH(qt-inc,
    [  --with-qt-inc=DIR   where the QT includes are. ],
    [  ac_qt_includes="$withval"
    ])

AC_ARG_WITH(qt-lib,
    [  --with-qt-lib=DIR where the QT libraries are. ],
    [  ac_qt_libraries="$withval"
    ])
  
dnl ---------------------------[check Qt headers]------------
  qt_incdirs="$ac_qt_includes $ac_qt_path/include /usr/include/qt4"
  AC_FIND_FILE(QtGui/qapplication.h, $qt_incdirs, qt_incdir)
  if test "$with_QRose" != no && test "$qt_incdir" = NONE; then
     AC_MSG_ERROR([ Qt headers not found,  use --with-qt=DIR or --with-qt-includes=DIR])
  fi
  QT_INCLUDES="-I$qt_incdir/QtCore -I$qt_incdir/QtGui -I$qt_incdir"
  AC_SUBST(QT_INCLUDES)

dnl ---------------------------[check Qt Libraries]------------
  qt_libdirs="$ac_qt_libraries $ac_qt_path/lib /usr/lib"
  AC_FIND_FILE_EXT(libQtGui, $qt_libdirs, qt_libdir)
  if test "$with_QRose" != no && test "$qt_libdir" = NONE; then
     AC_MSG_ERROR([ Qt libraries not found,  use --with-qt=DIR or --with-qt-libraries=DIR ])
  fi
dnl  LIB_QT="-lQtCore -lQtGui"
  QT_LDFLAGS="-L$qt_libdir -lQtCore -lQtGui"
dnl  AC_SUBST(LIB_QT)
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
   if test "$with_QRose" != no && test "$moc_dir" = NONE; then
     AC_MSG_ERROR([ No Qt meta object compiler ($moc_binary) found! (should be found in PATH, or --with-qt=DIR-qt_root, or --with-qt-bin=DIR-qt_bin)])
   fi
   MOC=$moc_dir/$moc_binary
   AC_MSG_RESULT([$MOC])
dnl ------------------------[check if moc is version 4]-----------------
   AC_MSG_CHECKING([Qt meta-object compiler version])

   if test "$moc_dir" != NONE; then
     try=`$MOC -v 2>&1 | grep "Qt 4."`
     if test -z "$try"; then
        AC_MSG_ERROR([ invalid version - $MOC must be version 4.x.x])
     else
        AC_MSG_RESULT([passed])
     fi
     AC_SUBST(MOC)
   fi
])

