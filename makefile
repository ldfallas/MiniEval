test: clean
	gcc eval.c test.c -o test
	./test

clean: 
	rm test