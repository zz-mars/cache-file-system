BINS = cut_rod cut_rod_dp

.PHONY : all
all : cut_rod cut_rod_dp
cut_rod : cut_rod.o
	gcc $? -g -o $@

cut_rod_dp : dp_cut_rod.o
	gcc $? -g -o $@
	
.PHONY : clean
clean :
	-rm $(BINS) *.o

