SCANNER_FILES := scanner.c scanner.h test_scanner.c
PARSER_FILES := parser.c parser.h scanner.c scanner.h test_parser.c symtable.c

all: test_scanner test_parser

test_scanner: $(SCANNER_FILES)
	gcc -g -o test_scanner $(SCANNER_FILES) -o test_scanner

test_parser: $(PARSER_FILES)
	gcc -g -o test_parser $(PARSER_FILES) -o test_parser

parser: parser.o
	cc -c parser.c -o parser.c

clean:
	rm -f test_scanner test_parser
