//Mitchell Bond 2018
//Convert .bin to .dfu for stm32 mcu's

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "binDfuSer.h"

int main(int argc, char *argv[]){
    FILE *bfp; //Input file pointer (.bin)
    FILE *dfp; //Output file pointer (.dfu)
    
    char *dfu; //Pointer for memory allocation
    unsigned int k = 0; //Idx for pointer
    unsigned int prefixIdx;
    
    long int dfuSize;
    long int targetSize;
    long int *targetSizePtr;
    long * ptr; 
    
    unsigned long int crc = 0xFFFFFFFF;
    char * dfuCrc;
    unsigned long int len;
    
   
    if(argc != 3){
        printf("ERR: Not enough commands entered\n");
        printf("%s input.bin output.dfu\n", argv[0]);
    }else{
        //Check to see if input is readable?
        bfp = fopen(argv[1], "rb");
        if(!bfp){
            printf("Input file error\n");
            return -1;
        }
        
        //Check to see if output is possible
        dfp = fopen(argv[2], "wb");
        if(!dfp){
            printf("Output file error\n");
            return -1;
        }
        
        /* Start making up the dfu file
           Checkout STM UM0391 for DFU file format
            ------------------
               DFU - PREFIX
            ------------------
        
               DFU - IMAGES
        
            ------------------
               DFU - SUFFIX
            ------------------
        */
        
        
        /*     DFU - IMAGES
        Assuming a single image
            ------------------
              Target Prefix
            ------------------
             Image Element 1
            ------------------
             Image Element 2
            ------------------
                  ....
        
        */
        //Reserve memory for dfu file
        fseek(bfp, 0L, SEEK_END);
        targetSize = ftell(bfp);
        fseek(bfp, 0L, SEEK_SET); //Set pointer to the begining of the file
        
        
        dfuSize = PREFIX_SIZE + SUFFIX_SIZE;
        dfuSize += IMAGE_ELEMENT_PRE_SIZE + targetSize + TARGET_PREFIX_SIZE;
        dfu = (char *)calloc(0, dfuSize);
        dfuCrc = dfu;
        
        if(!dfu){
            printf("Error Allocating Memory\n");
            return -1;
        }
        
        //DFU - PREFIX
        // szSignature - offset 0
        dfu[k++] = 'D';
        dfu[k++] = 'f';
        dfu[k++] = 'u';
        dfu[k++] = 'S';
        dfu[k++] = 'e';
        //  bVersion - offset 5
        dfu[k++] = 0x01;
        //  DFUImageSize - offset 6 (big endian)
        dfu[k++] = (dfuSize & 0x000000ff);
        dfu[k++] = (dfuSize & 0x0000ff00) >> 8;
        dfu[k++] = (dfuSize & 0x00ff0000) >> 16;
        dfu[k++] = (dfuSize & 0xff000000) >> 24;
        // bTargets - offset 10
        dfu[k++] = 0x01;
        
        //TARGET PREFIX
        prefixIdx = k;
        //  szSignature ("Target")
        dfu[k++] = 'T';
        dfu[k++] = 'a';
        dfu[k++] = 'r';
        dfu[k++] = 'g';
        dfu[k++] = 'e';
        dfu[k++] = 't';
        //  bAlternative Setting
        dfu[k++] = 0x00;
        //  bTargetNamed
        dfu[k++] = 0x01;
        k += 3;
        // bTargetName
        dfu[k++] = 'S';
        dfu[k++] = 'T';
        dfu[k++] = '.';
        dfu[k++] = '.';
        dfu[k++] = '.';
        // dwTargetSize - Skip to that section
        k = prefixIdx + 266;

        dfu[k++] = ((targetSize + 8) & 0x000000ff);
        dfu[k++] = ((targetSize + 8) & 0x0000ff00) >> 8;
        dfu[k++] = ((targetSize + 8) & 0x00ff0000) >> 16;
        dfu[k++] = ((targetSize + 8) & 0xff000000) >> 32;
        //  dwNbElements
        dfu[k++] = ((numberElements) & 0x000000ff);
        dfu[k++] = ((numberElements) & 0x0000ff00) >> 8;
        dfu[k++] = ((numberElements) & 0x00ff0000) >> 16;
        dfu[k++] = ((numberElements) & 0xff000000) >> 32;
        
        //IMAGE ELEMENT
        //dwElementAddress
        dfu[k++] = ((elementAddress) & 0x000000ff);
        dfu[k++] = ((elementAddress) & 0x0000ff00) >> 8;
        dfu[k++] = ((elementAddress) & 0x00ff0000) >> 16;
        dfu[k++] = ((elementAddress) & 0xff000000) >> 24;
        //dwElementSize
        dfu[k++] = ((targetSize) & 0x000000ff);
        dfu[k++] = ((targetSize) & 0x0000ff00) >> 8;
        dfu[k++] = ((targetSize) & 0x00ff0000) >> 16;
        dfu[k++] = ((targetSize) & 0xff000000) >> 32;
        //Data
        fread(&dfu[k], targetSize, 1, bfp);
        //Set pointer
        k += targetSize;
  
        //DFU - SUFFIX
        //bcdDevice
        dfu[k++] = (PID & 0xFF);
        dfu[k++] = (PID & 0xFF00) >> 8;
        //idProduct PID
        dfu[k++] = (PID & 0xFF);
        dfu[k++] = (PID & 0xFF00) >> 8;
        //bcdVendor VID
        dfu[k++] = (VID & 0xFF);
        dfu[k++] = (VID & 0xFF00) >> 8;
        //bcdDFULo
        dfu[k++] = 0x1A;
        //bcdDFUHi
        dfu[k++] = 0x01;
        //ucDfuSignature
        dfu[k++] = 'U';
        dfu[k++] = 'F';
        dfu[k++] = 'D';
        //bLength
        dfu[k++] = 16;
        //dwCRC

        len = dfuSize - 4;
              
        while(len > 0){
            crc = UPDC32((char)*dfuCrc, crc);
            dfuCrc++;
            len--;
        };
        
        dfu[k++] = ((crc) & 0x000000ff);
        dfu[k++] = ((crc) & 0x0000ff00) >> 8;
        dfu[k++] = ((crc) & 0x00ff0000) >> 16;
        dfu[k++] = ((crc) & 0xff000000) >> 24;

        fwrite(dfu, dfuSize, 1, dfp);
        
        free(dfu);
        fclose(dfp);
        fclose(bfp);
         
        
    }
}

