#include "crc_calc.h"

#define VID 0x483
#define PID 0xdf11
#define PREFIX_SIZE 11
#define SUFFIX_SIZE 16
#define TARGET_PREFIX_SIZE 274
#define IMAGE_ELEMENT_PRE_SIZE 8

const long int numberElements = 1;
const long int elementAddress = 0x08000000;

