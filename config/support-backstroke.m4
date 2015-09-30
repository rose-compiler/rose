
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

echo "In ROSE SUPPORT MACRO: with_backstroke_ross $with_backstroke_ross"

if test "$with_backstroke_ross" = no; then
   # If BACKSTROKE ROSS is not specified, then don't use it.
   echo "Skipping use of BACKSTROKE ROSS specific support!"
else
   if test "$with_backstroke_ross" = yes; then
      # Path not specified...
      echo "Error: Path to ROSS not specified...(usage: --with-backstroke-ross=PATH)"
      exit 1
   fi
   backstroke_ross_path=$with_backstroke_ross
   echo "Setup ROSS support in ROSE! path = $backstroke_ross_path"
   AC_DEFINE([USE_ROSE_SUPPORT_BACKSTROKE_ROSS],1,[Controls use of ROSE support in Backstroke project for ROSS Discrete Event Simulator.])
   BACKSTROKE_ROSS_INCLUDE="-I$backstroke_ross_path"

   echo "BACKSTROKE_ROSS_INCLUDE = $BACKSTROKE_ROSS_INCLUDE"
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

echo "In ROSE SUPPORT MACRO: with_backstroke_speedes $with_backstroke_speedes"

if test "$with_backstroke_speedes" = no; then
   # If BACKSTROKE SPEEDES is not specified, then don't use it.
   echo "Skipping use of BACKSTROKE SPEEDES specific support!"
else
   if test "$with_backstroke_speedes" = yes; then
      # Path not specified...
      echo "Error: Path to SPEEDES not specified...(usage: --with-backstroke-speedes=PATH)"
      exit 1
   fi
   backstroke_speedes_path=$with_backstroke_speedes
   echo "Setup SPEEDES support in ROSE! path = $backstroke_speedes_path"
   AC_DEFINE([USE_ROSE_SUPPORT_BACKSTROKE_SPEEDES],1,[Controls use of ROSE support in Backstroke project for SPEEDES Discrete Event Simulator.])
   BACKSTROKE_SPEEDES_INCLUDE="-I$backstroke_backstroke_path"

   echo "BACKSTROKE_SPEEDES_INCLUDE = $BACKSTROKE_SPEEDES_INCLUDE"
fi

AC_SUBST(backstroke_speedes_path)
AC_SUBST(BACKSTROKE_SPEEDES_INCLUDE)

# End macro ROSE_SUPPORT_BACKSTROKE.
]
)
