# Taken from CCAIN (http://sourceforge.net/projects/ccain).

AC_DEFUN([OFP_CONFIGURE_SETUP],[

# Some helper script functions.  Unfortunately, we cannot use $1 kinds
# of arugments here because of the m4 substitution.  So we have to set
# special variable names before invoking the function.  :-\

ofp_show_title() {
  cat <<EOF

============================================================================
== ${1}
============================================================================
EOF
}


ofp_show_subtitle() {
  cat <<EOF

*** ${1}
EOF
}
#
# Save some stats about this build
#

OFP_CONFIGURE_USER="`whoami`"
OFP_CONFIGURE_HOST="`hostname | head -n 1`"
OFP_CONFIGURE_DATE="`date`"

#
# Save these details so that they can be used in ofp_info later
#
AC_SUBST(OFP_CONFIGURE_USER)
AC_SUBST(OFP_CONFIGURE_HOST)
AC_SUBST(OFP_CONFIGURE_DATE)])dnl

dnl #######################################################################
dnl #######################################################################
dnl #######################################################################

AC_DEFUN([OFP_BASIC_SETUP],[
#
# Save some stats about this build
#

OFP_CONFIGURE_USER="`whoami`"
OFP_CONFIGURE_HOST="`hostname | head -n 1`"
OFP_CONFIGURE_DATE="`date`"

#
# This is useful later (ofp_info, and therefore mpiexec)
#

AC_CANONICAL_HOST
AC_DEFINE_UNQUOTED(OFP_ARCH, "$host", [OFP architecture string])

#
# Basic sanity checking; we can't install to a relative path
#

case "$prefix" in
  /*/bin)
    prefix="`dirname $prefix`"
    echo installing to directory \"$prefix\" 
    ;;
  /*) 
    echo installing to directory \"$prefix\" 
    ;;
  NONE)
    echo installing to directory \"$ac_default_prefix\" 
    ;;
  *) 
    AC_MSG_ERROR(prefix \"$prefix\" must be an absolute directory path) 
    ;;
esac

# Allow the --enable-dist flag to be passed in

AC_ARG_ENABLE(dist, 
    AC_HELP_STRING([--enable-dist],
		   [guarantee that that the "dist" make target will be functional, although may not guarantee that any other make target will be functional.]),
    OFP_WANT_DIST=yes, OFP_WANT_DIST=no)

if test "$OFP_WANT_DIST" = "yes"; then
    AC_MSG_WARN([Configuring in 'make dist' mode])
    AC_MSG_WARN([Most make targets may be non-functional!])
fi])dnl

dnl #######################################################################
dnl #######################################################################
dnl #######################################################################

AC_DEFUN([OFP_LOG_MSG],[
# 1 is the message
# 2 is whether to put a prefix or not
if test -n "$2"; then
    echo "configure:__oline__: $1" >&5
else
    echo $1 >&5
fi])dnl

dnl #######################################################################
dnl #######################################################################
dnl #######################################################################

AC_DEFUN([OFP_LOG_FILE],[
# 1 is the filename
if test -n "$1" -a -f "$1"; then
    cat $1 >&5
fi])dnl

dnl #######################################################################
dnl #######################################################################
dnl #######################################################################

AC_DEFUN([OFP_LOG_COMMAND],[
# 1 is the command
# 2 is actions to do if success
# 3 is actions to do if fail
echo "configure:__oline__: $1" >&5
$1 1>&5 2>&1
ofp_status=$?
OFP_LOG_MSG([\$? = $ofp_status], 1)
if test "$ofp_status" = "0"; then
    unset ofp_status
    $2
else
    unset ofp_status
    $3
fi])dnl

dnl #######################################################################
dnl #######################################################################
dnl #######################################################################

AC_DEFUN([OFP_UNIQ],[
# 1 is the variable name to be uniq-ized
ofp_name=$1

# Go through each item in the variable and only keep the unique ones

ofp_count=0
for val in ${$1}; do
    ofp_done=0
    ofp_i=1
    ofp_found=0

    # Loop over every token we've seen so far

    ofp_done="`expr $ofp_i \> $ofp_count`"
    while test "$ofp_found" = "0" -a "$ofp_done" = "0"; do

	# Have we seen this token already?  Prefix the comparison with
	# "x" so that "-Lfoo" values won't be cause an error.

	ofp_eval="expr x$val = x\$ofp_array_$ofp_i"
	ofp_found=`eval $ofp_eval`

	# Check the ending condition

	ofp_done="`expr $ofp_i \>= $ofp_count`"

	# Increment the counter

	ofp_i="`expr $ofp_i + 1`"
    done

    # If we didn't find the token, add it to the "array"

    if test "$ofp_found" = "0"; then
	ofp_eval="ofp_array_$ofp_i=$val"
	eval $ofp_eval
	ofp_count="`expr $ofp_count + 1`"
    else
	ofp_i="`expr $ofp_i - 1`"
    fi
done

# Take all the items in the "array" and assemble them back into a
# single variable

ofp_i=1
ofp_done="`expr $ofp_i \> $ofp_count`"
ofp_newval=
while test "$ofp_done" = "0"; do
    ofp_eval="ofp_newval=\"$ofp_newval \$ofp_array_$ofp_i\""
    eval $ofp_eval

    ofp_eval="unset ofp_array_$ofp_i"
    eval $ofp_eval

    ofp_done="`expr $ofp_i \>= $ofp_count`"
    ofp_i="`expr $ofp_i + 1`"
done

# Done; do the assignment

ofp_newval="`echo $ofp_newval`"
ofp_eval="$ofp_name=\"$ofp_newval\""
eval $ofp_eval

# Clean up

unset ofp_name ofp_i ofp_done ofp_newval ofp_eval ofp_count])dnl

