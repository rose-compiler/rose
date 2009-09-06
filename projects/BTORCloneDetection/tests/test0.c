void _start(void) {
  asm ("mov $1, %eax; mov %eax, 4(%esp); mov 4(%esp), %ebx; mov %ebx, 8(%esp); int $0x80; hlt");
}
