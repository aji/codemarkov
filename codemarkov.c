/*
   Copyright (c) 2013 Alex Iadicicco

   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
   OTHER DEALINGS IN THE SOFTWARE.

   Just don't be a mong, okay?
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

/* 16 megabytes if unsigned long is 8 bytes */
unsigned long *counts[128];

void tally(char a1, char a2, char a3, char b)
{
	a1 &= 0x7f; a2 &= 0x7f; a3 &= 0x7f; b &= 0x7f;
	counts[a1][a2*128*128 + a3*128 + b]++;
	counts[0][b]++;
}

char roulette(unsigned long *cnt)
{
	unsigned long i, total;
	unsigned long x, seen, want;

	for (total=0, i=0; i<128; i++)
		total += cnt[i];
	if (total == 0)
		return -1;
	want = rand() % total;
	for (seen=0, i=0; i<128; i++) {
		x = cnt[i];
		if (seen + x > want)
			return i;
		seen += x;
	}
	abort();
}

char next(char a1, char a2, char a3)
{
	char n;

	if ((n = roulette(&counts[a1][a2*128*128+a3*128])) == -1) {
		if ((n = roulette(counts[0])) == -1)
			abort();
		return n;
	}

	return n;
}

int inch(FILE *f)
{
	int c = fgetc(f);
	if (c == EOF)
		return c;
	return ((unsigned)c) & 0x7f;
}

int do_alloc(void)
{
	size_t i, sz = 128 * 128 * 128;

	for (i=0; i<128; i++) {
		if (!(counts[i] = calloc(sz, sizeof(unsigned long))))
			return 0;
	}

	return 1;
}

void do_seed(void)
{
	FILE *f = fopen("/dev/urandom", "r");
	int x;

	if (f == NULL) {
		x = time(NULL);
	} else {
		fread(&x, sizeof(x), 1, f);
		fclose(f);
	}

	srand(x);
}

int main(int argc, char *argv[])
{
	FILE *f = stdin;
	int i, a1, a2, a3, b;
	int delay;

	if (argc < 2)
		return fprintf(stderr, "forgetting something?\n"), 1;

	delay = atof(argv[1]) * 1000;

	do_seed();

	fprintf(stderr, "allocating...\n");
	if (!do_alloc())
		return fprintf(stderr, "alloc crapped itself :(\n"), 1;

	for (a1=a2=a3=0; (b=inch(f))!=EOF; a1=a2,a2=a3,a3=b)
		tally(a1, a2, a3, b);

	a1 = a2 = a3 = 0;
	for (a1=a2=a3; ; a1=a2,a2=a3,a3=b) {
		b = next(a1, a2, a3);
		fputc(b, stdout);
		fflush(stdout);
		usleep(delay);
	}

	return 0;
}
