include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = libastHiddenTypeAndDeclarationLists.la

libastHiddenTypeAndDeclarationLists_la_SOURCES      = \
     HiddenList.C  newHiddenList.C  HiddenList_Intersection.C HiddenList_Output.C NamespacesAndClassTraversal.C

# HiddenList_main.C  HiddenList_Output.C

libastHiddenTypeAndDeclarationLists_la_LIBADD       = 
libastHiddenTypeAndDeclarationLists_la_DEPENDENCIES = 

include_HEADERS = \
     HiddenList.h  HiddenList_Intersection.h  HiddenList_Output.h

EXTRA_DIST = CMakeLists.txt
