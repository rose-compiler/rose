dnl $Id: support-rpath.m4,v 1.6 2007/02/19 16:08:37 vuduc2 Exp $


AC_DEFUN([BTNG_LIBS_ADD_RPATH],[
# Begin macro BTNG_LIBS_ADD_RPATH
dnl Support RPATH by going in a LIBS string and, for each -L flag,
dnl add a flag immediately following it to set the RPATH, for
dnl paths that contain shared libraries.
dnl
dnl arg1 is a LIBS string.
dnl arg2 is the name of the variable to set to the new LIBS string.
dnl arg3 is non-empty to use id of the C++ compiler instead of the C compiler.


dnl Determine which compiler is being used, because
dnl the syntax of the RPATH flag depends on the compiler.
dnl Use the C++ compiler and assume the C compiler
dnl is from the same family.
AC_REQUIRE([BTNG_INFO_CC_CXX_ID])


AC_ARG_ENABLE(rpath,
[  --enable-rpath=SYNTAX	When linking add syntax for rpath for every
			-L option that points to a directory with .so
			files in it.  If SYNTAX is omitted, an attempt
			is made to find out the correct rpath syntax for
			the compiler being used.]
,,enable_rpath=yes)

if test "$enable_rpath" = yes; then
  # Determine the proper rpath syntax.

  AC_LANG_SAVE

  ifelse([$3],,
  AC_LANG_C
  btng_rpath_compiler_id="$CC_ID",
  AC_LANG_CPLUSPLUS
  btng_rpath_compiler_id="$CXX_ID"
  )


  # Unset the rpath syntax variable so we can check on whether we
  # found a way to set it.
  unset btng_rpath_beginning;

  # Determine, based on the compiler, the syntax for specifying RPATH.
  # It should be of the form "$btng_rpath_beginning$the_path", where
  # btng_rpath_beginning is the compiler-dependent part.
  case "$btng_rpath_compiler_id" in
    gnu)
      # This compiler may use a variable rpath syntax because it may use
      # the native loader.
      BTNG_LIBS_FIND_RPATH(btng_rpath_beginning,
	['---bogus-flag-meant-to-cause-error' '-Wl,-rpath ' '-Wl,-R' '-Wl,-R '])
    ;;
    intel)
      # This compiler may use a variable rpath syntax because it may use
      # the native loader.
      BTNG_LIBS_FIND_RPATH(btng_rpath_beginning,
	['---bogus-flag-meant-to-cause-error' '-Wl,-rpath ' '-Wl,-R' '-Wl,-R '])
    ;;
    sunpro)
      # This compiler may use a variable rpath syntax.
      BTNG_LIBS_FIND_RPATH(btng_rpath_beginning,['---bogus-flag-meant-to-cause-error' '-R' '-R '])
    ;;
    kai)
      # The KAI compilers use the system native loader.
      #
      # On some platforms (PC/Linux at least), this compiler seems
      # to return 0 even if it encounters error, thus it can return
      # the first guess for the rpath syntax, even if the guess is
      # wrong.  We try to catch this by making the first flag bogus.
      # If the compiler accepts this flag (by returning 0), we know
      # it is wrong and we resort to an alternative method for
      # getting the rpath syntax.
      BTNG_LIBS_FIND_RPATH(btng_rpath_beginning,
	['---bogus-flag-meant-to-cause-error' '-R' '-R ' '-rpath ' '-Wl,-rpath ' '-Wl,-R' '-Wl,-R '])
      if test "$btng_rpath_beginning" = "---bogus-flag-meant-to-cause-error"; then
        # Do not rely on the compiler return value to test for syntax
        # Guess the syntax assuming the native loader will be used.
        case "$host_os" in
          linux*) btng_rpath_beginning='-Wl,-rpath ' ;;
          sun*|solaris*) btng_rpath_beginning='-R' ;;
          osf*) btng_rpath_beginning='-rpath ' ;;
          *) btng_rpath_beginning='' ;;
        esac
        AC_MSG_WARN(
  [Your compiler ifelse($3,,$CC,$CXX) returns 0 even when it is
  given a bogus flag.  Therefore, I cannot find the proper syntax
  for the rpath for this compiler.  I have resorted to a guess that
  may not be correct: '$btng_rpath_beginning'.
  You can override this by using --enable-rpath=SYNTAX])
      fi
    ;;
    *)
      BTNG_LIBS_FIND_RPATH(btng_rpath_beginning)
    ;;
  esac
  BTNG_AC_LOG_VAR(host_os CC_ID CXX_ID btng_rpath_compiler_id btng_rpath_beginning, forming rpaths)

  AC_LANG_RESTORE

  # It is valid to have btng_rpath_beginning be blank.
  # but if it is unset, we could not find a way to set it.
  if test ! "${btng_rpath_beginning+set}" = set; then
    AC_MSG_WARN(I cannot find a working syntax for setting relocatable paths)
  fi

elif test ! "${enable_rpath}" = no; then

  # User has provide the rpath syntax.
  btng_rpath_beginning=$enable_rpath

fi;	# End block determining the proper rpath syntax.


# Use the rpath syntax.
if test "${btng_rpath_beginning+set}" = set	\
  && test -n "${btng_rpath_beginning}" ; then
  # Add the RPATH flags only if we know the syntax for it,
  # and if it is needed as indicated by a non-empty btng_rpath_beginning.

  # Loop through the flags in $1, looking for the -L flag,
  # and append RPATH flag to each one found, if the the
  # path specified by the flag includes shared libraries.
  for i in ${$1}; do
    btng_new_$2="${btng_new_$2} ${i}"
    btng_tmp_addl_string=`echo $i | sed 's/^-L//'`
    test "$btng_tmp_addl_string" = "$i" && continue	# does not contain -L.
    test -d "$btng_tmp_addl_string" || continue;	# directory nonexistent.
    test "`echo $btng_tmp_addl_string/*.so`" = "$btng_tmp_addl_string/*.so" \
      && continue;	# does not contain shared libraries.
    echo "${btng_new_$2}"	\
      | grep ".*${btng_rpath_beginning}[[ 	]]*${btng_tmp_addl_string}"	\
      > /dev/null	\
      && continue	# already contains the flag we want to add.
    btng_new_$2="${btng_new_$2} ${btng_rpath_beginning}${btng_tmp_addl_string}"
  done
  $2="${btng_new_$2}"

fi

dnl Now, arg2 should be similar to arg1, but with the additional RPATH flags.

# End macro BTNG_LIBS_ADD_RPATH
])




AC_DEFUN([BTNG_LIBS_FIND_RPATH],[
# Begin macro BTNG_LIBS_FIND_RPATH
dnl Find the correct rpath syntax from the list given in arg1.
dnl arg1: variable to set to the syntax string
dnl arg2: list of syntaxes to try;
dnl   if blank, a large number of syntaxes will be tried.
dnl
dnl arg1 is list of possible rpath syntaxes to try.
define(btng_possible_rpaths,dnl
[ifelse($2,,['-R ' '-R' '-rpath ' '-Wl,-rpath ' '-Wl,-R ' '-Wl,-R'],[[$2]])])
  btng_save_LIBS="$LIBS";
  for i in btng_possible_rpaths; do
    LIBS="${i}/usr/local"
    echo "btng_possible_rpaths = $btng_possible_rpaths"
    echo "LIBS = $LIBS i = $i"
    AC_TRY_LINK(,,$1="$i", unset $1)
    echo "status = $?"
    echo "1 = $1"
    if test "${$1+set}" = set; then break; fi
  done
  LIBS="$btng_save_LIBS"
undefine([btng_possible_rpaths])
# End macro BTNG_LIBS_FIND_RPATH
])
