all: checkarrayoperators

APPNAME=checkarrayoperators

$(APPNAME):
	g++ $(APPNAME).cpp -o$(APPNAME)

clean:
	rm -f $(APPNAME)

check: $(APPNAME)
	./$(APPNAME) 0
	./$(APPNAME) 1
	./$(APPNAME) 2
	./$(APPNAME) 3
	@echo "ALL TESTS PASSED."


