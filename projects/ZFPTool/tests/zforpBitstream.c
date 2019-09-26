#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "bitstream.h"

  typedef struct bitstream_container bitstream_container;
  struct bitstream_container {
    bitstream* stream;
  };


int main()
{
  // setup

  size_t initialBufferSizeBytes = 5;
  void* buffer = calloc(initialBufferSizeBytes, sizeof(char));
  if (buffer == NULL) {
    printf("Failed to allocate memory for buffer\n");
    return 1;
  }

  bitstream_container bs;
  bs.stream = stream_open(buffer, initialBufferSizeBytes);

  // test assertions
  // (would like to automatically substitute fortran wrappers in place of C API calls)

  size_t observedBufferSizeBytes;
  observedBufferSizeBytes = stream_buffer_size(bs.stream);
  assert(observedBufferSizeBytes == initialBufferSizeBytes);

  size_t newBufferSizeBytes = 11;
  stream_set_buffer_size(bs.stream, newBufferSizeBytes);
  observedBufferSizeBytes = stream_buffer_size(bs.stream);
  assert(observedBufferSizeBytes == newBufferSizeBytes);

  // finish, free
  stream_close(bs.stream);
  free(buffer);

  return 0;
}
