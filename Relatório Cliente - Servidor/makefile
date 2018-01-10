#makefile

FILE = relatorio

all: $(FILE).pdf
	@xdg-open $(FILE).pdf &
	@clear

$(FILE).pdf: $(FILE).tex
	@lualatex $(FILE).tex
	@rm -f *.aux *.log
	@clear

