MAIN_FILES := main.c scanner.c parser.c symtable.c error_code.h scanner.h parser.h symtable.h
main: $(MAIN_FILES)
	gcc -g -o main $(MAIN_FILES)
