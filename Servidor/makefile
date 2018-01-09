EXEC=servidor
LOG=reqs.log
REGS=registers.dat

all: execute
	@echo "Sucesso!"

compile:
	@gcc $(EXEC).c -o $(EXEC) -lm -lpthread

files: compile
	@if ! [ -e $(LOG) ]; then \
	touch $(LOG); \
	fi;
	@if ! [ -e $(REGS) ]; then \
	touch $(REGS); \
	fi;

execute: files
	@./servidor
