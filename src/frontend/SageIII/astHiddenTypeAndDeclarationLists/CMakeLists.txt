
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


#add_library( libastHiddenTypeAndDeclarationLists STATIC ${libastHiddenTypeAndDeclarationLists_SRC}  )
#astHiddenTypeAndDecladd_dependencies( libastHiddenTypeAndDeclarationLists ROSETTA )

########### install files ###############

install(FILES  HiddenList.h  HiddenList_Intersection.h  HiddenList_Output.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES)
#
#noinst_LTLIBRARIES = libastHiddenTypeAndDeclarationLists.la
#
#libastHiddenTypeAndDeclarationLists_la_SOURCES      = \
#     HiddenList.C  HiddenList_Intersection.C HiddenList_Output.C NamespacesAndClassTraversal.C
#
## HiddenList_main.C  HiddenList_Output.C
#
#libastHiddenTypeAndDeclarationLists_la_LIBADD       = 
#libastHiddenTypeAndDeclarationLists_la_DEPENDENCIES = 
#
#include_HEADERS = \
#     HiddenList.h  HiddenList_Intersection.h  HiddenList_Output.h
