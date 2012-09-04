all:
	gcc -Wall -Wextra -pedantic -std=c99 -g itemlist.c inventory.c main.c -l ncurses -o hax 
test:
	gcc -Wall -Wextra -pedantic -std=c99 -g itemlist.c inventory.c test.c -l ncurses -o test
clean:
	rm ./hacking ./test
