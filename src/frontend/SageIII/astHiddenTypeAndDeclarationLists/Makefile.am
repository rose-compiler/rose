include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = libastHiddenTypeAndDeclarationLists.la

# DQ (6/22/2011): I think all of these files are no longer required in ROSE.
# libastHiddenTypeAndDeclarationLists_la_SOURCES = HiddenList.C  newHiddenList.C  HiddenList_Intersection.C HiddenList_Output.C NamespacesAndClassTraversal.C
libastHiddenTypeAndDeclarationLists_la_SOURCES = HiddenList.C  HiddenList_Intersection.C HiddenList_Output.C NamespacesAndClassTraversal.C

# HiddenList_main.C  HiddenList_Output.C

libastHiddenTypeAndDeclarationLists_la_LIBADD       = 
libastHiddenTypeAndDeclarationLists_la_DEPENDENCIES = 

# DQ (6/22/2011): I think all of these files are no longer required in ROSE.
include_HEADERS = HiddenList.h  HiddenList_Intersection.h  HiddenList_Output.h

EXTRA_DIST = CMakeLists.txt
