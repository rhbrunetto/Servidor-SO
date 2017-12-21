EXEC=main

all: execute
	@echo "Sucesso!"

compile:
	@gcc $(EXEC).c -o $(EXEC) -lm

clear: compile
	@rm -f *.synctex.gz
	@rm -f *.aux
	@rm -f *.log
	@rm -f *.out
	@rm -f *.blg
	@rm -f *.bbl
	@rm -f *.ind
	@rm -f *.ilg

execute: clear
	@start "" $(EXEC).exe
