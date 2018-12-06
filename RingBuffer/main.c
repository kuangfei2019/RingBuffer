/** **************************************************************************
 @Company
 LP Systems https://lpsystems.eu
 
 @File Name
 main.c
 
 @Author
 Luca Pascarella https://lucapascarella.com
 
 @Summary
 This file is a main used to test the RingBuffer library
 
 @Description
 This file is the entry point for testing the RingBuffer library.
 A preprocessor directive must be used to select the base buffer
 size that can be both POWER of 2 than arbitrary size.
 
 @License
 Copyright (C) 2016 LP Systems
 
 Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 in compliance with the License. You may obtain a copy of the License at
 
 https://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software distributed under the License
 is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 or implied. See the License for the specific language governing permissions and limitations under
 the License.
 ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "Test1.h"
#include "RingBuffer.h"

#define SPACE_SIZE      17

void printb(uint8_t *buf, size_t size);
void testLinearSpaces(RING_DATA *ring);

int main(int argc, const char * argv[]) {
    uint8_t *ptr, *ptr1, *buf, dynBuf[SPACE_SIZE];
    size_t i, toWrite, free, full, size, written, readable;
    RING_DATA *ring;
    
    printf("Start RingBuffer\n\n");
    
    printf("Test creation and destroing: %c\n", Test_CreateAndDestroyRing()?'Y':'N');
    printf("Test size: %c\n", Test_Size()?'Y':'N');
    printf("Test free space: %c\n", Test_FreeSpace()?'Y':'N');
    printf("Test full space: %c\n", Test_FullSpace()?'Y':'N');
    printf("Test fill all: %c\n", Test_FillAll()?'Y':'N');
    printf("Test multiple fill: %c\n", Test_MultipleFill()?'Y':'N');
    printf("Test multiple fill long: %c\n", Test_MultipleFillLong()?'Y':'N');
    printf("Test linear add: %c\n", Test_LinearAdd()?'Y':'N');
    printf("Test linear get: %c\n", Test_LinearGet()?'Y':'N');
    
    // This is usefull for debugging purpose when ring is rounded down than buffer size,
    // After tests not used space must contain the assigned character
    memset(dynBuf, 'X', SPACE_SIZE);
    
    dynBuf[SPACE_SIZE-1] = 'X';
    if ((ring = RING_InitBuffer(dynBuf, SPACE_SIZE)) == NULL){
        printf("Failed to allocate\n");
        return -1;
    }
    assert(ring != NULL);
    assert(dynBuf[SPACE_SIZE-1] == 'X');
    
    // testLinearSpaces(ring);
    
    // Test size
    size = RING_GetBufferSize(ring);
    printf("Rounded down from %d to %ld\n", SPACE_SIZE, size);
    //assert(size == 16);

    // Test free and full spece
    printf("\n");
    free = RING_GetFreeSpace(ring);
    full = RING_GetFullSpace(ring);
    printf("Free space: %ld. Full space: %ld\n", free, full);
    printf("Free linear space: %ld. Full linear space: %ld\n", RING_GetFreeLinearSpace(ring), RING_GetFullLinearSpace(ring));
    
    // Test add buffer
    printf("\n");
    char *str = "0123456789A"; //defghjklmnopqrst";
    // written = RING_AddBuffer(ring, (uint8_t*)str, (uint32_t)strlen(str));
    buf = RING_AddBufferDirectly(ring, &toWrite, (uint32_t)strlen(str));
    for(i=0; i < toWrite; i++)
        buf[i] = str[i];
    written = toWrite;
    printf("Written: %ld of %ld, Free space: %ld, Full space: %ld\n", written, strlen(str), RING_GetFreeSpace(ring), RING_GetFullSpace(ring));
    printf("Free linear space: %ld. Full linear space: %ld\n", RING_GetFreeLinearSpace(ring), RING_GetFullLinearSpace(ring));
    
    // Test add buffer
    printf("\n");
    char *str1 = "BCDEFGH"; //defghjklmnopqrst";
    written = RING_AddBuffer(ring, (uint8_t*)str1, (uint32_t)strlen(str1));
    printf("Written: %ld of %ld, Free space: %ld, Full space: %ld\n", written, strlen(str1), RING_GetFreeSpace(ring), RING_GetFullSpace(ring));
    printf("Free linear space: %ld. Full linear space: %ld\n", RING_GetFreeLinearSpace(ring), RING_GetFullLinearSpace(ring));
    
    // Test read
    printf("\n");
    printf("Free space: %ld. Full space: %ld\n", RING_GetFreeSpace(ring), RING_GetFullSpace(ring));
    ptr1 = malloc(SPACE_SIZE);
    buf = RING_GetBufferDirectly(ring, &readable, SPACE_SIZE/2);
    for(i=0; i<readable; i++)
        ptr1[i] = buf[i];
    printf("Readable: %ld, ", readable);
    printb(ptr1, readable);
    
    char *str2 = "KZ";
    written = RING_AddBuffer(ring, (uint8_t*)str2, (uint32_t)strlen(str2));
    
    printf("Written: %ld of %ld, Free space: %ld, Full space: %ld\n", written, strlen(str2), RING_GetFreeSpace(ring), RING_GetFullSpace(ring));
    printf("Free linear space: %ld. Full linear space: %ld\n", RING_GetFreeLinearSpace(ring), RING_GetFullLinearSpace(ring));
    
    buf = RING_GetBufferDirectly(ring, &readable, SPACE_SIZE);
    for(i=0; i<readable; i++)
        ptr1[i] = buf[i];
    printf("Readable: %ld, ", readable);
    printb(ptr1, readable);
    //free(ptr1);
    printf("Free space: %ld. Full space: %ld\n", RING_GetFreeSpace(ring), RING_GetFullSpace(ring));
    printf("Free linear space: %ld. Full linear space: %ld\n", RING_GetFreeLinearSpace(ring), RING_GetFullLinearSpace(ring));
   
    if (dynBuf[SPACE_SIZE-1] == 'X')
        printf("Ended correctly\n");
    else
        printf("Ended with errors\n");
    
    return 0;
}

void testLinearSpaces(RING_DATA *ring) {
    size_t free, full;
    
    free = RING_GetFreeSpace(ring);
    full = RING_GetFullSpace(ring);
    printf("Free space: %ld. Full space: %ld\n", free, full);
    
    free = RING_GetFreeLinearSpace(ring);
    full = RING_GetFullLinearSpace(ring);
    printf("Free linear space: %ld. Full linear space: %ld\n", free, full);
}

void printb(uint8_t *buf, size_t size) {
    size_t i;
    
    printf("'");
    for(i=0; i<size; i++)
        printf("%c", buf[i]);
    printf("'\n");
}
