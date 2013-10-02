BINS = cut_rod matrix_mul longest_palindrome_sub_string

.PHONY : all
all : cut_rod matrix_mul longest_palindrome_sub_string
cut_rod : cut_rod.o
	gcc $? -g -o $@

matrix_mul : matrix_chain_multiplication.o
	gcc $? -g -o $@

longest_palindrome_sub_string : longest_palindrome_sub_string.o
	gcc $? -g -o $@
	
.PHONY : clean
clean :
	-rm $(BINS) *.o

