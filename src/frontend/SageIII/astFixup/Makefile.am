include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = libastFixup.la

libastFixup_la_SOURCES      = \
     AstFixup.C \
     fixupforGnuBackendCompiler.C \
     fixupInClassDataInitialization.C \
     removeInitializedNamePtr.C \
     fixupDeclarations2.C \
     fixupFunctionDefinitions.C \
     fixupStorageAccessOfForwardTemplateDeclarations.C \
     fixupSourcePositionInformation.C \
     fixupEnumValues.C \
     fixupFriendTemplateDeclarations.C \
     fixupPrettyFunction.C


libastFixup_la_LIBADD       = 
libastFixup_la_DEPENDENCIES = 

include_HEADERS = \
     AstFixup.h \
     fixupFunctionDefinitions.h \
     fixupStorageAccessOfForwardTemplateDeclarations.h \
     removeInitializedNamePtr.h \
     fixupforGnuBackendCompiler.h \
     fixupInClassDataInitialization.h \
     fixupTemplateDeclarations.h \
     fixupSourcePositionInformation.h \
     fixupEnumValues.h \
     fixupFriendTemplateDeclarations.h \
     fixupPrettyFunction.h

EXTRA_DIST = CMakeLists.txt
