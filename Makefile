luter: main.c trie.c
	cc -o luter -g main.c trie.c

clean:
	rm -f luter *.o 
