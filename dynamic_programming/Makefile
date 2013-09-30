BINS = cut_rod matrix_mul

.PHONY : all
all : cut_rod matrix_mul
cut_rod : cut_rod.o
	gcc $? -g -o $@

matrix_mul : matrix_chain_multiplication.o
	gcc $? -g -o $@
	
.PHONY : clean
clean :
	-rm $(BINS) *.o

