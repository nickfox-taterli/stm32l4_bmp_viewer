#include "BMP.h"
#include "LCD.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include <stdio.h>

#define BITMAP_BUFFER_SIZE 720 /* 720 = 3Bit * 240*/
#define BITMAP_HEADER_SIZE sizeof(BmpHeader) /* Bitmap specificity */
#define MIN(a,b) (((a)<(b))?(a):(b))

DIR BMPDirectory;
FILINFO BMPFileInfo;
uint8_t aBuffer[BITMAP_HEADER_SIZE + BITMAP_BUFFER_SIZE];
uint32_t BytesWritten, BytesRead;

/**
  * @brief  Open a file and display it on lcd
  * @param  DirName: the Directory name to open
  * @param  FileName: the file name to open
  * @param  BufferAddress: A pointer to a buffer to copy the file to
  * @param  FileLen: the File length
  * @retval err: Error status (0=> success, 1=> fail)
  */
uint32_t Storage_OpenReadFile(uint8_t Xpoz, uint16_t Ypoz, const char *BmpName)
{
  uint32_t size = 0;
  FIL bmpfile;
  BmpHeader* pbmpheader = (BmpHeader*)aBuffer;
  
  /* Close a bmp file */
  f_open(&bmpfile, BmpName, FA_READ);
  
  /* Read the constant part of the header from the file and store it at the top of aBuffer*/
  f_read(&bmpfile, &aBuffer, BITMAP_HEADER_SIZE, &BytesRead);
  
  /* Get the size of the data stored inside the file */
  size = pbmpheader->fsize - pbmpheader->offset;
  
  /* Start reading at the top of the file */
  f_lseek(&bmpfile, 0);
  		
			/* Read the entire header from the file and store it at the top of aBuffer */
			f_read(&bmpfile, &aBuffer, pbmpheader->offset, &BytesRead);
			
			/* Compute the number of entire lines which can be stored inside the buffer */
			if(!((BITMAP_BUFFER_SIZE - pbmpheader->offset + BITMAP_HEADER_SIZE)/(pbmpheader->w * (pbmpheader->bpp/8)))){
				return 1;
			}
			/* As long as the entire bitmap file as not been displayed */
			do
			{
				uint32_t nbbytetoread;
				
				/* Get the number of bytes which can be stored inside the buffer */
				nbbytetoread = MIN(size,pbmpheader->w*(pbmpheader->bpp/8));
			
				/* Adapt the total size of the bitmap, stored inside the header, to this chunck */
				pbmpheader->fsize = pbmpheader->offset + nbbytetoread;
			
				/* Adapt the number of line, stored inside the header, to this chunck */
				pbmpheader->h = nbbytetoread/(pbmpheader->w*(pbmpheader->bpp/8));
				
				/* Start reading at the end of the file */
				f_lseek(&bmpfile, pbmpheader->offset + size - nbbytetoread);
				
				/* Store this chunck (or the entire part if possible) of the file inside a buffer */
				f_read(&bmpfile, aBuffer + pbmpheader->offset, nbbytetoread, &BytesRead);
				/* Draw the bitmap */
				ILI9341_LCD_DrawBitmap(Xpoz, Ypoz, aBuffer);    
					
				/* Update the remaining number of bytes to read */
				size -= nbbytetoread;
				
				/* Change the display position of the next bitmap */
				Ypoz++;
					
			}while (size > 0);

  
  /* Close the bmp file */
  f_close(&bmpfile);
  
  return 0;
}      

/**
  * @brief  List up to 25 file on the root directory with extension .BMP
  * @param  DirName: Directory name
  * @param  Files: Buffer to contain read files
  * @retval The number of the found files
  */
uint32_t Storage_GetDirectoryBitmapFiles(const char* DirName, char* Files[])
{
  uint32_t counter = 0, index = 0;
	FRESULT res;

  res = f_opendir(&BMPDirectory, DirName);
  
  if(res == FR_OK)
  {
    for (;;)
    {
      res = f_readdir(&BMPDirectory, &BMPFileInfo);
      if(res != FR_OK || BMPFileInfo.fname[0] == 0) 
        break;
      if(BMPFileInfo.fname[0] == '.') 
        continue;
      
      if(!(BMPFileInfo.fattrib & AM_DIR))
      {
        do
        {
          counter++;
        }
        while (BMPFileInfo.fname[counter] != 0x2E);
        
        
        if(index < MAX_BMP_FILES)
        {
          if((BMPFileInfo.altname[counter + 1] == 'B') && (BMPFileInfo.altname[counter + 2] == 'M') && (BMPFileInfo.altname[counter + 3] == 'P'))
          {
            if(sizeof(BMPFileInfo.altname) <= (MAX_BMP_FILE_NAME + 2))
            {
              sprintf (Files[index], "%s", BMPFileInfo.fname);
              index++;
            }
          }
        }
        counter = 0;
      }
    }
  }
    
  return index;
}
