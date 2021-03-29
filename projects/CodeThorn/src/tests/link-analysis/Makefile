all: LA001 LA002 LA009

LA001: LA001a.c LA001b.c
	gcc LA001a.c LA001b.c -oLA001
	./LA001

LA002: LA002a.c LA002b.c
	gcc LA002a.c LA002b.c -oLA002
	./LA002

LA009: LA009a.c LA009b.c
	gcc LA009a.c LA009b.c -oLA009
	./LA009

check: 
	codethorn --interpreter-mode=1 LA001a.c LA001b.c
	codethorn --interpreter-mode=1 LA002a.c LA002b.c
	codethorn --interpreter-mode=1 LA009a.c LA009b.c

clean:
	rm -f LA001 LA002 LA009

