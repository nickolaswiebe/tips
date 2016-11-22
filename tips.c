#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int dorecord(char *file,int len,FILE *patch) {
	char addrstr[4];
	fread(addrstr,sizeof(char),3,patch);
	addrstr[3] = '\0';
	
	if(strcmp(addrstr,"EOF") == 0)
		return 0;
	
	unsigned int addr = 0;
	for(int i = 0; i < 3; i++) {
		addr <<= 8;
		addr |= 255 & (unsigned int)addrstr[i];
	}
	
	unsigned int size = 0;
	for(int i = 0; i < 2; i++) {
		size <<= 8;
		size |= 255 & (unsigned int)fgetc(patch);
	}
	
	if(size == 0) { // rle record
		unsigned int rlesize = 0;
		for(int i = 0; i < 2; i++) {
			rlesize <<= 8;
			rlesize |= 255 & (unsigned int)fgetc(patch);
		}
		
		unsigned int val = 255 & fgetc(patch);
		
		printf("rle: addr=%u size=%u byte=%u\n",addr,rlesize,(unsigned int)val);
		
		for(int i = 0; i < rlesize; i++)
			file[i + addr] = val;
		
		return 1;
	}
	
	printf("rec: addr=%u size=%u\n",addr,size);
	
	fread(&file[addr],sizeof(char),size,patch);
}

int main(int argc,char *argv[]) {
	if(argc != 4) {
		printf("usage: %s <input> <patch> <output>\n",argv[0]);
		return -1;
	}
	
	char *infile = argv[1];
	char *patchfile = argv[2];
	char *outfile = argv[3];
	
	FILE *in = fopen(infile,"r");
	FILE *patch = fopen(patchfile,"r");
	FILE *out = fopen(outfile,"w");
	
	char header[6];
	fread(header,sizeof(char),5,patch);
	header[5] = '\0';
	if(strcmp(header,"PATCH") != 0) {
		fclose(in);
		fclose(patch);
		fclose(out);
		printf("%s: error: invalic IPS header\n",patchfile);
		exit(-2);
	}
	
	fseek(in,0,SEEK_END);
	int len = ftell(in);
	fseek(in,0,SEEK_SET);
	
	char *file = malloc(sizeof(char) * len);
	fread(file,sizeof(char),len,in);
	
	while(dorecord(file,len,patch));
	
	fwrite(file,sizeof(char),len,out);
	
	fclose(in);
	fclose(patch);
	fclose(out);
}