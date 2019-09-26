dnl -*- autoconf -*-
AC_DEFUN([ROSE_SUPPORT_SWIPL],
[

AC_MSG_CHECKING(for SWI Prolog)

AC_ARG_WITH([swi-prolog],
        AS_HELP_STRING([--with-swi-prolog=PATH],
                [use swi-prolog for prolog support (default autodetect).  PATH is bin directory containing swipl if given]),
        [swiplsetting=$withval],
        [swiplsetting=try])

ROSE_HAVE_SWI_PROLOG=0

if test "x$swiplsetting" != xno; then
        if test "x$swiplsetting" = xtry -o "x$swiplsetting" = xyes ; then
# autodetect swipl bin PATH
                if which swipl > /dev/null 2> /dev/null; then
                        SWIPL="`which swipl`"
                        SWIPLBIN="`dirname $SWIPL`"
                        ROSE_HAVE_SWI_PROLOG=1
                elif test "x$swiplsetting" = xyes ; then
                        AC_MSG_ERROR([--with-swi-prolog set but swipl command not found in PATH])
                fi

        else
                if test -d "$swiplsetting"; then
#Verification of the bin directory containing the program is deferred later
                        SWIPLBIN="$swiplsetting"
                        ROSE_HAVE_SWI_PROLOG=1
                else
                        AC_MSG_ERROR([argument to --with-swi-prolog must be path to bin directory, but argument is not a directory])
                fi
        fi
fi

if test $ROSE_HAVE_SWI_PROLOG = 1; then
        if test ! -x "$SWIPLBIN/swipl" ; then
                AC_MSG_ERROR([swipl could not be found in bin directory $SWIPLBIN])
        fi
        AC_MSG_RESULT([$SWIPLBIN])
else
        AC_MSG_RESULT([no])
fi

AC_SUBST(SWIPLBIN)
AM_CONDITIONAL(ROSE_HAVE_SWI_PROLOG,test "$ROSE_HAVE_SWI_PROLOG" = 1)

])

#AC_DEFUN([ROSE_SUPPORT_SWIPL], [
#  ROSE_ARG_WITH(
#    [swi-prolog],
#    [SWI Prolog],
#    [enable SWI-Prolog in projects/OpenK @<:@default=no@:>@],
#    [])
#   
#  AS_IF([test "x$CONFIG_HAS_ROSE_WITH_SWI_PROLOG" = xyes], [
#   
#    PKG_CHECK_MODULES(PL, [pl >= 5.8.0],
#        [AC_SUBST([SWIPL_PREFIX], [`$PKG_CONFIG pl --variable prefix`])
#         AS_IF([test -x $SWIPL_PREFIX/bin/swipl],
#               [AC_SUBST([SWIPL], ["${SWIPL_PREFIX}/bin/swipl"])],
#               [AC_SUBST([SWIPL], ["${SWIPL_PREFIX}/bin/pl"])])
#         AC_SUBST([SWIPL_VERSION], [`${SWIPL} --version | awk '{print $3}'`])
#         AC_MSG_RESULT([using ${SWIPL}])
#        ],
#        [# alternate pkg-config name
#         PKG_CHECK_MODULES(SWIPL, [swipl >= 5.8.0], 
#            [AC_SUBST([SWIPL_PREFIX], [`$PKG_CONFIG swipl --variable prefix`])
#             AC_SUBST([SWIPL], ["${SWIPL_PREFIX}/bin/swipl"])
#             AC_SUBST([SWIPL_VERSION], [`${SWIPL} --version | awk '{print $3}'`])
#        ],
#        [AC_MSG_ERROR([Did not find SWI-Prolog version 5.8 or later! 
#    Please remember to set PKG_CONFIG_PATH accordingly.])
#             AC_SUBST([SWIPL], [""])
#            ])
#        ])
#     
#    have_swi_prolog=1
#    eval `$SWIPL --dump-runtime-variables |grep -v '^CC='`
#    AC_SUBST([SWI_PL], ["$SWIPL"])
#    AC_SUBST([SWI_PROLOG_LIBS], ["$PLLIBS"])
#    AC_SUBST([SWI_PROLOG_LDADD], ["-L'$PLBASE/lib/$PLARCH' -Wl,-rpath '$PLBASE/lib/$PLARCH' $PLLIB $PLLIBS"])
#    AC_SUBST([SWI_PROLOG_HOME_DIR], ["$PLBASE"])
#    AC_SUBST([SWI_PROLOG_INCLUDE_DIR], ["$PLBASE/include"])
#    AC_SUBST([SWI_PLC], ["$SWIPL -q -t main -O -L0 -G0 -T0"])
#    AC_DEFINE([HAVE_SWI_PROLOG], [1], [Define if you have SWI Prolog])
#    AC_MSG_RESULT([Termite will be built with SWI-Prolog support.])     
#  ],
#    [AC_SUBST([SWI_PL], ["false"])
#     AC_SUBST([SWI_PROLOG_LDADD], [""])	 # set a safe dummy default path
#     AC_SUBST([SWI_PROLOG_INCLUDE_DIR], ["/usr/include"])
#     AC_DEFINE([HAVE_SWI_PROLOG], [0], [Define if you have SWI Prolog])
#     AC_MSG_RESULT([Termite will be built WITHOUT SWI-Prolog support.])
#     have_swi_prolog=0
#    ])
#
#  AM_CONDITIONAL(ROSE_HAVE_SWI_PROLOG, [test "x$have_swi_prolog" = x1])
#  AS_IF([test "x$have_swi_prolog" = x1],
#      [AC_DEFINE(ROSE_HAVE_SWI_PROLOG, 1, [linking against libswipl])],
#      [AC_DEFINE(ROSE_HAVE_SWI_PROLOG, 0, [linking against libswipl])])
#  AC_SUBST(have_swi_prolog)
#])
