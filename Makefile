BIN=codemarkov
$(BIN): $(BIN).c
	gcc -O2 -g -o $@ $^ 
