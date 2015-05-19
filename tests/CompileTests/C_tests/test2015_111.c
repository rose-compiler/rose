
void iodelay ( void );

static void efi_iodelay ( void ) 
   {
   }

// extern typeof ( iodelay ) __efi_iodelay; 
extern typeof ( iodelay ) __efi_iodelay; 

// typeof ( iodelay ) __efi_iodelay __attribute__ (( alias ( "efi_iodelay" ) ));;
typeof ( iodelay ) __efi_iodelay __attribute__ (( alias ( "efi_iodelay" ) ));;
