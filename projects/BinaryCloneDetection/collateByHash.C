#include <stdio.h>
#include <string>
#include <sstream>
#include <iomanip>

int main(int, char**) {
  FILE* outfiles[256];
  for (size_t i = 0; i < 256; ++i) {
    char pipename[100];
    sprintf(pipename, "gzip > collated_%02X.gz", (unsigned int)i);
    outfiles[i] = popen(pipename, "w");
    if (!outfiles[i]) {
      perror("popen: ");
      exit (1);
    }
  }
  size_t linecount = 0;
  while (true) {
    char line[65537];
    bool result = (bool)fgets(line, 65536, stdin);
    if (!result && feof(stdin)) break;
    if (!result && !feof(stdin)) {
      perror("fgets: ");
      exit (1);
    }
    line[65536] = 0;
    size_t lineLength = strchr(line, '\n') - line;
    if (lineLength == 65536) {
      fprintf(stderr, "Line more than 64k long\n");
      exit (1);
    }
    line[lineLength] = 0;
    unsigned char hash = 0;
    for (size_t i = 0; i < lineLength; ++i) {
      hash = (hash << 1) ^ (hash >> 7) ^ (line[i] == ' ' ? 0xA : (line[i] & 15));
    }
    fputs(line, outfiles[hash]);
    fputc('\n', outfiles[hash]);
    ++linecount;
    if (linecount % 100000 == 0) {
      fprintf(stderr, "Finished line %zu\n", linecount);
    }
  }
  for (size_t i = 0; i < 256; ++i) {
    pclose(outfiles[i]);
  }
}
