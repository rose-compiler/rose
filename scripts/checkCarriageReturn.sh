#/bin/bash -vx
if test "`find $1 -regex ".*\(\.h\|\.c\|\.C\|\.h\.in\)$" -exec grep -nH  {} \;`"; then
	exit 1;
fi;
