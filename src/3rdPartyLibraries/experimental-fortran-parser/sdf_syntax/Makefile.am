# TEST_DIR = ../../tests/rule-tests
TEST_DIR = $(srcdir)

STRATEGO_TBL = /usr/local/packages/strategoxt-0.17/stratego-front/syn/StrategoStratego.tbl

TestFiles = $(wildcard $(TEST_DIR)/*.f90)
ASTFiles  = $(wildcard $(TEST_DIR)/*.f90.ast)

SDF_FILES =  FortranLex.sdf           \
             FortranLexFixed.sdf      \
             FortranLexFree.sdf       \
             FortranFixed.sdf         \
             Main.sdf

%.tbl : %
	sdf2table -i $< -o $@

# all : Fortran.tbl FortranFixed.tbl
all : Fortran.tbl

Main.def : Main.sdf FortranLex.sdf FortranLexFree.sdf
	pack-sdf -i $< -o $@

FortranFixed.def : FortranFixed.sdf FortranLex.sdf FortranLexFixed.sdf
	pack-sdf -i $< -o $@

Fortran.tbl : Main.def
	sdf2table -i $< -o $@ -m Main

FortranFixed.tbl : FortranFixed.def
	sdf2table -i $< -o $@ -m FortranFixed

Fortran.def : Fortran.sdf FortranLex.sdf FortranLexFree.sdf
	pack-sdf -i $< -o $@

Main.rtg : Main.def
	sdf2rtg -i Main.def -m Main -o Main.rtg --ignore-missing-cons

Main.sig : Main.rtg
	rtg2sig -i Main.rtg -o Main.sig

check : Fortran.tbl
	@for File in $(TestFiles) ;\
           do \
              sglr -v -p Fortran.tbl -i $$File -o $$File.ast ; \
           done

pretty : Fortran.tbl
	@for File in $(ASTFiles) ;\
           do \
              echo "Pretty printing $$File..." ; \
              pp-aterm -i $$File -o $$File.pp ; \
           done

Fortran.sig.aterm : Main.sig
	sglri -p $(STRATEGO_TBL) -i Main.sig | implode-asfix | pp-aterm -o Fortran.sig.aterm

junk : junk.f90 Fortran.tbl
	sglri -i junk.f90 -p Fortran.tbl | implode-asfix | pp-aterm -o junk.f90.aterm

junkf : junk.f FortranFixed.tbl
	sglri -i junk.f -p FortranFixed.tbl | implode-asfix | pp-aterm -o junk.f.aterm

junk.ptree : junk.f90 Fortran.tbl
	sglri -i junk.f90 -p Fortran.tbl -o junk.ptree

junk.ast-info.pp : junk.f90 Fortran.tbl
	sglr -i junk.f90 -p Fortran.tbl | addPosInfo -p ./junk.f90 -m | implode-asfix | pp-aterm -o junk.ast-info.pp

clean :
	rm -f *.def *.tbl *.rtg *.sig
	rm -f *.ast *.pp
	rm -f *.o *.mod
	rm -f $(TEST_DIR)/*.ast $(TEST_DIR)/*.ast.pp
