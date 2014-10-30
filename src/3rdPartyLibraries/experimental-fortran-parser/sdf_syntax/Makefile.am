# include ../../make.inc
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SDF2_PATH =
ST_PATH =

%.tbl : %
	$(SDF2_PATH)sdf2table -i $< -o $@

all : Fortran.tbl

Main.def : Main.sdf FortranLex.sdf FortranLexFree.sdf
	$(ST_PATH)pack-sdf -i $< -o $@

FortranFixed.def : FortranFixed.sdf FortranLex.sdf FortranLexFixed.sdf
	$(ST_PATH)pack-sdf -i $< -o $@

Fortran.tbl : Main.def
	$(SDF2_PATH)sdf2table -i $< -o $@ -m Main

FortranFixed.tbl : FortranFixed.def
	$(SDF2_PATH)sdf2table -i $< -o $@ -m FortranFixed

Fortran.def : Fortran.sdf FortranLex.sdf FortranLexFree.sdf
	$(ST_PATH)pack-sdf -i $< -o $@

clean :
	rm -f *.def *.tbl

