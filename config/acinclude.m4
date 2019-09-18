dnl
dnl This file contains local autoconf functions (borrowed from PHP).
dnl
dnl echo "${1}..............." | awk '{ printf("%s", substr([$]$0, 0, 15)); }'"

dnl ----------------------------------------------------------------------------
dnl Output stylize macros for configure (help/runtime)
dnl ----------------------------------------------------------------------------

dnl
dnl ROSE_HELP_WITH_SEPARATOR(title)
dnl
dnl Adds separator title into the --with section of the
dnl configure --help display.
dnl
AC_DEFUN([ROSE_HELP_WITH_SEPARATOR],[
AC_ARG_WITH([],[
$1
],[])
])


dnl
dnl ROSE_HELP_ENABLE_SEPARATOR(title)
dnl
dnl Adds separator title into the --enable section of the
dnl configure --help display.
dnl
AC_DEFUN([ROSE_HELP_ENABLE_SEPARATOR],[
AC_ARG_ENABLE([],[
$1
],[])
])

dnl
dnl ROSE_CONFIGURE_SECTION(title)
dnl
dnl Adds separator title configure output (idea borrowed from mm)
dnl
AC_DEFUN([ROSE_CONFIGURE_SECTION],[
  AC_MSG_RESULT()
  echo "${T_BM}$1${T_NM}"
])

dnl ----------------------------------------------------------------------------
dnl  Misc. macros
dnl ----------------------------------------------------------------------------
dnl


dnl
dnl ROSE_MSG_ERROR(title)
dnl
dnl Pretty wrapper around AC_MSG_ERROR.
dnl
AC_DEFUN([ROSE_MSG_ERROR],[
"$rose_shtool" echo "--------------------------------------------------------------------------------"
AC_MSG_ERROR([$1

For assistance, please visit http://rosecompiler.org or contact the rose-public@nersc.gov mailing list.
])
])

dnl
dnl ROSE_CONFIG_NICE(filename)
dnl
dnl Generates the config.nice file
dnl
AC_DEFUN([ROSE_CONFIG_NICE],[
  AC_REQUIRE([AC_PROG_EGREP])
  AC_REQUIRE([LT_AC_PROG_SED])
  cat >$1<<EOF
#! /bin/sh
#
# Created by configure

EOF

  for var in CFLAGS CXXFLAGS CPPFLAGS LDFLAGS EXTRA_LDFLAGS_PROGRAM LIBS CC CXX; do
    eval val=\$$var
    if test -n "$val"; then
      echo "$var='$val' \\" >> $1
    fi
  done

  echo "'[$]0' \\" >> $1
  if test `expr -- [$]0 : "'.*"` = 0; then
    CONFIGURE_COMMAND="$CONFIGURE_COMMAND '[$]0'"
  else
    CONFIGURE_COMMAND="$CONFIGURE_COMMAND [$]0"
  fi
  for arg in $ac_configure_args; do
     if test `expr -- $arg : "'.*"` = 0; then
        if test `expr -- $arg : "--.*"` = 0; then
          break;
        fi
        echo "'[$]arg' \\" >> $1
        CONFIGURE_OPTIONS="$CONFIGURE_OPTIONS '[$]arg'"
     else
        if test `expr -- $arg : "'--.*"` = 0; then
          break;
        fi
        echo "[$]arg \\" >> $1
        CONFIGURE_OPTIONS="$CONFIGURE_OPTIONS [$]arg"
     fi
  done
  echo '"[$]@"' >> $1
  chmod +x $1
  CONFIGURE_COMMAND="$CONFIGURE_COMMAND $CONFIGURE_OPTIONS"
])

dnl
dnl ROSE_CONFIGURE_SECTION(title)
dnl
dnl Adds environment information to a log file.
dnl This is meant to be more useful than config.log as a quick
dnl reference for critical configuration information.
dnl
dnl Particularly, this is useful for QMTest and our automatic
dnl issue tracking with Redmine, to formulate test dimensions.
dnl
AC_DEFUN([ROSE_ENV_LOG],[
  if test -z "$1$2"; then
    AC_MSG_FAILURE([ROSE environment log macro takes two arguments: name, value])
  fi

  printf "%30s : %s\n" "$1" "$2" >> "$ENV_LOG"
  #echo "$1" >> $ENV_LOG
])

dnl
dnl ROSE_CHECK_CONFIGURE_OPTIONS
dnl
AC_DEFUN([ROSE_CHECK_CONFIGURE_OPTIONS],[
  for arg in $ac_configure_args; do
    dnl Sometimes the args are enclosed in single-quotes.
    arg="$(echo "$arg" | sed -e "s/^'//" -e "s/'$//")"
    case $arg in
      --with-*[)]
        arg_name="`echo [$]arg | $SED -e 's/--with-/with-/g' -e 's/=.*//g'`"
        ;;
      --without-*[)]
        arg_name="`echo [$]arg | $SED -e 's/--without-/with-/g' -e 's/=.*//g'`"
        ;;
      --enable-*[)]
        arg_name="`echo [$]arg | $SED -e 's/--enable-/enable-/g' -e 's/=.*//g'`"
        ;;
      --disable-*[)]
        arg_name="`echo [$]arg | $SED -e 's/--disable-/enable-/g' -e 's/=.*//g'`"
        ;;
      *[)]
        continue
        ;;
    esac
    case $arg_name in
      # Allow certain libtool options
      enable-ltdl-convenience | enable-libtool-lock | with-pic | with-tags | enable-shared | enable-static | enable-fast-install | with-gnu-ld[)];;

      # All the rest must be set using the ROSE_ARG_* macros
      # ROSE_ARG_* macros set rose_enable_<arg_name> or rose_with_<arg_name>
      *[)]
        is_arg_set=rose_[]`echo [$]arg_name | tr 'ABCDEFGHIJKLMNOPQRSTUVWXYZ-' 'abcdefghijklmnopqrstuvwxyz_'`
        if eval test "x\$$is_arg_set" = "x"; then
          ROSE_UNKNOWN_CONFIGURE_OPTIONS="$ROSE_UNKNOWN_CONFIGURE_OPTIONS
[$]arg"
        fi
        ;;
    esac
  done

  dnl Output unknown configure options
  if test "$ROSE_UNKNOWN_CONFIGURE_OPTIONS"; then
cat <<X
--------------------------------------------------------------------------------
${T_BM}Warning${T_NM}: Unknown configure options were used:
$ROSE_UNKNOWN_CONFIGURE_OPTIONS

Please check '[$]0 --help' for available options.

X
  fi
])

dnl ----------------------------------------------------------------------------
dnl  Wrapper macros for AC_ARG_WITH / AC_ARG_ENABLE
dnl ----------------------------------------------------------------------------

dnl
dnl ROSE_ARG_WITH(arg-name, check message, help text[, default-val])
dnl Sets ROSE_ARG_NAME either to the user value or to the default value.
dnl default-val defaults to no.  This will also set the variable ext_shared,
dnl and will overwrite any previous variable of that name.
dnl
AC_DEFUN([ROSE_ARG_WITH],[
rose_with_[]translit($1,A-Z0-9-,a-z0-9_)=ifelse($4,,no,$4)
ROSE_REAL_ARG_WITH([$1],[$2],[$3],[$4],ROSE_WITH_[]translit($1,a-z0-9-,A-Z0-9_))
])

dnl ROSE_REAL_ARG_WITH
dnl internal
AC_DEFUN([ROSE_REAL_ARG_WITH],[
ifelse([$2],,,[AC_MSG_CHECKING([$2])])
AC_ARG_WITH($1,AS_HELP_STRING([--with-$1=PREFIX],[$3]),
[
  CONFIG_HAS_[]$5=yes
  $5=[$]withval
],
[
  CONFIG_HAS_[]$5=no
  $5=ifelse($4,,no,$4)
])
AC_MSG_RESULT([[$]$5])
])

dnl
dnl ROSE_ARG_ENABLE(arg-name, check message, help text[, default-val])
dnl Sets ROSE_ARG_NAME either to the user value or to the default value.
dnl default-val defaults to no.  This will also set the variable ext_shared,
dnl and will overwrite any previous variable of that name.
dnl
AC_DEFUN([ROSE_ARG_ENABLE],[
rose_enable_[]translit($1,A-Z0-9-,a-z0-9_)=ifelse($4,,no,$4)
ROSE_REAL_ARG_ENABLE([$1],[$2],[$3],[$4],ROSE_ENABLE_[]translit($1,a-z0-9-,A-Z0-9_))
])

dnl ROSE_REAL_ARG_ENABLE
dnl internal
AC_DEFUN([ROSE_REAL_ARG_ENABLE],[
ifelse([$2],,,[AC_MSG_CHECKING([$2])])
AC_ARG_ENABLE($1,AS_HELP_STRING([--enable-$1],[$3]),
[
  CONFIG_HAS_[]$5=yes
  $5=[$]enableval
],
[
  CONFIG_HAS_[]$5=no
  $5=ifelse($4,,no,$4)
])
AC_MSG_RESULT([[$]$5])
])

