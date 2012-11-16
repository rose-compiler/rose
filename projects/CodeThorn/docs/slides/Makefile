all: presentation.pdf

presentation.pdf: ctheader.tex presentation.tex ltl_presentation.tex
	pdflatex -halt-on-error presentation
	pdflatex -halt-on-error presentation

clean:
	rm -f presentation.pdf
	rm -f *.aux *.log *.nav *.out *.snm *.toc *.vrb
	rm -f example1gfx1/*
	rm -f example1gfx2/*

CTDIR=../../src

examples:
	${CTDIR}/codethorn ${CTDIR}/tests/rers_mini8.c --tg2-estate-properties=no --viz=yes
	cp *.dot example1gfx1
	dot example1gfx1/transitiongraph2.dot -Tpng -oexample1gfx1/transitiongraph2.png
	dot example1gfx1/transitiongraph2.dot -Tps -oexample1gfx1/transitiongraph2.ps
	dot example1gfx1/cfg.dot -Tps -oexample1gfx1/cfg.ps
	${CTDIR}/codethorn ${CTDIR}/tests/rers_mini8.c --tg2-estate-properties=no --viz=yes --semantic-fold=yes
	cp *.dot example1gfx2
	dot example1gfx2/transitiongraph2.dot -Tpng -oexample1gfx2/transitiongraph2.png
	dot example1gfx2/transitiongraph2.dot -Tps -oexample1gfx2/transitiongraph2.ps

	${CTDIR}/codethorn ${CTDIR}/tests/rers_mini8.c --tg2-estate-properties=yes --viz=yes --semantic-fold=yes -Eminlen=3
	cp *.dot example1gfx3
	dot example1gfx3/transitiongraph2.dot -Tpng -oexample1gfx3/transitiongraph2.png
	dot example1gfx3/transitiongraph2.dot -Tps -oexample1gfx3/transitiongraph2.ps