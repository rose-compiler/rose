# Makefile for using with unix
# To use this file type "make -f unix.mk [entry-point]"
# or copy it to "Makefile" to avoid using the "-f unix.mk" option
# Entry Points:
# all		(Default) Show Help
# all1   	builds all
# clean         Clean all object and executable files
# dos2unix      Fix source files that have any CR/LF -> LF
# fix_2_cpp     Fix file extensions of C++ files to .cpp
# fix_2_cxx     Fix file extensions of C++ files to .cxx

# linux		Set environment variables for Linux (Slackware 3)
# solaris       Set environment variables for Sun with Solaris 2.X
#		and gnu C/C++

MAKE         = gmake
DOS2UNIX     = ./dos2unix.sh
SUBDIRS      = sources taste cplus2 taste_cp
SUBDIRS2     = docs frames frames/cplus2 cplus2 samples

all:
		$(MAKE) -s -f unix.mk help

help:
		echo
		echo "----------------------------------------------------";\
		echo "Use the dos2unix entry point first to avoid problems";\
		echo "----------------------------------------------------";\
		echo "";\
		echo "Use one of the following entry points:";\
		echo "";\
		echo "clean     remove objects and executables";\
                echo "linux     for Linux (slackware 3, etc)";\
		echo "solaris   for Sun with Solaris 2.X and GNU C/C++";\
		echo "all1      for generic unix (cc is the default compiler)";\
		echo "";\
		echo "Other entries to make the distribution more portable:";\
		echo "dos2unix  fix the Dos CR/LF to Unix LF on source files";\
                echo "fix_2_cpp fix the C++ file extension to .cpp";\
                echo "fix_2_cxx fix the C++ file extension to .cxx";\
		echo "";\
		echo "----------------------------------------------------";\
		echo "Use the dos2unix entry point first to avoid problems";\
		echo "----------------------------------------------------";\
		echo "";

all1:
		set -e; \
		for i in $(SUBDIRS);\
		do (cd $$i; $(MAKE) -f unix.mk)\
		done

linux:		;$(MAKE) -f unix.mk CC=gcc all1
solaris:	;$(MAKE) -f unix.mk CC=gcc all1

dos2unix:
                # uudecode dos2unix.sh to make sure it has the CR intact
		uudecode dos2unix.uue; \
                chmod +x dos2unix.sh

                # dos2unix all the unix.mk files, otherwise it won't work
		find . -name unix.mk -exec $(DOS2UNIX) {} \;

		# dos2unix all the files (docs, read.me, etc)

		$(DOS2UNIX) history read.me readme.1st
		set -e; \
		for i in $(SUBDIRS2); \
		do $(DOS2UNIX) $$i/*; \
		done 

		set -e; \
		for i in $(SUBDIRS); \
		do (cd $$i;$(MAKE) -f unix.mk dos2unix); \
		done 

fix_2_cpp:
		set -e; \
		for i in taste_cp cplus2;\
		do (cd $$i; $(MAKE) -f unix.mk fix_2_cpp)\
		done

fix_2_cxx:
		set -e; \
		for i in taste_cp cplus2;\
		do (cd $$i; $(MAKE) -f unix.mk fix_2_cxx)\
		done

clean:
		set -e; \
		for i in $(SUBDIRS); \
		do (cd $$i; $(MAKE) -i -f unix.mk clean); \
		done 




