include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

if DEV_LIBS
lib_LTLIBRARIES = libastHiddenTypeAndDeclarationLists.la
else
noinst_LTLIBRARIES = libastHiddenTypeAndDeclarationLists.la
endif

libastHiddenTypeAndDeclarationLists_la_SOURCES      = \
     HiddenList.C  HiddenList_Intersection.C HiddenList_Output.C NamespacesAndClassTraversal.C

# HiddenList_main.C  HiddenList_Output.C

libastHiddenTypeAndDeclarationLists_la_LIBADD       = 
libastHiddenTypeAndDeclarationLists_la_DEPENDENCIES = 

include_HEADERS = \
     HiddenList.h  HiddenList_Intersection.h  HiddenList_Output.h
