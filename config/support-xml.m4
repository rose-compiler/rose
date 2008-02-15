AC_DEFUN([ROSE_SUPPORT_XML],
[
AC_PREREQ([2.57])

dnl Begin macro ROSE_SUPPORT_XML.

# DQ (1/15/2007): This is a better macro that the one I wrote below!

dnl Inclusion of test for libxml2 and it's location.

AC_MSG_CHECKING(for xml)
AC_ARG_WITH(xml,
[  --with-xml=PATH	Specify the prefix where libxml2 is installed],
,
if test ! "$with_xml" ; then
   with_xml=no
fi
)

echo "In ROSE SUPPORT MACRO: with_xml $with_xml"

if test "$with_xml" = no; then
   # If libxml2 is not specified, then don't use it.
   echo "Skipping use of XML support!"
else
   xml_path=$with_xml
   echo "Setup XML support in ROSE! path = $xml_path"
   AC_DEFINE([USE_ROSE_XML_SUPPORT],1,[Controls use of ROSE support for XML (libxml2).])
fi

AC_SUBST(xml_path)

# End macro ROSE_SUPPORT_XML.
]
)


# AC_DEFUN([ROSE_SUPPORT_XML],
# [
# Begin macro ROSE_SUPPORT_ XML.
# Later this macro should permit a path to be specified (where we can find the xml2 library).
# For now we will assume it is in the /usr/lib locations, as it is typlically under Linux.
# This assumption is problematic when using the Intel compilers however so we need this
# configure option to avoid generalization (and failing with the Intel compilers).
# AC_ARG_WITH(xml, [  --with_xml=yes Specify if xml is available in the default /usr/lib directory (typical for Linux)])
# if (test "$with_xml" = yes); then
#   echo "Default xml being used (linux for example has it is /usr/lib)"
# else
#   echo "Default xml if available will not be used (not all systems have it, e.g. Intel compiler)"
# fi
# End macro ROSE_SUPPORT_ XML.
# ])
