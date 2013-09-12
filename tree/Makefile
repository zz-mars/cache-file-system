BINS = exp_eval

.PHONY : all
all : exp_eval

exp_eval : exp_eval_and_tree_traverse.o binary_tree.o threadTree.o stack.o
	gcc $? -o $@

.PHONY : clean

clean :
	-rm *.o $(BINS)
