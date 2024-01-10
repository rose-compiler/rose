XXFLAGS = -std=c++11 -Wall -Wextra
TARGET = analyzeGDBcallStack

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

.PHONY: clean check

clean:
	rm -f $(TARGET) *.preprocessed.txt *.analyzed.txt
AST_INPUT_FILE = ast.txt
GDB_INPUT_FILE = gdb.bt.txt 
check: $(TARGET) $(AST_INPUT_FILE) $(GDB_INPUT_FILE)
	./$(TARGET) $(AST_INPUT_FILE) $(GDB_INPUT_FILE)

