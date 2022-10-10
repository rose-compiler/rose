all: termpretty

termpretty: termpretty.l++
	flex termpretty.l++
	g++ -std=c++98 lex.yy.c -otermpretty

clean:
	rm -f lex.yy.c termpretty
