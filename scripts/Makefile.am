include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# copy the tools (scripts only) from the source tree to the source destination in the distribution
distributedScripts = \
     configure_with_SQL     \
     remote-check-dist-file        standard-remote-check-dist-file \
     cvschk                        find_cvs_conflicts     \
     printConstructors             runscript              \
     dotgen                        rcdf-config            \
     dotgen2                       lsdot                  \
     check-dist-file dotgenLetter  generateDirectoryGraph \
     cvschk                        find_cvs_conflicts     \
     libtoolcp     \
     dotgenLetterPDF dotgenLetterPNG

# scripts put into the distribution
EXTRA_DIST = README $(distributedScripts)

# install the tools in 'bin' (a subsect of EXTRA_DIST)
bin_SCRIPTS =  $(distributedScripts)

