#ifndef __TATER_BMP_H
#define __TATER_BMP_H

#include "ff.h"
#include "stm32l4xx.h"

#define  MAX_BMP_FILES  25
#define  MAX_BMP_FILE_NAME 11

#pragma pack(1) /* 不知道为什么不加会有问题. */
typedef struct BmpHeader
{
  uint8_t  B;
  uint8_t  M;
  uint32_t fsize;
  uint16_t res1;
  uint16_t res2;
  uint32_t offset;
  uint32_t hsize;
  uint32_t w;
  uint32_t h;
  uint16_t planes;
  uint16_t bpp;
  uint32_t ctype;
  uint32_t dsize;
  uint32_t hppm;
  uint32_t vppm;
  uint32_t colorsused;
  uint32_t colorreq;
}BmpHeader;

uint32_t Storage_GetDirectoryBitmapFiles(const char* DirName, char* Files[]);
uint32_t Storage_OpenReadFile(uint8_t Xpoz, uint16_t Ypoz, const char *BmpName);

#endif 
