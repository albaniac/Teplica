#ifndef _MEMORY_H
#define _MEMORY_H
//--------------------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>
//--------------------------------------------------------------------------------------------------------------------------------
void MemInit();
uint8_t MemRead(unsigned int address);
void MemWrite(unsigned int address, uint8_t val);
void* MemFind(const void *haystack, size_t n, const void *needle, size_t m);
//--------------------------------------------------------------------------------------------------------------------------------

#endif
