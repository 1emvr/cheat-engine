/* imagemaker.c: combines the binary files into one diskimage */


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

int img_read(unsigned char *buffer, const char *filename, int size) { // moved to separate function to change signature (probably slower).
	FILE *fp = fopen(filename, "r");
	buffer = malloc(size);
	fread((void*)buffer, size, 1, fp);
	fclose(fp);

	return size;
}

int main(void) {
 // FILE *fpBootloader;
 // FILE *fpVmloader;
 // FILE *fpVmm;
 FILE *fpDisk;
 FILE *fpFullDisk;
  
  unsigned char *bootloader;
  unsigned char *vmloader;
  unsigned char *vmm;
  unsigned char *vmdisk;
  
  unsigned short int VMMlocation;
  unsigned char sector[512];
  
  int bootloader_size;
  int vmloader_size;
  int vmm_size;
  int vmdisk_size;
  int *sectornumber=(int *)sector;
  
  long fpos;
  struct stat tempstat;
  
  memset(sector,0,512);
  printf("INF: image being created\n");
  
  printf("INF: opening bin files\n");
  printf("INF: opening and reading bootloader.bin...");  

  if (stat("bootloader.bin", &tempstat) == -1) {
    printf("ERR: file can't be found\n");
    return 1;
  }

  //bootloader_size=tempstat.st_size;
  //bootloader=malloc(bootloader_size);  
  //fpBootloader=fopen("bootloader.bin","r");    
  //fread(bootloader,tempstat.st_size,1,fpBootloader);
  bootloader_size = img_read(bootloader, "bootloader.bin", tempstat.st_size);
  printf("INF: done\n");
  
    
  printf("INF: opening and reading vmloader.bin...");  
  if (stat("vmloader.bin", &tempstat)) {
    printf("ERR: file can't be found\n");
    return 1;
  }  
  //vmloader_size=tempstat.st_size;
  //vmloader=malloc(vmloader_size);  
  //fpVmloader=fopen("vmloader.bin","r");    
  //fread(vmloader,vmloader_size,1,fpVmloader);
  vmloader_size = img_read(vmloader, "vmloader.bin", tempstat.st_size);
  printf("INF: done\n");  
  
  
  printf("INF: opening and reading vmm.bin...");  
  if (stat("vmm.bin",&tempstat)) {
    printf("ERR: file can't be found\n");
    return 1;
  }  
  //vmm_size=tempstat.st_size;
  //vmm=malloc(vmm_size);  
  //fpVmm=fopen("vmm.bin","r");    
  //fread(vmm,tempstat.st_size,1,fpVmm);
  vmm_size = img_read(vmm, "vmm.bin", tempstat.st_size);
  printf("INF: done\n");
  
  printf("INF: creating vmdisk.img...\n");
  fpDisk = fopen("vmdisk.img","w");
  if (fpDisk==NULL) {
    printf("ERR: failed creating file\n");
    return 1;    
  }
  
  printf("INF: writing VMMlocation in bootsector\n");
  VMMlocation = 2+(1+vmloader_size/512);  
  *(unsigned short int *)&bootloader[0x8] = VMMlocation; // write address to header (?)
  
  fwrite(bootloader, bootloader_size, 1, fpDisk);
  fwrite(vmloader, vmloader_size, 1, fpDisk);
  
  //now seek to the VMM startsector
  fseek(fpDisk, VMMlocation*512, SEEK_SET); //go to next sector pos  
  fwrite(vmm,vmm_size,1,fpDisk);
  
  //fill till dividable by 512
  bzero(sector, 512);
  fwrite(sector, 512 - ((VMMlocation*512+vmm_size) % 512), 1, fpDisk);
  fclose(fpDisk);

  //fclose(fpVmm);
  //fclose(fpVmloader);
  //fclose(fpBootloader);    

  //full disk image (cdrom capable)
  printf("INF: reopening and reading vmdisk.img...");  
  if (stat("vmdisk.img",&tempstat)) {
    printf("ERR: file can't be found\n");
    return 1;
  }  
  vmdisk_size = tempstat.st_size;
  vmdisk = malloc(vmdisk_size);  
    
  fpDisk = fopen("vmdisk.img","r");
  fpFullDisk = fopen("vmdisk144.img","w");
  
  fread(vmdisk, vmdisk_size, 1, fpDisk);
  fwrite(vmdisk, vmdisk_size, 1, fpFullDisk);  
  
  fseek(fpFullDisk, 1474559, SEEK_SET);
  fwrite(sector, 1, 1, fpFullDisk);
  
  printf("INF: done\n");
  printf("INF: completed\n");

  return 0;  
}
