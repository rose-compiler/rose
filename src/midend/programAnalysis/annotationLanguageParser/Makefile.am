include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

noinst_HEADERS = \
     actionchanger.h analyze.h annvariable.h callingcontext.h diagnostic.h \
     localstl.h property.h set_property.h action.h ann.h bdwy_liveness.h \
     constantprop.h enum_property.h pointerrule.h report.h structure.h \
     adaptor_statistics.h annotations.h broadway.h cpattern.h broadway_expr.h \
     procedure.h rule.h unification_ann.h

include_HEADERS = 

noinst_LTLIBRARIES = libannotationLanguageParser.la

## Specifications for building the pdf library.
libannotationLanguageParser_la_SOURCES = \
   language-lexer.cc language-parser.cc \
   annotations.cc \
   annvariable.cc \
   structure.cc \
   enum_property.cc \
   procedure.cc \
   set_property.cc \
   pointerrule.cc \
   analyze.cc \
   rule.cc \
   broadway_expr.cc \
   report.cc \
   language.tab.h

libannotationLanguageParser_la_DEPENDENCES = language-lexer.cc language-parser.cc language.tab.h
# libannotationLanguageParser_la_LIBADD = 

BUILT_SOURCES = language-lexer.cc language-parser.cc language.tab.h

AM_CXXFLAGS = -D__LOCALSTL_H

language-parser.cc: $(srcdir)/language.y
	bison -d -pann -o ./tempBisonOutput.c $(srcdir)/language.y
	mv tempBisonOutput.c language-parser.cc
	mv tempBisonOutput.h language.tab.h

language-lexer.cc: $(srcdir)/language.l language-parser.cc
	flex $(srcdir)/language.l
	mv lex.ann.c language-lexer.cc

language.tab.h: language-parser.cc

clean:
	rm -f language-lexer.cc lex.ann.c language-parser.cc language.tab.h tempBisonOutput.*
	rm -f *.lo *.la

EXTRA_DIST = CMakeLists.txt language.l language.y PolicyControl.txt
