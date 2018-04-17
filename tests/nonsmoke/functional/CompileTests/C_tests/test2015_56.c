
// typedef unsigned char uint8_t;
// inline static uint8_t readb(volatile uint8_t *io_addr) __attribute__((no_instrument_function)) __attribute__((always_inline)) __attribute__((visibility("hidden")));

static inline __attribute__ (( no_instrument_function )) __attribute__ (( always_inline )) void
iodelay ( void ) 
   {
   }

void iodelay ( void );

static void efi_iodelay ( void ) 
   {
   }

/* Should be unparsed as:
   typeof ( iodelay ) iodelay; 
   typeof ( efi_iodelay ) efi_iodelay; 
   typeof ( iodelay ) efi_iodelay; 
   extern typeof ( iodelay ) __efi_iodelay; 
   typeof ( iodelay ) __efi_iodelay __attribute__ (( alias ( "efi_iodelay" ) ));;
But is unparsed as:
   inline static void iodelay() __attribute__((no_instrument_function)) __attribute__((always_inline)) __attribute__((visibility("hidden")));
   static void efi_iodelay() __attribute__((visibility("hidden")));
   static void efi_iodelay() __attribute__((visibility("hidden")));
   extern void __efi_iodelay() __attribute__((visibility("hidden"))) __attribute__((alias("efi_iodelay")));
   void __efi_iodelay() __attribute__((visibility("hidden"))) __attribute__((alias("efi_iodelay")));
 */
typeof ( iodelay ) iodelay; 
typeof ( efi_iodelay ) efi_iodelay; 
typeof ( iodelay ) efi_iodelay; 
extern typeof ( iodelay ) __efi_iodelay; 
typeof ( iodelay ) __efi_iodelay __attribute__ (( alias ( "efi_iodelay" ) ));;

