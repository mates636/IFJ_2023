MAIN_FILES := main.c scanner.c parser.c symtable.c error_code.h scanner.h parser.h symtable.h
main: $(MAIN_FILES)
	gcc -g -o main $(MAIN_FILES)

SCANNER_FILES := scanner.c scanner.h test_scanner.c

all: test_scanner test_parser main

test_scanner: $(SCANNER_FILES)
	gcc -g -o test_scanner $(SCANNER_FILES)

all: test_scanner test_parser

PARSER_FILES := parser.c parser.h scanner.c scanner.h test_parser.c symtable.c
test_parser: $(PARSER_FILES)
	gcc -g -o test_parser $(PARSER_FILES)

