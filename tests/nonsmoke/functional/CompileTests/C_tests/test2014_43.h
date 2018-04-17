#define VEX_REGPARM(_n) __attribute__((regparm(_n)))

typedef long V128;

extern VEX_REGPARM(3)
       void h_generic_calc_Mul32x4    ( /*OUT*/V128*, V128*, V128* );
extern VEX_REGPARM(3)
       void h_generic_calc_Max32Sx4   ( /*OUT*/V128*, V128*, V128* );
extern VEX_REGPARM(3)
       void h_generic_calc_Min32Sx4   ( /*OUT*/V128*, V128*, V128* );
extern VEX_REGPARM(3)
       void h_generic_calc_Max32Ux4   ( /*OUT*/V128*, V128*, V128* );
