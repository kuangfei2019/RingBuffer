/** **************************************************************************
 @Company
 LP Systems https://lpsystems.eu
 
 @File Name
 Test1.c
 
 @Author
 Luca Pascarella https://lucapascarella.com
 
 @Summary
 This file is the first test for the RingBuffer library
 
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

#include "Test1.h"

#define TEST_1_RING_BUFFER_SPACE        17

bool Test_CreateAndDestroyRing(void) {
    
    bool rtn = true;
    
    RING_DATA *ring1, *ring2;
    uint8_t dynBuffer[TEST_1_RING_BUFFER_SPACE];
    
    // Test user given buffer
    ring1 = RING_InitBuffer(dynBuffer, TEST_1_RING_BUFFER_SPACE);
    rtn &= (ring1 != NULL);
    
    // Test function provided buffer
    ring2 = RING_InitBuffer(NULL, TEST_1_RING_BUFFER_SPACE);
    rtn &= (ring2 != NULL);
    
    // Release memory
    RING_DeinitializeBuffer(ring1);
    RING_DeinitializeBuffer(ring2);
    
    return rtn;
}

bool Test_Size(void) {
    
    RING_DATA *ring1, * ring2;
    size_t size1, size2;
    bool rtn = true;
    
    // Create Ring objects
    ring1 = RING_InitBuffer(NULL, 19);
    rtn &= (ring1 != NULL);
    ring2 = RING_InitBuffer(NULL, 35);
    rtn &= (ring2 != NULL);
    
    // Test size
    size1 = RING_GetBufferSize(ring1);
    size2 = RING_GetBufferSize(ring2);
#ifdef POWER_2_OPTIMIZATION
    rtn &= (size1 == 16);
    rtn &= (size2 == 32);
#else
    rtn &= (size1 == 19);
    rtn &= (size2 == 35);
#endif
    
    // Destroy Ring object
    RING_DeinitializeBuffer(ring1);
    RING_DeinitializeBuffer(ring2);
    
    return rtn;
}

bool Test_FreeSpace(void) {
    RING_DATA *ring;
    size_t free, freeLinear;
    bool rtn = true;
    
    ring = RING_InitBuffer(NULL, 17);
    rtn &= (ring != NULL);
    
    free = RING_GetFreeSpace(ring);
    freeLinear = RING_GetFreeLinearSpace(ring);
#ifdef POWER_2_OPTIMIZATION
    rtn &= (free == 15);
    rtn &= (freeLinear == 14);
#else
    rtn &= (free == 16);
    rtn &= (freeLinear == 15);
#endif
    RING_DeinitializeBuffer(ring);
    
    return rtn;
}

bool Test_FullSpace(void) {
    RING_DATA *ring;
    size_t full, fullLinear;
    bool rtn = true;
    
    ring = RING_InitBuffer(NULL, 17);
    rtn &= (ring != NULL);
    
    full = RING_GetFullSpace(ring);
    fullLinear = RING_GetFullLinearSpace(ring);
#ifdef POWER_2_OPTIMIZATION
    rtn &= (full == 0);
    rtn &= (fullLinear == 0);
#else
    rtn &= (full == 0);
    rtn &= (fullLinear == 0);
#endif
    RING_DeinitializeBuffer(ring);
    
    return rtn;
}

bool Test_FillAll(void) {
    RING_DATA *ring;
    char buf[] = "0123456789ABCDEF"; // 16 bytes
    size_t len;
    bool rtn = true;
    
    // Test same size
    ring = RING_InitBuffer(NULL, 16);
    rtn &= (ring != NULL);
    len = RING_AddBuffer(ring, (uint8_t*) buf, strlen(buf));
    rtn &= (len == 15);
    RING_DeinitializeBuffer(ring);
    
    ring = RING_InitBuffer(NULL, 9);
    rtn &= (ring != NULL);
    len = RING_AddBuffer(ring, (uint8_t*) buf, strlen(buf));
#ifdef POWER_2_OPTIMIZATION
    rtn &= (len == 7);
#else
    rtn &= (len == 8);
#endif
    RING_DeinitializeBuffer(ring);
    
    return rtn;
}

bool Test_MultipleFill(void) {
    RING_DATA *ring;
    char buf1[] = "0123456789ABCDEFGHIJKLMONPQRSTUVWXYZ";
    char buf2[sizeof (buf1)];
    size_t free, full, ia, ig, added, got;
    bool rtn = true;
    
    ring = RING_InitBuffer(NULL, 9);
    rtn &= (ring != NULL);
    
    ia = ig = 0;
    do {
        free = RING_GetFreeSpace(ring);
        full = RING_GetFullSpace(ring);
#ifdef POWER_2_OPTIMIZATION
        rtn &= (free == 7);
#else
        rtn &= (free == 8);
#endif
        rtn &= (full == 0);
        
        added = RING_AddBuffer(ring, (uint8_t*) & buf1[ia], strlen(buf1));
        free = RING_GetFreeSpace(ring);
        full = RING_GetFullSpace(ring);
#ifdef POWER_2_OPTIMIZATION
        rtn &= (full == 7);
#else
        rtn &= (full == 8);
#endif
        rtn &= (free == 0);
        
        got = RING_GetBuffer(ring, (uint8_t*) & buf2[ig], added);
        ia += added;
        ig += got;
        rtn &= (added == got);
    } while (ia<sizeof (buf1));
    
    rtn &= (memcmp(buf1, buf2, sizeof (buf1)) == 0);
    
    RING_DeinitializeBuffer(ring);
    
    return rtn;
}

static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!";

bool Test_MultipleFillLong(void) {
    RING_DATA *ring;
    int testSize, i, key;
    char buffer[9];
    char *src, *dst;
    size_t ia, ig, read;
    bool rtn = true;
    
    testSize = 1351;
    src = malloc(sizeof (char) * testSize);
    dst = malloc(sizeof (char) * testSize);
    
    memset(src, '\0', testSize);
    memset(dst, '\0', testSize);
    for (i = 0; i < testSize; i++) {
        key = rand() % (int) (sizeof (charset) - 1);
        src[i] = charset[key];
    }
    
    ring = RING_InitBuffer((uint8_t*) buffer, sizeof (buffer));
    rtn &= (ring != NULL);
    
    ia = ig = 0;
    do {
        if (ia < testSize)
            ia += RING_AddBuffer(ring, (uint8_t*) & src[ia], testSize - ia);
        read = (RING_GetFullSpace(ring) / 2) + 1;
        ig += RING_GetBuffer(ring, (uint8_t*) & dst[ig], read);
    } while (ig < testSize);
    
    rtn &= (ia == ig);
    rtn &= (memcmp(src, dst, testSize) == 0);
    
    RING_DeinitializeBuffer(ring);
    
    free(src);
    free(dst);
    
    return rtn;
}

bool Test_LinearAdd(void) {
    RING_DATA *ring;
    int testSize, i, key;
    char buffer[9];
    char *src, *dst, *ptr;
    size_t ia, ig, got, toWrite, read;
    bool rtn = true;
    
    testSize = 175;
    src = malloc(sizeof (char) * testSize);
    dst = malloc(sizeof (char) * testSize);
    
    memset(src, '\0', testSize);
    memset(dst, '\0', testSize);
    for (i = 0; i < testSize; i++) {
        key = rand() % (int) (sizeof (charset) - 1);
        src[i] = charset[key];
    }
    
    ring = RING_InitBuffer((uint8_t*) buffer, sizeof (buffer));
    rtn &= (ring != NULL);
    
    ia = ig = toWrite = 0;
    do {
        if (ia < testSize) {
            ptr = (char*) RING_AddBufferDirectly(ring, &toWrite, min(16, testSize - ia));
            memcpy(ptr, &src[ia], toWrite);
            ia += toWrite;
        }
        
        read = (RING_GetFullSpace(ring) / 2) + 1;
        got = RING_GetBuffer(ring, (uint8_t*) & dst[ig], read);
        
        ig += got;
    } while (ig < testSize);
    
    rtn &= (ia == ig);
    rtn &= (memcmp(src, dst, testSize) == 0);
    
    RING_DeinitializeBuffer(ring);
    
    free(src);
    free(dst);
    
    return rtn;
}

bool Test_LinearGet(void) {
    RING_DATA *ring;
    int testSize, i, key;
    char buffer[9];
    char *src, *dst, *ptr;
    size_t ia, ig, toWrite, readable, read;
    bool rtn = true;
    
    testSize = 64;
    src = malloc(sizeof (char) * testSize);
    dst = malloc(sizeof (char) * testSize);
    
    memset(src, '\0', testSize);
    memset(dst, '\0', testSize);
    for (i = 0; i < testSize; i++) {
        key = rand() % (int) (sizeof (charset) - 1);
        src[i] = charset[key];
    }
    
    ring = RING_InitBuffer((uint8_t*) buffer, sizeof (buffer));
    rtn &= (ring != NULL);
    
    ia = ig = toWrite = 0;
    do {
        if (ia < testSize) {
            ptr = (char*) RING_AddBufferDirectly(ring, &toWrite, testSize - ia);
            memcpy(ptr, &src[ia], toWrite);
            ia += toWrite;
        }
        read = (RING_GetFullSpace(ring) / 2) + 1;
        ptr = (char*) RING_GetBufferDirectly(ring, &readable, read);
        memcpy(&dst[ig], ptr, readable);
        ig += readable;
    } while (ig < testSize);
    
    rtn &= (ia == ig);
    rtn &= (memcmp(src, dst, testSize) == 0);
    
    RING_DeinitializeBuffer(ring);
    
    free(src);
    free(dst);
    
    return rtn;
}
