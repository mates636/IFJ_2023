SCANNER_FILES := scanner.c scanner.h test_scanner.c

test_scanner: $(SCANNER_FILES)
	gcc -o test_scanner $(SCANNER_FILES)

test: test_scanner
	./test_scanner
