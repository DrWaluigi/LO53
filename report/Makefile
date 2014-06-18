LATEX := pdflatex

SOURCES := report.tex
TARGET := report.pdf

.PHONY: all clean distclean

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(LATEX) --shell-escape $(SOURCES)
	$(LATEX) --shell-escape $(SOURCES)

clean:
	rm -f *.log *.nav *.snm *.aux *.out *.toc *.pyg *.bcf *.run.xml
	rm -f *~

distclean: clean
	rm -f $(TARGET)

# EOF
