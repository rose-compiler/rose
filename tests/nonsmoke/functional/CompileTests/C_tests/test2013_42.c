typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

static int sad_hpel_motion_search( // MpegEncContext * s,
                                  int *mx_ptr, int *my_ptr, int dmin,
                                  int src_index, int ref_index,
                                  int size, int h)
{
//    MotionEstContext * const c= &s->me;
   const int penalty_factor; // = c->sub_penalty_factor;
    int mx, my, dminh;
    uint8_t *pix, *ptr;
    int stride; // = c->stride;
    const int flags; // = c->sub_flags;
//  uint32_t __attribute__((unused)) * const score_map= c->score_map; const int __attribute__((unused)) xmin= c->xmin; const int __attribute__((unused)) ymin= c->ymin; const int __attribute__((unused)) xmax= c->xmax; const int __attribute__((unused)) ymax= c->ymax; uint8_t *mv_penalty= c->current_mv_penalty; const int pred_x= c->pred_x; const int pred_y= c->pred_y;
    uint32_t* const score_map; // = c->score_map; 
    const int xmin; //= c->xmin; 
    const int ymin; //= c->ymin; 
    const int xmax; //= c->xmax; 
    const int ymax; //= c->ymax; 
    uint8_t *mv_penalty; //= c->current_mv_penalty; 
    const int pred_x; //= c->pred_x; 
    const int pred_y; //= c->pred_y;

    ((void)0);

#if 0
    if(c->skip){
        *mx_ptr = 0;
        *my_ptr = 0;
        return dmin;
    }

    pix = c->src[src_index][0];

    mx = *mx_ptr;
    my = *my_ptr;
    ptr = c->ref[ref_index][0] + (my * stride) + mx;

    dminh = dmin;
#endif

    if (mx > xmin && mx < xmax &&
        my > ymin && my < ymax) {
        int dx=0, dy=0;
        int d, pen_x, pen_y;
        const int index= (my<<3) + mx;
#if 0
        const int t= score_map[(index-(1<<3))&(64 -1)];
        const int l= score_map[(index- 1 )&(64 -1)];
        const int r= score_map[(index+ 1 )&(64 -1)];
        const int b= score_map[(index+(1<<3))&(64 -1)];
#else
        const int t; // = score_map[(index-(1<<3))&(64 -1)];
        const int l; // = score_map[(index- 1 )&(64 -1)];
        const int r; // = score_map[(index+ 1 )&(64 -1)];
        const int b; // = score_map[(index+(1<<3))&(64 -1)];
#endif
        mx <<= 1;
        my <<= 1;

        pen_x= pred_x + mx;
        pen_y= pred_y + my;

        ptr-= stride;
        if(t<=b)
        {
           {
          // d= s->dsp.pix_abs[size][(0?1:0)+(-1?2:0)] ( ((void *)0), pix, ptr+((0)>>1), stride, h); 
             d += (mv_penalty[pen_x + 0] + mv_penalty[pen_y + -1])*penalty_factor; 
             __asm__ volatile( "cmpl  %0, %3       \n\t" "cmovl %3, %0       \n\t" "cmovl %4, %1       \n\t" "cmovl %5, %2       \n\t" : "+&r" (dminh), "+&r" (dx), "+r" (dy) : "r" (d), "r" (0), "r" (-1));


           }

            {
           // d = s->dsp.pix_abs[size][(0?1:0)+(-1?2:0)](((void *)0), pix, ptr+((0)>>1), stride, h); 
              d += (mv_penalty[pen_x + 0] + mv_penalty[pen_y + -1])*penalty_factor; 
              asm ( "cmpl  %0, %3       \n\t" "cmovl %3, %0       \n\t" "cmovl %4, %1       \n\t" "cmovl %5, %2       \n\t" : "+&r" (dminh), "+&r" (dx), "+r" (dy) : "r" (d), "r" (0), "r" (-1));
            }
#if 0
            { 
              const int hx= 2*(mx)+(0); 
              const int hy= 2*(my-1)+(1); 
           // d= cmp_hpel(s, mx, my-1, 0, 1, size, h, ref_index, src_index, cmp_sub, chroma_cmp_sub, flags); 
              d += (mv_penalty[hx - pred_x] + mv_penalty[hy - pred_y])*penalty_factor; 
              __asm__ volatile( "cmpl  %0, %3       \n\t" "cmovl %3, %0       \n\t" "cmovl %4, %1       \n\t" "cmovl %5, %2       \n\t" : "+&r" (dmin), "+&r" (bx), "+r" (by) : "r" (d), "r" (hx), "r" (hy));
            }
#endif
        }
    }
}
