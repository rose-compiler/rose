AC_DEFUN([ROSE_SUPPORT_GIT_VERSIONING],
[
# Begin macro ROSE_SUPPORT_GIT_VERSIONING.
CONFIGURE_DIR="[$(dirname $][0)]"
GIT_DIR="$CONFIGURE_DIR"/.git
VERSION_FILE="$CONFIGURE_DIR"/VERSION

GIT_HEAD_HASH=
GIT_HEAD_DATE=
GIT_HASH_LENGTH=40
GIT_HASH_REGEX="^[[0-9a-zA-Z]]{40}$"
GIT_DATE_REGEX="^[[0-9]]{10,}$"

AC_MSG_CHECKING([for ROSE Git version information in '$CONFIGURE_DIR'])

if test -e "$GIT_DIR"; then
  if test -n "$(which git)"; then
    GIT_HEAD_HASH="$(cd "$CONFIGURE_DIR" && git rev-parse HEAD)"
    GIT_HEAD_DATE="$(cd "$CONFIGURE_DIR" && git log -1 --format="%at" "$GIT_HEAD_HASH")"
  else
    AC_MSG_FAILURE([git executable not found])
  fi
else
  if test -e "$VERSION_FILE"; then
      GIT_HEAD_HASH=$(head -n1 "$VERSION_FILE" | cut -d' ' -f1)
      GIT_HEAD_DATE=$(head -n1 "$VERSION_FILE" | cut -d' ' -f2)
  else
      AC_MSG_NOTICE([])
      AC_MSG_WARN([You are using a non-Git version of ROSE that lacks a VERSION file. This may be])
      AC_MSG_WARN([due to you having downloaded a ZIP archive of the ROSE repository from github.])

      dnl The new instructions, as of 2016-05-18
      AC_MSG_WARN([ROSE no longer supports being installed from a github ZIP archive; your ROSE])
      AC_MSG_WARN([source tree must contain Git version information, i.e., a ".git" directory.])

      AC_MSG_NOTICE([])
      AC_MSG_FAILURE([$VERSION_FILE file does not exist])
  fi
fi

# Hash length
HASH_LENGTH="${#GIT_HEAD_HASH}"
if test "$HASH_LENGTH" -ne "$GIT_HASH_LENGTH"; then
  AC_MSG_FAILURE([invalid Git hash length=$HASH_LENGTH: $GIT_HEAD_HASH])
fi

# Hash format
if ! [[[ "$GIT_HEAD_HASH" =~ $GIT_HASH_REGEX ]]]; then
  AC_MSG_FAILURE([invalid Git hash does not match regex="$GIT_HASH_REGEX": $GIT_HEAD_HASH])
fi

# Date format
if ! [[[ "$GIT_HEAD_DATE" =~ $GIT_DATE_REGEX ]]]; then
  AC_MSG_FAILURE([invalid Git author date does not match regex="$GIT_DATE_REGEX": $GIT_HEAD_HASH])
fi

AC_DEFINE_UNQUOTED([ROSE_SCM_VERSION_ID], ["$GIT_HEAD_HASH"], [Git SCM version information for ROSE: commit identifier.])
AC_DEFINE_UNQUOTED([ROSE_SCM_VERSION_UNIX_DATE], [$GIT_HEAD_DATE], [Git SCM version information for ROSE: commit date (Unix timestamp).])
AC_MSG_RESULT([$GIT_HEAD_HASH $GIT_HEAD_DATE])
])
