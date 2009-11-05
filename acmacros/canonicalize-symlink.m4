dnl Params are:
dnl $1 Name of variable to receive canonicalized filename
dnl $2 Non-canonical filename
AC_DEFUN([ROSE_CANON_SYMLINK], [
	$1=$2
	while test -h "$$1" ; do
		__rose_canon_symlink_base="`readlink "$$1"`"
		AS_SET_CATFILE([__rose_canon_symlink_full_path], ["`dirname "$$1"`"], ["$__rose_canon_symlink_base"])
		__rose_canon_symlink_dir="`dirname "$__rose_canon_symlink_full_path"`"
		__rose_canon_symlink_canon_dir="`cd "$__rose_canon_symlink_dir" && pwd`"
		AS_SET_CATFILE([$1], ["$__rose_canon_symlink_canon_dir"], ["`basename $__rose_canon_symlink_full_path`"])
	done
])
