all: LA001 LA002 LA003 LA009

LA001: LA001a.c LA001b.c
	gcc LA001a.c LA001b.c -oLA001
	./LA001

LA002: LA002a.c LA002b.c
	gcc LA002a.c LA002b.c -oLA002
	./LA002

LA003: LA003a.c LA003b.c LA003c.c
	gcc LA003a.c LA003b.c LA003c.c -oLA003
	./LA003

LA009: LA009a.c LA009b.c
	gcc LA009a.c LA009b.c -oLA009
	./LA009

check: 
	codethorn --interpreter-mode=1 LA001a.c LA001b.c
	codethorn --interpreter-mode=1 LA002a.c LA002b.c
	codethorn --interpreter-mode=1 LA003a.c LA003b.c LA003c.c
	codethorn --interpreter-mode=1 LA009a.c LA009b.c

check-external:
	thorn1 --external-functions-csv=external1.csv LA003a.c
	thorn1 --external-functions-csv=external2.csv LA003a.c LA003b.c
	thorn1 --external-functions-csv=external3.csv LA003a.c LA003b.c LA003c.c
	cat external1.csv
	cat external2.csv
	cat external3.csv

clean:
	rm -f LA001 LA002 LA003 LA009 *.csv

