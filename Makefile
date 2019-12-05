all:
	g++ ./src/bin/main.cpp ./src/bin/equivalent_operations.cpp ./src/bin/check_taks.cpp ./src/libs/Operation.cpp ./src/libs/Operation.h -O2 -o alg_KnyazevAA_204
	g++ ./src/bin/test_generator.cpp -o gen_KnyazevAA_204 -O2
test:
	g++ ./src/bin/test_applying.cpp -o test.out -O2 -Wall
clean:
	rm *Knyazev*
	rm *.out

