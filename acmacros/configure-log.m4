AC_DEFUN([BTNG_AC_LOG],[echo "configure:__oline__:" $1 >&AC_FD_CC])

AC_DEFUN([BTNG_AC_LOG_VAR],[
dnl arg1 is list of variables to log.
dnl arg2 (optional) is a label.
dnl
dnl This macro makes code that write out at configure time
dnl label: x is '...'
dnl if x is set and
dnl label: x is unset
dnl otherwise.
define([btng_log_label],ifelse($2,,,[$2: ]))
btng_log_vars="$1"
for btng_log_vars_index in $btng_log_vars ; do
  eval "test \"\${${btng_log_vars_index}+set}\" = set"
  if test $? = 0; then
    btng_log_vars_value="'`eval echo \\${$btng_log_vars_index}`'";
  else
    btng_log_vars_value="unset";
  fi
  BTNG_AC_LOG("btng_log_label$btng_log_vars_index is $btng_log_vars_value");
dnl
dnl This is a shorter version, but it does not work for some Bourne shells
dnl due to misinterpretation of the multiple backslashes
dnl BTNG_AC_LOG("btng_log_label$btng_log_vars_index is `eval if test \\\"\$\{$btng_log_vars_index+set\}\\\"\; then echo \\\""'"\$\{$btng_log_vars_index\}"'"\\\"\; else echo 'unset'\; fi`")
done
undefine([btng_log_label])
])
