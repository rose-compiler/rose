INCLUDES = -I$(top_srcdir)/src/ListT @SAGE_INCLUDES@ @STL_INCLUDES@ -I$(top_srcdir)/src/Padding -I$(top_srcdir)/include -I$(top_srcdir)/src/Transform_2 -I$(top_srcdir)/src -I$(top_builddir)/include @INCLUDES@ -I$(top_srcdir)/src/unparser -I$(top_srcdir)/src/TransformBaseClass
noinst_LIBRARIES = libpadtransform.a
libpadtransform_a_SOURCES = ArrayPaddingTransform.C aptScope.C arrayDeclInfo.C arrayRefInfo.C
noinst_HEADERS = aptScope.h aptScopeType.h arrayDeclInfo.h ArrayPaddingTransform.h arrayRefInfo.h pad_transform.h
AM_CPPFLAGS = -g

clean-local:
	rm -rf Templates.DB ii_files ti_files
