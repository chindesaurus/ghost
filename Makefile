ghost: ghost.c
	gcc -ggdb -std=c99 -Wall -Werror -o ghost ghost.c -lm

clean:
	rm -f *.o a.out core ghost
	rm -r *.dSYM
