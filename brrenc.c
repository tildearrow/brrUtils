#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include "brrUtils.h"

#define CHUNK_SIZE 8192

short** chunks=NULL;
int chunkCapacity=0;
int chunkCount=0;

unsigned char pushChunk(short* buf) {
  if ((chunkCount+1)>chunkCapacity) {
    short** oldChunks=chunks;
    int newCapacity=(chunkCapacity==0)?256:(chunkCapacity+256);
    chunks=malloc(newCapacity*sizeof(short*));
    if (chunks==NULL) return 0;
    if (chunkCapacity>0) {
      memcpy(chunks,oldChunks,chunkCapacity*sizeof(short*));
    }
    free(oldChunks);
    chunkCapacity=newCapacity;
  }
  chunks[chunkCount]=malloc(CHUNK_SIZE*sizeof(short));
  if (chunks[chunkCount]==NULL) return 0;
  memcpy(chunks[chunkCount],buf,CHUNK_SIZE*sizeof(short));
  chunkCount++;
  return 1;
}

int main(int argc, char** argv) {
  if (argc<3) {
    printf("usage: %s in out\n",argv[0]);
    return 1;
  }

  SF_INFO si;
  memset(&si,0,sizeof(SF_INFO));
  SNDFILE* sf=sf_open(argv[1],SFM_READ,&si);

  if (sf==NULL) {
    fprintf(stderr,"input: open error: %s\n",sf_strerror(NULL));
    return 1;
  }

  float* nextChunkF=malloc(CHUNK_SIZE*sizeof(float)*si.channels);
  short* nextChunk=malloc(CHUNK_SIZE*sizeof(short));

  if (nextChunkF==NULL) {
    perror("input: could not create first read chunk");
    sf_close(sf);
    return 1;
  }
  if (nextChunk==NULL) {
    perror("input: could not create second read chunk");
    sf_close(sf);
    return 1;
  }

  long total=0;

  while (1) {
    long count=sf_readf_float(sf,nextChunkF,CHUNK_SIZE);
    if (count==0) break;
    total+=count;
    for (int i=0; i<CHUNK_SIZE; i++) {
      int accum=0;
      for (int j=0; j<si.channels; j++) {
        accum+=nextChunkF[i*si.channels+j]*32767.0f;
      }
      accum/=si.channels;
      if (accum<-32768) accum=-32768;
      if (accum>32767) accum=32767;
      nextChunk[i]=accum;
    }
    if (!pushChunk(nextChunk)) {
      perror("input: could not push chunk");
      sf_close(sf);
      return 1;
    }
  }

  sf_close(sf);

  free(nextChunkF);
  free(nextChunk);

  short* inBuf=malloc(chunkCount*CHUNK_SIZE*sizeof(short));
  if (inBuf==NULL) {
    perror("input: could not create buffer");
    return 1;
  }
  unsigned char* outBuf=malloc((total/16)*9);
  if (outBuf==NULL) {
    perror("output: could not create buffer");
    return 1;
  }

  short* pos=inBuf;
  for (int i=0; i<chunkCount; i++) {
    memcpy(pos,chunks[i],CHUNK_SIZE*sizeof(short));
    pos+=CHUNK_SIZE;
  }

  printf("input: %ld samples\n",total);

  long encoded=brrEncode(inBuf,outBuf,total,0);

  printf("output: %ld bytes (%ld blocks)\n",encoded,encoded/9);

  FILE* out=fopen(argv[2],"wb");
  if (out==NULL) {
    perror("output: open error");
    return 1;
  }

  fwrite(outBuf,1,encoded,out);

  fclose(out);

  return 0;
}
