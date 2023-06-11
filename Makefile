ftw1: ftw1.c
	gcc ftw1.c -o ftw1

ftw2: ftw2.c
	gcc ftw2.c -o ftw2

stree: stree.c
	gcc stree.c -o stree

.PHONY: clean
clean:
	rm ftw1 ftw2 stree 2>/dev/null