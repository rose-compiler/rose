
AC_DEFUN([ROSE_SUPPORT_BACKSTROKE],
[
# Begin macro ROSE_SUPPORT_BACKSTROKE.

# Inclusion of test for specific Discrete Event Simulators

# Support for ROSS Discrete Event Simulator and it's location.

AC_MSG_CHECKING(for ROSS Discrete Event Simulator)
AC_ARG_WITH(ross,
[  --with-backstroke-ross=PATH	Specify the path where ROSS is installed],
,
if test ! "$with_backstroke_ross" ; then
   with_backstroke_ross=no
fi
)

AC_MSG_NOTICE([in ROSE SUPPORT MACRO: with_backstroke_ross = "$with_backstroke_ross"])

if test "$with_backstroke_ross" = no; then
   # If BACKSTROKE ROSS is not specified, then don't use it.
   AC_MSG_NOTICE([skipping use of BACKSTROKE ROSS specific support])
else
   if test "$with_backstroke_ross" = yes; then
      # Path not specified...
      AC_MSG_FAILURE([path to ROSS not specified (usage: --with-backstroke-ross=PATH)])
   fi
   backstroke_ross_path=$with_backstroke_ross
   AC_MSG_NOTICE([setup ROSS support in ROSE. path = "$backstroke_ross_path"])
   AC_DEFINE([USE_ROSE_SUPPORT_BACKSTROKE_ROSS],1,[Controls use of ROSE support in Backstroke project for ROSS Discrete Event Simulator.])
   BACKSTROKE_ROSS_INCLUDE="-I$backstroke_ross_path"

   AC_MSG_NOTICE([BACKSTROKE_ROSS_INCLUDE = "$BACKSTROKE_ROSS_INCLUDE"])
fi

AC_SUBST(backstroke_ross_path)
AC_SUBST(BACKSTROKE_ROSS_INCLUDE)


AC_MSG_CHECKING(for SPEEDES Discrete Event Simulator)
AC_ARG_WITH(speedes,
[  --with-backstroke-speedes=PATH	Specify the path where SPEEDES is installed],
,
if test ! "$with_backstroke_speedes" ; then
   with_backstroke_speedes=no
fi
)

AC_MSG_NOTICE([in ROSE SUPPORT MACRO: with_backstroke_speedes = "$with_backstroke_speedes"])

if test "$with_backstroke_speedes" = no; then
   # If BACKSTROKE SPEEDES is not specified, then don't use it.
   AC_MSG_NOTICE([skipping use of BACKSTROKE SPEEDES specific support])
else
   if test "$with_backstroke_speedes" = yes; then
      # Path not specified...
      AC_MSG_FAILURE([path to SPEEDES not specified (usage: --with-backstroke-speedes=PATH)])
   fi
   backstroke_speedes_path=$with_backstroke_speedes
   AC_MSG_NOTICE([setup SPEEDES support in ROSE. path = "$backstroke_speedes_path"])
   AC_DEFINE([USE_ROSE_SUPPORT_BACKSTROKE_SPEEDES],1,[Controls use of ROSE support in Backstroke project for SPEEDES Discrete Event Simulator.])
   BACKSTROKE_SPEEDES_INCLUDE="-I$backstroke_backstroke_path"

   AC_MSG_NOTICE([BACKSTROKE_SPEEDES_INCLUDE = "$BACKSTROKE_SPEEDES_INCLUDE"])
fi

AC_SUBST(backstroke_speedes_path)
AC_SUBST(BACKSTROKE_SPEEDES_INCLUDE)

# End macro ROSE_SUPPORT_BACKSTROKE.
]
)
