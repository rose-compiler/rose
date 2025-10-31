


########### install files ###############

install(FILES  DominatorTree.h filteredCFG.h DominanceFrontier.h DominatorTreeImpl.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files cxx_templates
#
#distclean-local:
#	rm -rf Templates.DB
#
#include_HEADERS =  DominatorTree.h filteredCFG.h DominanceFrontier.h DominatorTreeImpl.h
#
