#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <sndfile.h>
#include "brrUtils.h"

int main(int argc, char** argv) {
  if (argc<3) {
    printf("usage: %s in out\n",argv[0]);
    return 1;
  }

  FILE* f=fopen(argv[1],"rb");
  if (f==NULL) {
    perror("input: open error");
    return 1;
  }

  if (fseek(f,0,SEEK_END)!=0) {
    perror("input: size error");
    fclose(f);
    return 1;
  }

  long len=ftell(f);
  if (len<0) {
    perror("input: size get error");
    fclose(f);
    return 1;
  }
  if (len==0) {
    fprintf(stderr,"input: that file is empty\n");
    fclose(f);
    return 1;
  }

  if (fseek(f,0,SEEK_SET)!=0) {
    perror("input: seek error");
    fclose(f);
    return 1;
  }

  unsigned char* inBuf=malloc(len);
  if (inBuf==NULL) {
    perror("input: could not create input buffer");
    fclose(f);
    return 1;
  }

  short* outBuf=malloc(16*(len/9)*sizeof(short));
  if (outBuf==NULL) {
    perror("output: could not create input buffer");
    fclose(f);
    return 1;
  }

  long actualLen=0;
  if ((actualLen=fread(inBuf,1,len,f))==0) {
    perror("input: read error");
    fclose(f);
    return 1;
  }
  fclose(f);

  if (actualLen%9) {
    fprintf(stderr,"warning: input size not multiple of 9!\n");
  }

  printf("input: %ld bytes (%ld blocks)\n",actualLen,actualLen/9);

  long decoded=brrDecode(inBuf,outBuf,len);

  printf("output: %ld samples\n",decoded);

  SF_INFO si;
  si.samplerate=32000;
  si.channels=1;
  si.format=SF_FORMAT_WAV|SF_FORMAT_PCM_16;

  SNDFILE* sf=sf_open(argv[2],SFM_WRITE,&si);

  if (sf==NULL) {
    fprintf(stderr,"output: open error: %s\n",sf_strerror(NULL));
    free(inBuf);
    free(outBuf);
    return 1;
  }

  sf_writef_short(sf,outBuf,decoded);

  sf_close(sf);
  return 0;
}
