#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
using namespace std;

#include <stddef.h>
#include <stdint.h>

#define SWAP32(q) ((((q) >> 24) & 0xff) + (((q) >> 8) & 0xff00) + \
                    (((q) & 0xff00) << 8) + (((q) & 0xff) << 24))
/* CRC-32C (iSCSI) polynomial in reversed bit order. */
#define POLY 0xedb88320
#ifdef BLOCKS
#define CHECKSUM_BLOCK_SIZE 0xC000
#endif

uint32_t crc32c(uint32_t crc, const unsigned char *buf, size_t len)
  {
  int k;

  crc = ~crc;
  while (len--)
    {
    crc ^= *buf++;
    for (k = 0; k < 8; k++)
      crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
    }
  return ~crc;
  }

int main(int argc, char *argv[])
  {
  if (argc<2)
    {
    printf("Zend(c) 2021 Tricore Aurix SAK-TC2xxxx CPU checksum calc\nUsage: crcTricore <file.bin>\n");
    return 0;
    }
  FILE *f = fopen(argv[1],"rb");
  if (NULL==f)
    {
    printf("error file open (%s)\n", argv[1]);
    return 1;
    }
  fseek(f,0,SEEK_END);
  uint32_t fsz = ftell(f);
  fseek(f,0,SEEK_SET);
#ifndef BLOCKS
  uint32_t CHECKSUM_BLOCK_SIZE = fsz;
  uint32_t j=0;
#endif
  uint8_t *pbuf = new uint8_t[CHECKSUM_BLOCK_SIZE];
  if (fsz % 4)
    {
    printf("file size must be multiple of 4!\n");
    return 1;
    }
  printf("reading file %s... 0x%x bytes\n", argv[1], fsz);
  //
#ifdef BLOCKS
  for (uint32_t j=0; j<fsz; j+=CHECKSUM_BLOCK_SIZE)
    {
#endif
    for (uint32_t i=0; i<(CHECKSUM_BLOCK_SIZE-4); i+=4)
      {
      uint32_t l;
      if (!fread(&l, 4, 1, f))
        {
        printf("read file error\n");
        return 1;
        }
      *(uint32_t*) (&pbuf[i]) = SWAP32(l);
      }
    uint32_t crc = crc32c(0, (const unsigned char*)pbuf, CHECKSUM_BLOCK_SIZE-4);
    uint32_t real_crc;
    if (!fread(&real_crc, 4, 1, f))
      {
      printf("read file error\n");
      return 1;
      }
    printf("CRC [%08X] Must be %08X IS %08X (%s)\n",(j+CHECKSUM_BLOCK_SIZE),SWAP32(crc), SWAP32(real_crc), (crc==real_crc)?"GOOD!": "===BAD===");
#ifdef BLOCKS
    }//..for j
#endif
  fclose(f);
  delete[] pbuf;
  return 0;
  }
