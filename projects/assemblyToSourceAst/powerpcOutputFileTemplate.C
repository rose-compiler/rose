#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint16_t mulhi16(uint16_t a, uint16_t b);
uint32_t mulhi32(uint32_t a, uint32_t b);
uint64_t mulhi64(uint64_t a, uint64_t b);
uint16_t imulhi16(uint16_t a, uint16_t b);
uint32_t imulhi32(uint32_t a, uint32_t b);
uint64_t imulhi64(uint64_t a, uint64_t b);
uint8_t div8(uint16_t a, uint8_t b);
uint8_t mod8(uint16_t a, uint8_t b);
uint16_t div16(uint16_t ah, uint16_t al, uint16_t b);
uint16_t mod16(uint16_t ah, uint16_t al, uint16_t b);
uint32_t div32(uint32_t ah, uint32_t al, uint32_t b);
uint32_t mod32(uint32_t ah, uint32_t al, uint32_t b);
uint64_t div64(uint64_t ah, uint64_t al, uint64_t b);
uint64_t mod64(uint64_t ah, uint64_t al, uint64_t b);
uint8_t idiv8(uint16_t a, uint8_t b);
uint8_t imod8(uint16_t a, uint8_t b);
uint16_t idiv16(uint16_t ah, uint16_t al, uint16_t b);
uint16_t imod16(uint16_t ah, uint16_t al, uint16_t b);
uint32_t idiv32(uint32_t ah, uint32_t al, uint32_t b);
uint32_t imod32(uint32_t ah, uint32_t al, uint32_t b);
uint64_t idiv64(uint64_t ah, uint64_t al, uint64_t b);
uint64_t imod64(uint64_t ah, uint64_t al, uint64_t b);
int bsr(uint64_t a);
int bsf(uint64_t a);

void systemCall(uint8_t a);

unsigned int gpr[32];
unsigned char cr[8];
uint32_t ip;

// DQ (10/25/2008): Added spr register array.  This might have to be
// a lenth to fix register numbers 1-287 (9 bits, at least).  See:
// http://www.go-ecs.com/ppc/ppctek1.htm for SPR details.
// For "user mode" we only require entries 1,8, and 9.
unsigned int spr[32];

uint8_t memoryReadByte(uint64_t addr) {
  return 0;
}

uint16_t memoryReadWord(uint64_t addr) {
  return 0;
}

uint32_t memoryReadDWord(uint64_t addr) {
  return 0;
}

uint64_t memoryReadQWord(uint64_t addr) {
  return 0;
}

void memoryWriteByte(uint64_t addr, uint8_t data) {
}

void memoryWriteWord(uint64_t addr, uint16_t data) {
}

void memoryWriteDWord(uint64_t addr, uint32_t data) {
}

void memoryWriteQWord(uint64_t addr, uint64_t data) {
}

void fail() {
  abort();
}

void startingInstruction() {
  printf("Starting 0x%08X\n", ip);
}

void run();

int main(int, char**) {
  run();
  return 0;
}
