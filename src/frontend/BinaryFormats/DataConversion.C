/* Functions for data conversion. Functions such as byte swapping, etc. */
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

uint8_t *
Rot13::encode(uint8_t *buffer, size_t *nbytes)
{
    for (size_t i=0; i<*nbytes; i++)
        buffer[i] += 13;
    return buffer;
}

uint8_t *
Rot13::decode(uint8_t* buffer, size_t* nbytes)
{
    for (size_t i=0; i<*nbytes; i++)
        buffer[i] -= 13;
    return buffer;
}

#endif
