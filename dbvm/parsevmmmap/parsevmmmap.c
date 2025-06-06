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
	FILE *fpVMMMAP;
	FILE *fpVMMBIN;
	char temp[256000];

	unsigned int startofbss;
	unsigned int sizeofbss;
	unsigned int FirstFreeAddress = 0;

	// NOTE: just making more readable. breaking statements up probably won't change what the compiler produces (probably slower too).
	fpVMMMAP=fopen("vmm.map","r"); 
  	if (!fpVMMMAP) {
		printf("ERR: failed opening vmm.map\n");
		errorcode=1;
		goto defer;
	}

	//find the line start starts with ".bss  "
	temp[0]=0;
	while (strcmp(temp, ".bss") != 0)
		fscanf(fpVMMMAP, "%s", temp);

	fscanf(fpVMMMAP, "%x", &startofbss);
	fscanf(fpVMMMAP, "%x", &sizeofbss);

	/*
	   temp[0]=0;
	   while (strcmp(temp,"FirstFreeAddress")!=0)
	   {
	   FirstFreeAddress=strtol(temp,NULL,16);
	   if (fscanf(fpVMMMAP,"%s",(char *)&temp)==EOF)
	   {
	   printf("Error: EOF\n");
	   return 1;
	   }
	   }
	*/
	fclose(fpVMMMAP);

	if ((startofbss+sizeofbss) % 0x1000) {
		//add a little bit more for a nice page boundary
		int difference = (startofbss + sizeofbss + 0x1000) % 0x1000;
		sizeofbss += 0x1000 - difference;
	}

	printf("INF: startofbss=%x\n",startofbss);
	printf("INF: sizeofbss=%x\n",sizeofbss);
	//printf("FirstFreeAddress=%x\n",FirstFreeAddress);

	fpVMMBIN=fopen("vmm.bin","r+");
	if (!fpVMMBIN) {
		printf("ERR: failed opening vmm.bin\n");
		errorcode=1;
		goto defer;
	}

	unsigned char *buf = calloc(1, sizeofbss);
	unsigned long long startoffreemem = startofbss + sizeofbss;
	int count;

	startoffreemem += 4096 - (startoffreemem % 4096);
	//printf("startoffreemem=%x\n",(int)startoffreemem);
	//*(unsigned long long *)(&buf[FirstFreeAddress-startofbss])=(startoffreemem+0x9000) & 0xfffffffffffff000ULL;

	//seek to the end
	printf("INF: seeking to %x\n",startofbss-0x00400000);
	if (fseek(fpVMMBIN, startofbss - 0x00400000, SEEK_SET) != -1) {
		if (count=fwrite(buf, sizeofbss, 1, fpVMMBIN) < 1) {
			printf("ERR: fwrite failed. Count=%d\n",count);				
			errorcode=1;
			goto defer;
		}
	} else {
		printf("ERR: seek failed\n");
		errorcode = 1;
	}

defer:
	if (fpVMMBIN) {
		fclose(fpVMMBIN);
	}
	if (buf) {
		free(buf);
	}
	return errorcode;
}
