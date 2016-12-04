TEXSRC = $(wildcard ./*.tex)

paper: paper.pdf

all: paper.pdf

pdf: paper.pdf

paper.pdf: biblio.bib $(TEXSRC)
	pdflatex paper
	bibtex paper
	pdflatex paper
	pdflatex paper

clean: 
	\rm -f *.dvi *.aux *.ps *~ *.log *.blg *.bbl *.pdf
