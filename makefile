test: clean
	gcc -Wall -g  eval.c test.c -o test
	./test

clean: 
	rm -f test
