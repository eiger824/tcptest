all: cli srv

cli: l4cli.c
	gcc -o $@ $< -Wall -Wextra -std=c11

srv: l4srv.c
	gcc -o $@ $< -Wall -Wextra -std=c11

clean:
	rm -f *~ cli srv
