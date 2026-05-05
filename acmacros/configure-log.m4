AC_DEFUN([BTNG_AC_LOG],
[AS_ECHO(["configure:$LINENO: $1"]) >&AS_MESSAGE_LOG_FD])

AC_DEFUN([BTNG_AC_LOG_VAR],[
dnl arg1 is list of variables to log.
dnl arg2 (optional) is a label.
dnl
dnl This macro makes code that write out at configure time
dnl label: x is '...'
dnl if x is set and
dnl label: x is unset
dnl otherwise.
m4_define([btng_log_label], m4_ifblank([$2], [], [$2: ]))
btng_log_vars="$1"
for btng_log_vars_index in $btng_log_vars ; do
    eval "test \"\${${btng_log_vars_index}+set}\" = set"
    if test $? = 0; then
        btng_log_vars_value="'`eval echo \\${$btng_log_vars_index}`'";
    else
        btng_log_vars_value="unset";
    fi
    BTNG_AC_LOG("btng_log_label$btng_log_vars_index is $btng_log_vars_value");
done
m4_undefine([btng_log_label])
])
