all:
	gcc -Wall -g main.c inventory.c -l ncurses -o hax
test:
	gcc -Wall -g test.c inventory.c -l ncurses -o test
clean:
	rm ./hacking ./test
