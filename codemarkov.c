#include <stdlib.h>
#include <stdio.h>

/* 16 megabytes if unsigned long is 8 bytes */
unsigned long counts[128][128][128][128];

void tally(char a1, char a2, char a3, char b)
{
	counts[a1&0x7f][a2&0x7f][a3&0x7f][b&0x7f]++;
	counts[0][0][0][b]++;
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

	if ((n = roulette(counts[a1][a2][a3])) == -1) {
		if ((n = roulette(counts[0][0][0])) == -1)
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

int main(int argc, char *argv[])
{
	FILE *f = stdin;
	int i, a1, a2, a3, b;
	int delay;

	if (argc < 2)
		return puts("forgetting something?"), 1;

	delay = atof(argv[1]) * 1000;

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
