#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

bool parity(uint8_t val) {
  val ^= (val >> 4);
  val ^= (val >> 2);
  val ^= (val >> 1);
  return (val & 1) != 0;
}

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

// These are trimmed to 32 bits for now
unsigned int rax, rbx, rcx, rdx, rsi, rdi, rsp, rbp, r8, r9, r10, r11, r12, r13, r14, r15;
// These are the 8-bit lower bytes, which seem to be used fairly often
unsigned int al, bl, cl, dl, sil, dil, spl, bpl, r8b, r9b, r10b, r11b, r12b, r13b, r14b, r15b;
uint32_t ip;
bool cf, pf, af, zf, sf, df, of;
bool sf_xor_of;
bool zf_or_cf;

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

void interrupt(int x) {
  printf("int 0x%02X\n", x);
}

void startingInstruction() {
  printf("Starting 0x%08X\n", ip);
}

void run();

int main(int, char**) {
  run();
  return 0;
}
