////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  29.file  |/usr/include/bits/stdio.h
//  29.86    |# ifdef __USE_MISC
//  29.87    |/* Faster version when locking is not necessary.  */
//  29.88    |__STDIO_INLINE int
//  29.89    |fputc_unlocked (int __c, FILE *__stream)
//  29.90    |{
//  29.91    |  return _IO_putc_unlocked (__c, __stream);
// 459.12    #           0x0804b172:  mov    edx, DWORD PTR ds:[eax + 0x14]
// 459.13    |           0x0804b175:  cmp    edx, DWORD PTR ds:[eax + 0x18]
// 459.14    |           0x0804b178:  jae    0x0804b1cb<<Insn>>
// 459.15    |        1x 0x0804b17a:  mov    BYTE PTR ds:[edx], bl
// 459.16    |           0x0804b17c:  add    edx, 0x01
// 459.17    |           0x0804b17f:  mov    DWORD PTR ds:[eax + 0x14], edx
// 459.43    #        1x 0x0804b1cb:  movzx  ebx, bl
// 459.44    |           0x0804b1ce:  mov    DWORD PTR ss:[esp + 0x04], ebx
// 459.45    |           0x0804b1d2:  mov    DWORD PTR ss:[esp], eax
// 459.46    |           0x0804b1d5:  call   0x08049528<__overflow@plt>
// 459.47    |        1x 0x0804b1da:  jmp    0x0804b182<<Insn>>
//  29.92    |}
//  29.93    |# endif /* misc */
int __overflow(unsigned char *stream, int c);

int
_IO_putc_unlocked(unsigned c, unsigned char *stream)
{
    unsigned char *m1 = *(unsigned char**)(stream+0x14);
    unsigned char *m2 = *(unsigned char**)(stream+0x18);
    if (m1 < m2) {
        *m1++ = c;
        *(unsigned char**)(stream+0x14) += 1;
    }
    return __overflow(stream, c & 0xff);
}

int
fputc_unlocked(int c, unsigned char *stream)
{
    return _IO_putc_unlocked(c, stream);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
