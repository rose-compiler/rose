void _start(void) {
  asm ("mov $1, %eax; mov $2, %ebx; int $0x80; hlt");
}
