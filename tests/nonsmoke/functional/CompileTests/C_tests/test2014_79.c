
typedef void (*foobar_type)(void);

void foobar_1(void);
void foobar_2(void);

// This seconedary declaration must come before the declaration of the array bound and the array bound is an enum.
extern foobar_type gmr1_ie_common_func[];

enum gmr1_ie_common_idx 
   {
     GMR1_IE_COM_CM2,
     GMR1_IE_COM_SPARE_NIBBLE,
     NUM_GMR1_IE_COMMON
   };

foobar_type gmr1_ie_common_func[NUM_GMR1_IE_COMMON] = 
   {
     foobar_1,
     foobar_2,
   };



