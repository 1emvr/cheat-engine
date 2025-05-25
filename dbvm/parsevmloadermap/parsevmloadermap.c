/* this program will parse the vmm file and generate a vmminfo.dat file that will
 * contain the data needed for the memorymanager inside the vmm 
 * It can also be used to grow the vmm.bin to include 0's */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

int main(void) {
	int errorcode = 0;
	FILE *fpVMLOADERMAP;
	FILE *fpVMLOADERBIN;
	char temp[256000];

	unsigned int startofbss;
	unsigned int sizeofbss;
	unsigned int memorylist = 0;

	// NOTE: breaking apart statements probably won't change the signature but makes it more readable.
	fpVMLOADERMAP = fopen("vmloader.map", "r"); 
	if (!fpVMLOADERMAP) {
		errorcode = 1;
		printf("ERR: failed opening vmm.map\n");
		goto defer;
	}

	//find the line start starts with ".bss  "
	temp[0] = 0;
	while (strcmp(temp, ".bss") != 0)
		fscanf(fpVMLOADERMAP, "%s", temp);

	fscanf(fpVMLOADERMAP, "%x", &startofbss);
	fscanf(fpVMLOADERMAP, "%x", &sizeofbss);
	fclose(fpVMLOADERMAP);

	printf("INF: startofbss=%x\n",startofbss);
	printf("INF: sizeofbss=%x\n",sizeofbss);

	fpVMLOADERBIN = fopen("vmloader.bin", "r+");
	if (!fpVMLOADERBIN) {
		printf("ERR: failed opening vmm.bin\n");
		errorcode = 1;
		goto defer;
	}

	unsigned char *buf = calloc(1, sizeofbss);
	unsigned int startoffreemem = startofbss + sizeofbss;
	int count;

	startoffreemem += 4096-(startoffreemem % 4096); // start of page-aligned free memory
	printf("INF: startoffreemem=%x\n",startoffreemem);

	//seek to the end
	printf("INF: seeking to %x\n", startofbss - 0x30000);
	if (fseek(fpVMLOADERBIN, startofbss - 0x30000, SEEK_SET) != -1) {
		if (count = fwrite(buf, sizeofbss, 1, fpVMLOADERBIN) < 1) {
			printf("ERR: fwrite failed. Count=%d\n",count);				
			errorcode=1;
		}
	} else {
		printf("ERR: seek failed\n");
		errorcode=1;
	}

defer:
	if (fpVMLOADERBIN) {
		fclose(fpVMLOADERBIN);
	}
	if (buf) {
		free(buf);
	}
	return errorcode;
}
