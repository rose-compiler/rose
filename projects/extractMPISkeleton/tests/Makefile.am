include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXEC=../../extractMPISkeleton

all: test

include Makefile.common

test:
	${MAKE} ${MARKS}
	${MAKE} ${ALL_OBJS}
	${MAKE} ${ALL_EXPECTED:.expected=.comp}
