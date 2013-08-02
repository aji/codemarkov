default: codemarkov binmarkov
codemarkov: codemarkov.c
	gcc -O2 -g -o $@ $^ 
binmarkov: binmarkov.c
	gcc -O2 -g -o $@ $^ 
clean:
	rm -f codemarkov binmarkov
