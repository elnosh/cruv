#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define PB "||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 50

void printPercentUsed(double);
char *getSubstring(char *, int, int);

int main(int argc, char *argv[]) {
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;

	fp = fopen("/proc/meminfo", "r");
	if (fp == NULL) {
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}
	
	int mem_free, total, used;
	while ((nread = getline(&line, &len, fp)) != EOF) {
		char *resource;

		if (strstr(line, "MemAvailable") != NULL || strstr(line, "MemTotal") != NULL) {
			printf("%s", line);

			int start = 1000, end = 1000, digidx = 1000;
			for (int i = 0; i < strlen(line); i++) {
				if (isalpha(line[i]) && start > i) {
					start = i;
				} else if (line[i] == ':') {
					end = i;
					resource = getSubstring(line, start, end);
					start = 1000;
				} else if (isdigit(line[i]) && start > i) {
					start = i;
					digidx = i;
				} else if (isblank(line[i]) && digidx < line[i]) {
					end = i;
					if (strstr(resource, "MemAvailable")) {
						resource = getSubstring(line, start, end);
						mem_free = atoi(resource);
					} else if (strstr(resource, "MemTotal")) {
						resource = getSubstring(line, start, end);
						total = atoi(resource);
					}
				}
			}
		}
	}

	used = total - mem_free;
	double pct_used = ((double) used / (double) total);

	printf("\nMemory used: %0.2f%%\n", pct_used * 100);

	printPercentUsed(pct_used);

	free(line);
	fclose(fp);
	
	exit(EXIT_SUCCESS);
}

// pct bar
void printPercentUsed(double percentage) {
	int lpad = (int) (percentage * PBWIDTH); // pbwidth 50
	int rpad = PBWIDTH - lpad;
	printf("\r%0.2f%% [%.*s%*s]\n", percentage * 100, lpad, PB, rpad, ""); // %.*s - prints lpad amount of PB (||) // *s - printfs rpad "" spaces
	fflush(stdout);
}

// function to extract substring from str with start and end idx - return NULL if err
char *getSubstring(char *str, int from, int to) {
	int len = to - from;

	char *retStr = (char *) malloc(len *sizeof(char));
	for (int i = 0; i < len; i++) {
		retStr[i] = str[from];
		from++;
	}

	return retStr;
}
