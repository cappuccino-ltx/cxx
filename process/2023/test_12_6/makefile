

.PHONY:test

test:test.c binaryTree.c stack.c queue.c
	gcc test.c -o test binaryTree.c stack.c queue.c -std=c99 -lm

.PHONY:clean
clean:
	rm -f test