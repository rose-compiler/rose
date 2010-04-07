
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


#add_library( astFixup ${astFixup_SRC} )
#add_dependencies( astFixup ROSETTA )
########### install files ###############

install(FILES  AstFixup.h fixupFunctionDefinitions.h
    fixupStorageAccessOfForwardTemplateDeclarations.h removeInitializedNamePtr.h
    fixupforGnuBackendCompiler.h fixupInClassDataInitialization.h
    fixupTemplateDeclarations.h fixupSourcePositionInformation.h
    fixupEnumValues.h fixupFriendTemplateDeclarations.h
    fixupPrettyFunction.h fixupPrettyFunction.h 
    DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES)
#
#noinst_LTLIBRARIES = libastFixup.la
#
#libastFixup_la_SOURCES      = \
#     AstFixup.C \
#     fixupforGnuBackendCompiler.C \
#     fixupInClassDataInitialization.C \
#     removeInitializedNamePtr.C \
#     fixupDeclarations2.C \
#     fixupFunctionDefinitions.C \
#     fixupStorageAccessOfForwardTemplateDeclarations.C \
#     fixupSourcePositionInformation.C \
#     fixupEnumValues.C \
#     fixupFriendTemplateDeclarations.C
#
#libastFixup_la_LIBADD       = 
#libastFixup_la_DEPENDENCIES = 
#
#include_HEADERS = \
#     AstFixup.h \
#     fixupFunctionDefinitions.h \
#     fixupStorageAccessOfForwardTemplateDeclarations.h \
#     removeInitializedNamePtr.h \
#     fixupforGnuBackendCompiler.h \
#     fixupInClassDataInitialization.h \
#     fixupTemplateDeclarations.h \
#     fixupSourcePositionInformation.h \
#     fixupEnumValues.h \
#     fixupFriendTemplateDeclarations.h
