all: presentation.pdf

presentation.pdf: ctheader.tex presentation.tex ltl_presentation.tex
	pdflatex presentation
	pdflatex presentation

clean:
	rm -f presentation.pdf
	rm -f *.aux *.log *.nav *.out *.snm *.toc *.vrb
