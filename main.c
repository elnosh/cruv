#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define PB "||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 50

void printPercentUsed(double);
char *getSubstring(char *, int, int);
void getMemoryUsg();
void getCPU();

int main(int argc, char *argv[]) {
	getMemoryUsg();
	getCPU();
	
	exit(EXIT_SUCCESS);
}

void getMemoryUsg() {
	// mem
	FILE *mem_fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;

	mem_fp = fopen("/proc/meminfo", "r");
	if (mem_fp == NULL) {
		perror("Error opening /proc/meminfo");
		exit(EXIT_FAILURE);
	}
	
	int mem_free, total, used;
	while ((nread = getline(&line, &len, mem_fp)) != EOF) {
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
	fclose(mem_fp);
}

// TODO: fix - very inaccurate
void getCPU() {
	// cpu
	FILE *cpu_fp;
	char *fline = NULL;
	size_t len = 0;
	ssize_t nread;

	cpu_fp = fopen("/proc/stat", "r");
	if (cpu_fp == NULL) {
		perror("error opening /proc/stat");
		exit(EXIT_FAILURE);
	}

	//int free_cpu, tot_cpu, used_cpu;
	while ((nread = getline(&fline, &len, cpu_fp)) != EOF) {
		int user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
		double idle_time;

		if (strstr(fline, "cpu") != NULL) {
			char cpu[5];
			cpu[4] = '\0';
			strncpy(cpu, &fline[0], 4);
			
			if (strcmp(cpu, "cpu ") == 0) {
				sscanf(fline, "cpu  %d %d %d %d %d %d %d %d %d %d", &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice); 
				//printf("printing values: %d %d %d %d %d %d %d %d %d %d", user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice); 
				double num = (double) idle * 100, den = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
				idle_time = num / den;
				double cpu_usage = 100 - idle_time;
				printf("\nCPU usage: %0.2f\n", cpu_usage);
				printPercentUsed(cpu_usage / 100);
			}
		}
	}
	free(fline);
	fclose(cpu_fp);
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
