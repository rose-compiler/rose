include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# copy the tools (scripts only) from the source tree to the source destination in the distribution
distributedScripts = \
     configure_with_SQL            runscript              \
     dotgen                                               \
     dotgen2                       lsdot                  \
     check-dist-file dotgenLetter  generateDirectoryGraph \
     libtoolcp     \
     dotgenLetterPDF dotgenLetterPNG \
     nmi

# scripts put into the distribution
EXTRA_DIST = README $(distributedScripts)

# install the tools in 'bin' (a subsect of EXTRA_DIST)
bin_SCRIPTS =  $(distributedScripts)

