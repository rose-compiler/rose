AC_DEFUN([ROSE_SUPPORT_MYSQL],
[
# Begin macro ROSE_SUPPORT_MYSQL.

# Inclusion of test for MySQL and it's location.
# We have no control over where the database is located (placed by MySQL)
# so we don't need to specify that.  We do have to specify:
#  1) server name (default: localhost)
#  2) username: (default: root)
#  3) password: (default: rose)
#  4) database name: (default: rosedb)

AC_MSG_CHECKING(for MySQL)
AC_ARG_WITH(MySQL,
[  --with-MySQL=PATH	Specify the prefix where MySQL & MySQL++ is installed],
,
if test ! "$with_MySQL" ; then
   with_MySQL=no
fi
)

AC_ARG_WITH(MySQL_server,
[  --with-MySQL_server=MACHINE_NAME	Specify the MySQL database server to be used],
,
if test ! "$with_MySQL_server" ; then
  if test -n "${HOST}" ; then
    with_MySQL_server="$HOST"
  elif test -n "${HOSTNAME}" ; then
    with_MySQL_server="$HOSTNAME"
  else
    with_MySQL_server="localhost"
  fi
fi
)

AC_ARG_WITH(MySQL_username,
[  --with-MySQL_username=USER_NAME	Specify the MySQL username to be used],
,
if test ! "$with_MySQL_username" ; then
  if test -n "${USERNAME}" ; then
    with_MySQL_username="$USERNAME"
  elif test -n "${USER}" ; then
    with_MySQL_username="$USER"
  else
    with_MySQL_username="root"
  fi
fi
)

AC_ARG_WITH(MySQL_password,
[  --with-MySQL_password=PASSWORD	Specify the MySQL password to be used],
,
if test ! "$with_MySQL_password" ; then
   with_MySQL_password="rosepwd"
fi
)

AC_ARG_WITH(MySQL_database_name,
[  --with-MySQL_database_name=DATABASE_NAME	Specify the MySQL database name to be used],
,
if test ! "$with_MySQL_database_name" ; then
   with_MySQL_database_name="rosedb"
fi
)

echo "In ROSE SUPPORT MACRO: with_MySQL $with_MySQL"

if test "$with_MySQL" = no; then
   # If MySQL is not specified, then don't use it.
   echo "Skipping use of MySQL!"
else
   optional_DataBase_subdirs="DataBase"
 # MySQL_login_string="localhost root rose"
#  MySQL_server="localhost"
#  MySQL_username="root"
#  MySQL_password="rose"
#  MySQL_database_name="rosedb"

   MySQL_path=$with_MySQL
   MySQL_server=$with_MySQL_server
   MySQL_username=$with_MySQL_username
   MySQL_password=$with_MySQL_password
   MySQL_database_name=$with_MySQL_database_name

   echo "Setup MySQL DataBase! path          = $MySQL_path"
   echo "Setup MySQL DataBase! server        = $MySQL_server"
   echo "Setup MySQL DataBase! username      = $MySQL_username"
   echo "Setup MySQL DataBase! password      = $MySQL_password"
   echo "Setup MySQL DataBase! database name = $MySQL_database_name"
   echo "Setup MySQL DataBase! optional_DataBase_subdirs = $optional_DataBase_subdirs"

   AC_CONFIG_SUBDIRS(Projects/DataBase)
fi

AC_SUBST(optional_DataBase_subdirs)
AC_SUBST(MySQL_login_string)
AC_SUBST(MySQL_path)
AC_SUBST(MySQL_server)
AC_SUBST(MySQL_username)
AC_SUBST(MySQL_password)
AC_SUBST(MySQL_database_name)

# End macro ROSE_SUPPORT_MYSQL.
]
)
