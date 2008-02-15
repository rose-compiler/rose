AC_DEFUN([ROSE_SUPPORT_SQLITE],
[
# Begin macro ROSE_SUPPORT_SQLITE.

# Inclusion of test for SQLite and it's location.
# Note that SQLite is much simpler than MySQL and so we don't have to
# worry about servers, or passwords, or usernames, or the database name.

AC_MSG_CHECKING(for SQLite)
AC_ARG_WITH(SQLite,
[  --with-SQLite=PATH	Specify the prefix where SQLite is installed],
,
if test ! "$with_SQLite" ; then
   with_SQLite=no
fi
)

echo "In ROSE SUPPORT MACRO: with_SQLite $with_SQLite"

if test "$with_SQLite" = no; then
   # If SQLite is not specified, then don't use it.
   echo "Skipping use of SQLite!"
else
   SQLite_path=$with_SQLite
   echo "Setup SQLite DataBase! path = $SQLite_path"
   AC_DEFINE([USE_ROSE_SQL_DATABASE_SUPPORT],1,[Controls use of ROSE SQLite Database Support.])
fi

# echo "Specify use of ROSE SQLite Database support SQL_DATABASE_SUPPORT = $SQL_DATABASE_SUPPORT"
# export SQL_DATABASE_SUPPORT
# AC_DEFINE([USE_ROSE_SQL_DATABASE_SUPPORT],SQL_DATABASE_SUPPORT,[Controls use of ROSE SQLite Database Support.])

AC_SUBST(SQLite_path)

# End macro ROSE_SUPPORT_SQLITE.
]
)
