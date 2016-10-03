typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef uint8_t intptr_t;

// DECLARE_ALIGNED(16, static const uint16_t, pw_7f)[8] = {0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F};
// static const int __attribute__ ((aligned (16))) pw_7f[2] = {0x00,0x60};
static const int pw_7f[2] = {0x00,0x60};

static void gradfun_filter_line_mmxext(uint8_t *dst, const uint8_t *src, const uint16_t *dc,
                                       int width, int thresh,
                                       const uint16_t *dithers)
{
    intptr_t x;
    if (width & 3) {
        x = width & ~3;
        ff_gradfun_filter_line_c(dst + x, src + x, dc + x / 2, width - x, thresh, dithers);
        width = x;
    }
    x = -width;
    __asm__ volatile(
        "movd          %4, %%mm5 \n"
        "pxor       %%mm7, %%mm7 \n"
        "pshufw $0, %%mm5, %%mm5 \n"
        "movq          %6, %%mm6 \n"
        "movq          (%5), %%mm3 \n"
        "movq         8(%5), %%mm4 \n"
        "1: \n"
#if 0
     // These will cause the gnu asmbler to fail to compile the generated code (not a ROSE issue).
        "movd     (%2,%0), %%mm0 \n"
        "movd     (%3,%0), %%mm1 \n"
#endif
        "punpcklbw  %%mm7, %%mm0 \n"
        "punpcklwd  %%mm1, %%mm1 \n"
        "psllw         $7, %%mm0 \n"
#if 0
     // These will cause the gnu asmbler to fail to compile the generated code (not a ROSE issue).
        "pxor       %%mm2, %%mm2 \n"
        "psubw      %%mm0, %%mm1 \n" // delta = dc - pix
        "psubw      %%mm1, %%mm2 \n"
        "pmaxsw     %%mm1, %%mm2 \n"
        "pmulhuw    %%mm5, %%mm2 \n" // m = abs(delta) * thresh >> 16
#endif
#if 0
     // These will cause the gnu asmbler to fail to compile the generated code (not a ROSE issue).
        "psubw      %%mm6, %%mm2 \n"
        "pminsw     %%mm7, %%mm2 \n" // m = -max(0, 127-m)
        "pmullw     %%mm2, %%mm2 \n"
        "paddw      %%mm3, %%mm0 \n" // pix += dither
        "psllw         $2, %%mm1 \n" // m = m*m*delta >> 14
        "pmulhw     %%mm2, %%mm1 \n"
        "paddw      %%mm1, %%mm0 \n" // pix += m
        "psraw         $7, %%mm0 \n"
        "packuswb   %%mm0, %%mm0 \n"
        "movd       %%mm0, (%1,%0) \n" // dst = clip(pix>>7)
        "add           $4, %0 \n"
        "jnl 2f \n"
#endif
#if 0
     // These will cause the gnu asmbler to fail to compile the generated code (not a ROSE issue).
        "movd     (%2,%0), %%mm0 \n"
        "movd     (%3,%0), %%mm1 \n"
        "punpcklbw  %%mm7, %%mm0 \n"
        "punpcklwd  %%mm1, %%mm1 \n"
        "psllw         $7, %%mm0 \n"
        "pxor       %%mm2, %%mm2 \n"
        "psubw      %%mm0, %%mm1 \n" // delta = dc - pix
        "psubw      %%mm1, %%mm2 \n"
        "pmaxsw     %%mm1, %%mm2 \n"
        "pmulhuw    %%mm5, %%mm2 \n" // m = abs(delta) * thresh >> 16
        "psubw      %%mm6, %%mm2 \n"
        "pminsw     %%mm7, %%mm2 \n" // m = -max(0, 127-m)
        "pmullw     %%mm2, %%mm2 \n"
        "paddw      %%mm4, %%mm0 \n" // pix += dither
        "psllw         $2, %%mm1 \n" // m = m*m*delta >> 14
        "pmulhw     %%mm2, %%mm1 \n"
        "paddw      %%mm1, %%mm0 \n" // pix += m
        "psraw         $7, %%mm0 \n"
        "packuswb   %%mm0, %%mm0 \n"
        "movd       %%mm0, (%1,%0) \n" // dst = clip(pix>>7)
#endif
        "add           $4, %0 \n"
        "jl 1b \n"

        "2: \n"
        "emms \n"
        :"+r"(x)
        :"r"(dst+width), "r"(src+width), "r"(dc+width/2),
         "rm"(thresh), "r"(dithers), "m"(*pw_7f)
        :"memory"
    );
}
