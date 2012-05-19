all:
	gcc -Wall -Wextra -pedantic -std=c99 -g inventory.c itemlist.c main.c -l ncurses -o hax 
test:
	gcc -Wall -Wextra -pedantic -std=c99 -g inventory.c itemlist.c test.c -l ncurses -o test
clean:
	rm ./hacking ./test
