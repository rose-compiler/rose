all: presentation.pdf

presentation.pdf: presentation.tex ctheader.tex
	pdflatex presentation

clean:
	rm -f presentation.pdf
	rm -f *.aux *.log *.nav *.out *.snm *.toc *.vrb

