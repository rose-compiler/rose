# This directory is for experimentation with ideas specific to
# how we can use Doxygen to better document the ROSE project.

logo:
#	Copy the logo file to the this directory
	mkdir -p ROSE_HTML_Reference
#	Copy to the current directory where doxygen will be run so that doxygen 
#	can copy it to the where it will put the web pages (./ROSE_HTML_Reference)
	cp @top_srcdir@/docs/Rose/compass_rose.gif .

docs: logo copyPublications test.cfg
	doxygen test.cfg

# DQ (5/8/2005): Added custon headers and footers for HTML generated documentation
# See use of HTML_HEADER and HTML_FOOTER in test.cfg.in file
DIST_EXTRA = htmlHeaderFile.html htmlFooterFile.html test.cfg.in

# DQ (1/25/2006): Renamed clean rule since we don't want "make clean" 
# at the top level directory to always remove this documentation.
cleandocs:
	rm -rf ROSE_HTML_Reference ROSE_ResearchPapers test.cfg *.ps *.gif

# This allows us to test the development of the 
# documenttion specific to the publications page.
copyPublications:
	cp -r $(top_srcdir)/ROSE_ResearchPapers .

# This copies the experimental version of the documentation to the
# location where it will permanently reside in the source tree.
copyDocumentation:
	cp $(srcdir)/*.docs $(top_srcdir)/src/frontend/SageIII/docs
