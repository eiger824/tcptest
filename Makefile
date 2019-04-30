all: cli srv

cli: tcpcli.c
	gcc -o $@ $< -Wall -Wextra -std=c11

srv: tcpsrv.c
	gcc -o $@ $< -Wall -Wextra -std=c11

clean:
	rm -f *~ cli srv
