include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Use --with-libxml2 on configure command-line for ROSE to activate this

if ROSE_USE_BINARYCONTEXTLOOKUP

INCLUDES = $(ROSE_INCLUDES) $(XML_CPPFLAGS)

SAMPLE_INPUT_DIR = $(srcdir)

bin_PROGRAMS = bcl_writer bcl_reader

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(XML_LIBS)

bcl_writer_SOURCES = bcl_writer.C

bcl_reader_SOURCES = bcl_reader.C

# If $(srcdir) is a relative path then we need this version
# becuae filenames in Sg_File_Info are absolute paths in ROSE.
old_mkSearch:
	g++ -O3 -g $(srcdir)/example1.C -o example1.bin
	objdump -ld example1.bin | grep '^/' > example1.in
	echo "$(srcdir)/example1.C:4000" >> example1.in
	echo "/not_a_file.goo:1" >> example1.in
	g++ -O3 -g $(srcdir)/example2.C -o example2.bin
	objdump -ld example2.bin | grep '^/' > example2.in
	echo "$(srcdir)/example2.C:4000" >>example2.in
	echo "$(srcdir)/example2.C:4000" >> example2.in
	echo "/not_a_file.goo:1" >> example2.in
	g++ -O3 -g $(srcdir)/example3.1.C $(srcdir)/example3.2.C -o example3.bin
	objdump -ld example3.bin | grep '^/' > example3.in
	echo "$(srcdir)/example3.1.C:4000" >> example3.in
	echo "/not_a_file.goo:1" >> example3.in

# Use this version if  $(srcdir) is an absolute path
mkSearch:
	g++ -O3 -g $(srcdir)/example1.C -o example1.bin;
	objdump -ld example1.bin | grep "example1.C" > example1.in;
	echo "$(srcdir)/example1.C:4000" >> example1.in;
	echo "/not_a_file.goo:1" >> example1.in;
	g++ -O3 -g $(srcdir)/example2.C -o example2.bin;
	objdump -ld example2.bin | grep "example2.C" > example2.in;
	echo "$(srcdir)/example2.C:4000" >>example2.in;
	echo "$(srcdir)/example2.C:4000" >> example2.in;
	echo "/not_a_file.goo:1" >> example2.in;
	g++ -O3 -g $(srcdir)/example3.1.C $(srcdir)/example3.2.C -o example3.bin;
	objdump -ld example3.bin | grep -E "example3\..\.C" > example3.in;
	echo "$(srcdir)/example3.1.C:4000" >> example3.in;
	echo "/not_a_file.goo:1" >> example3.in;

check-local: bcl_writer bcl_reader mkSearch
	./bcl_writer $(srcdir)/example1.C -o example1.xml
	./bcl_reader example1.xml < example1.in
	./bcl_writer $(srcdir)/example2.C -o example2.xml
	./bcl_reader example2.xml < example2.in
	./bcl_writer $(srcdir)/example3.1.C $(srcdir)/example3.2.C -o example3.xml
	./bcl_reader example3.xml < example3.in

EXTRA_DIST = README example1.C example2.C example3.1.C example3.2.C example.g++.param example.objdump.param 

else
# DQ (1/15/2007): The Intel compiler does not have a version of libxml2 usable by default 
# in /usr/lib under Linux, but GNU does (we this needs to be specificed explicltly).
check-local:
	@echo "\nBinaryContextLookup tests NOT run (depends upon availablity of libxml2).  To run these reconfigure with:"
	@echo "**********************************************************************************************************"
	@echo "********************* make check not run here: configure using --with-xml=yes option *********************"
	@echo "**********************************************************************************************************"

# Add the source files so that they will always be distributed
EXTRA_DIST = bcl_writer.C bcl_reader.C README example1.C example2.C example3.1.C example3.2.C example.g++.param example.objdump.param 

endif

clean-local:
	rm -f a.out rose_*.C *.xml *.bin example*.in

