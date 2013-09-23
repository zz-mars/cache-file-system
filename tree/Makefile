BINS = exp_eval buildTree

.PHONY : all
all : exp_eval buildTree

exp_eval : exp_eval_and_tree_traverse.o binary_tree.o threadTree.o stack.o
	gcc $? -o $@

buildTree : buildTreeTest.o binary_tree.o build_tree.o stack.o
	gcc $? -o $@

.PHONY : clean

clean :
	-rm *.o $(BINS)
