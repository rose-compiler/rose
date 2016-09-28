/*

CodeCrypt 0.0 Written by Tibbar @ GSO.  You may use this code in your
own projects for non-commercial purposes provided you give credit to
the author.

This is provided for educational purposes only.  No legal
reponsibility is held or accepted by the author for misuse of this
code.

This code was substantially changed by Kirk Sayre (sayre@cert.org).
*/

#define _WIN32_WINNT 0x0500

#include <string.h>
#include <stdio.h> 

#include "windows.h"
#include "winbase.h"
#include "limits.h"

// Do correct pointer addition.
// cast - The type of pointer to create.
// ptr, addValue - The pointers to add together.
#define MakePtr( cast, ptr, addValue ) (cast)( (DWORD_PTR)(ptr) + (DWORD_PTR)(addValue))

#ifndef IMAGE_FIRST_SECTION32
#define IMAGE_FIRST_SECTION32( ntheader ) ((PIMAGE_SECTION_HEADER)        \
    ((UINT_PTR)ntheader +                                                  \
     FIELD_OFFSET( IMAGE_NT_HEADERS32, OptionalHeader ) +                 \
     ((PIMAGE_NT_HEADERS32)(ntheader))->FileHeader.SizeOfOptionalHeader   \
    ))

#endif

// Taken from Ashkbiz Danehkar's PE protector code:
// (http://www.codeproject.com/cpp/peprotector1.asp)
//
// Get the size (in bytes) of a function.
//
// Note that this function works by looking for the bytes
// corresponding to the string "ETGXZKATZ" at the end of the
// function. In assembly this string of bytes is the following code:
//
//    INC EBP     ;'E'
//    PUSH ESP    ;'T'
//    INC EDI     ;'G'
//    POP EAX     ;'X'
//    POP EDX     ;'Z'
//    DEC EBX     ;'K'
//    INC ECX     ;'A'
//    PUSH ESP    ;'T'
//    POP EDX     ;'Z'
//
// These bytes are just used as a marker, they will not be included in
// the size of the function.
//
// Body - Pointer to the body of the function for which to get its
// size in bytes. The function should have the __declspec( naked )
// attribute. The function also must be declared as static (or hoops
// must be jumped through to get a pointer that actually points to the
// function code rather than to a jump table entry).
DWORD GetFunctionSize(void (*Body)(void)) {
  char* pFuncBody=(char *) Body;
  UCHAR _temp;
  bool notEnd=TRUE;
  char *DepackerCodeEnd=new char[10];
  DWORD l=0;
  do
    {
      CopyMemory(&_temp,pFuncBody+l,1);
      if(_temp==0xC3)
	{
	  CopyMemory(DepackerCodeEnd,pFuncBody+l+0x01,10);
	  DepackerCodeEnd[9]=0x00;
	  if(strcmp(DepackerCodeEnd,"ETGXZKATZ")==0)
	    {
	      notEnd=FALSE;
	    }
	}
      l++;
    }while(notEnd);
  return(l);
}

// Taken from Ashkbiz Danehkar's PE protector code:
// (http://www.codeproject.com/cpp/peprotector1.asp)
//
// Return the bytes of a named function.
char* CopyFunction(void (*Body)(void)) {
    DWORD dwSize=GetFunctionSize(Body);
    char* pFuncBody=(char *) Body;
    char* filebuff=new char[dwSize+1];
    CopyMemory(filebuff,pFuncBody,dwSize);
    return(filebuff);
}

// This is the unpacker stub. It will be compiled and copied to the
// packed file.
//
// This has to have:
//
// __declspec( naked ) - So that the standard function preamble and
// postamble is not generated.
//
// static - So that the '&' address of the function does not refer to
// a jump table entry but to the actual start of the function itself.
__declspec( naked ) static void DecryptAndStart() {

  /*  
  _asm
    {
      // This call instruction is 5 bytes.
      call top; 
    top:
      // Get the address right after 'call top'.
      pop     EAX; 

      // Subtract 5 to get past 'call top'. This is the starting memory
      // address of the stub.
      sub     EAX,5; 

      // The data needed to perform the unpacking is stored in the 4
      // DWORDs right before the unpacker stub. Pull the 4 pieces of
      // data out into registers.

      // Get the # of bytes to unpack in EDX.
      mov EDX, [EAX - 4*1];

      // Get the base image address in EBX.
      mov EBX, [EAX - 4*2];

      // Get the RVA of the original entry point.
      mov ECX, [EAX - 4*3];

      // Get the RVA of the code to unpack.
      mov EAX, [EAX - 4*4];

      // Compute the actual address of the code to unpack by adding
      // the image base address (EBX) to the RVA of the code to unpack
      // (EAX).
      add EAX, EBX;

      // Compute the actual address of the original entry point by adding
      // the image base address (EBX) to the RVA of the entry point
      // (ECX).
      add ECX, EBX;

      // Put the current address being decrypted into EBX.
      mov EBX, EAX;
      // Put the last address to be decrypted into EDX.
      add EDX, EAX;
      
    StartLoop:
      // Have we reached the last byte to decrypt?
      CMP EBX, EDX;
      // If so, stop decrypting.
      JZ StopLooping;
      
      // Get the encrypted byte.
      mov AL, BYTE PTR [EBX];

      // Decrypt the byte by subtracting 1 from it.
      dec AL;

      // Put the decrypted byte back into the code block.
      mov BYTE PTR [EBX], AL;
      
      // Move to the next to decrypt.
      inc EBX;
      jmp StartLoop;

    StopLooping:

      // The code section has been decrypted. Jump to the original
      // program entry point and run the original program.
      jmp ECX;

      // This ret is important. It is used by GetFunctionSize() to
      // indicate that the code end marker string is next.
      ret;

      // These bytes are needed by GetFunctionSize() to mark the end
      // of the function.
      INC EBP     ;//'E'
      PUSH ESP    ;//'T'
      INC EDI     ;//'G'
      POP EAX     ;//'X'
      POP EDX     ;//'Z'
      DEC EBX     ;//'K'
      INC ECX     ;//'A'
      PUSH ESP    ;//'T'
      POP EDX     ;//'Z'
    }  
  */
}

// Find the last section in the PE file that is currently mapped into
// memory. The 'last' section is defined as the section with the
// largest RVA.
//
// Params:
//
// firstSectionHeader - A pointer to the section header for the 1st
// section.
//
// numSections - The number of sections in the PE file.
//
// sectionBase - The virtual address of the section we are looking
// for.
//
// Return:
//
// A pointer to the header of the section with the largest RVA.
PIMAGE_SECTION_HEADER 
FindLastSection(IMAGE_SECTION_HEADER* firstSectionHeader, 
		DWORD numSections) {

    DWORD currentMaxVA = 0;
    IMAGE_SECTION_HEADER* lastSectionHeader = NULL;

    // Look through all of the section headers, and find the one with
    // the largest RVA.
    IMAGE_SECTION_HEADER* tempHeader = firstSectionHeader;
    DWORD headerSize = sizeof(IMAGE_SECTION_HEADER);
    unsigned i;
    for(i = 0; i < numSections; i++)
    {
        if((DWORD)(tempHeader->VirtualAddress) > currentMaxVA) 
        {
            currentMaxVA = (DWORD)(tempHeader->VirtualAddress);
	    lastSectionHeader = tempHeader;
        }
        tempHeader = MakePtr(IMAGE_SECTION_HEADER*, tempHeader, headerSize);
    }

    return lastSectionHeader;
}

// Find the section that has the given virtual address.
//
// Params:
//
// firstSectionHeader - A pointer to the section header for the 1st
// section.
//
// numSections - The number of sections in the PE file.
//
// sectionBase - The virtual address of the section we are looking
// for.
//
// Return:
//
// If found, a pointer to the section header of the section with the
// given virtual address.
// If not found, return NULL.
PIMAGE_SECTION_HEADER FindSection(IMAGE_SECTION_HEADER* firstSectionHeader, 
				  DWORD numSections,
				  DWORD sectionbase) {
    DWORD currentMaxVA = 0;
    int pos = 0;

    // Look through the section headers, trying to find one whose
    // virtual address is equal to sectionBase.
    IMAGE_SECTION_HEADER* tempHeader = firstSectionHeader;
    DWORD headerSize = sizeof(IMAGE_SECTION_HEADER);
    unsigned i;
    for(i = 0; i < numSections; i++)
    {
        if (tempHeader->VirtualAddress == sectionbase) 
        {
            return tempHeader;
        }
        tempHeader = MakePtr(IMAGE_SECTION_HEADER*, tempHeader, headerSize);
    }

    return NULL;
}

// Make all of the sections in the PE file mapped to memory writable.
//
// Params:
//
// firstSectionHeader - A pointer to the section header for the 1st
// section.
//
// numSections - The number of sections in the PE file.
void MakeAllSectionsWritable(IMAGE_SECTION_HEADER* firstSectionHeader, 
                             DWORD numSections) {
    DWORD currentMaxVA = 0;
    int pos = 0;

    IMAGE_SECTION_HEADER* tempHeader = firstSectionHeader;
    DWORD headerSize = sizeof(IMAGE_SECTION_HEADER);
    unsigned i;
    for(i = 0; i < numSections; i++) {
      tempHeader->Characteristics |= IMAGE_SCN_MEM_WRITE;
      tempHeader = MakePtr(IMAGE_SECTION_HEADER*, tempHeader, headerSize);
    }

    return;
}

// Most of the work is done in main()...
int main( int argc, char* argv[] ) {

  // Print out help message if needed.
  if (argc != 2) {
      printf("*** Code Crypt 0.0 by Tibbar@GovernmentSecurity.org ***\n");
      printf("***                                                 ***\n");
      printf("*** Usage: codecrypt filename.exe                   ***\n");
      printf("*** Will encrypt the codesection to avoid detection ***\n");
      printf("*** Disclaimer: This software is for educational    ***\n");
      printf("*** purposes only.  No responsibility is held or    ***\n");
      printf("*** accepted for misuse.                            ***\n");
      return 0;
    }
  
  // The file to pack is the 1st argument.
  LPCSTR fileName = argv[1];

  // Open the .exe file to be packed.
  HANDLE hFile = CreateFileA(
                             fileName,
			     GENERIC_WRITE | GENERIC_READ,
			     FILE_SHARE_READ | FILE_SHARE_WRITE,
			     NULL,
			     OPEN_EXISTING,
			     FILE_ATTRIBUTE_NORMAL,
			     NULL);

  // Did the file open work?
  if (hFile == INVALID_HANDLE_VALUE) {
    DWORD x = GetLastError();
    // No, get out.
    printf("Cannot open file '%s'...exiting!\n", argv[1]);
    char *buf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		  FORMAT_MESSAGE_FROM_SYSTEM |
		  FORMAT_MESSAGE_IGNORE_INSERTS,
		  NULL,x,0,
		  (LPTSTR) &buf,0,NULL);
    printf("Error: %d, Error Code:%d\n", buf, x);
    return 1;
  }

  // Get size of file. This is needed because we need to tell the
  // CreateFileMapping function to add memory to the end of the
  // memory-mapped file. We will later use this memory to add in the
  // unpacker stub and its required data.
  LARGE_INTEGER bigInt;
  BOOL r = GetFileSizeEx(hFile, &bigInt);
  if (!r) {
    printf("Cannot get file size.\n");
    return 1;
  }

  // Is the file too big to add in the unpacker stub?
  DWORD fileSize = bigInt.LowPart;
  if (fileSize + 0x2000 >= ULONG_MAX) {
    printf("The file is too big to pack.\n");
    return 1;
  }

  // Create a handle to a memory mapped file. Note that this does not
  // actually map the file, it just creates the handle that will actually be
  // mapped later. Note that extra space is allocated at the end to
  // the mapped file for the unpacker stub.
  HANDLE hFileMap = CreateFileMapping(hFile, 
				      NULL, 
				      PAGE_READWRITE,
				      bigInt.HighPart,
				      bigInt.LowPart + 0x2000 ,
				      NULL);
    
  // Did creating the file mapping handle work?
  if (hFileMap == NULL) {
    printf("Unable to create file mapping! Exiting...");
    return 1;
  }

  // Map the .exe file into memory. The map is set up so we can read and
  // write to it.
  LPVOID hMap = MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

  // Did mapping the file to memory work?
  if (hMap == NULL) {
    printf("Unable to map file into memory! Exiting...");
    return 1;
  }

  // Get the DOS header from the .exe file. This is the 1st thing in the
  // file.
  PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hMap;

  // This packer only works with .exe files. Make sure we have one.
  // The e_magic field tells us what sort of file this is.
  if (!(pDosHeader->e_magic == IMAGE_DOS_SIGNATURE)) {
    printf("This packer only works with .exe files.\n");
    return 1;
  }

  // Get the IMAGE_NT_HEADER. This has 3 fields, the signature (which should
  // be set 0x00004550 for valid PE files), the file header, and the
  // optional file header.
  PIMAGE_NT_HEADERS pNTHeader;
  // The e_lfanew field in the DOS header gives us the offset to the
  // IMAGE_NT_HEADER.
  pNTHeader = MakePtr(PIMAGE_NT_HEADERS, pDosHeader, pDosHeader->e_lfanew);

  // Check to make sure this is a PE file we are working with.
  if (!(pNTHeader->Signature == IMAGE_NT_SIGNATURE)) {
    printf("This packer only works with PE files.\n");
    printf("Signature for PE files is %x. Current signature is %x\n",
           IMAGE_NT_SIGNATURE, pNTHeader->Signature);
    return 1;
  }

  // The entire .exe image is stored in a HMODULE structure. Cast the memory
  // mapped file so it can be viewed as an HMODULE.
  HMODULE *hModule = (HMODULE *) hMap;

  // Grab a pointer to the IMAGE_OPTIONAL_HEADER structure, which contains
  // information about the size of the code and all data sections, where to
  // find the sections, etc.
  //
  // The IMAGE_OPTIONAL_HEADER structure is contained in the IMAGE_NT_HEADER
  // in the OptionalHeader field.
  //
  // Note that we are assuming that we are working with a 32bit executable.
  PIMAGE_OPTIONAL_HEADER32 optionHeader =
    (PIMAGE_OPTIONAL_HEADER32) &(pNTHeader->OptionalHeader);

  // Get the RVA (relative virtual address) of the first byte of code when
  // loaded into memory. To find the actual address of the code add the load
  // address (in this case the address of hModule) to the RVA.
  VOID* baseCodeRVA = (VOID*)optionHeader->BaseOfCode;

  // Get the combined size of all code sections.
  DWORD codeSize = optionHeader->SizeOfCode;

  // TCC does not seem to set the SizeOfCode field, so executables
  // compiled with TCC cannot be packed with this packer.
  if (codeSize < 1) {
    printf("SizeOfCode field is < 1. Cannot pack.\n");
    printf("The execuatble may have been compiled with TCC.\n");
    return 1;
  }

  // After the MAGE_NT_HEADERS is the section table. The section table is an
  // array of IMAGE_SECTION_HEADERs structures. An IMAGE_SECTION_HEADER
  // provides information about its associated section.
  //
  // Use the IMAGE_FIRST_SECTION macro defined in winnt.h to get the address
  // of the 1st IMAGE_SECTION_HEADER.
  IMAGE_SECTION_HEADER* firstSectionHeader = 
    IMAGE_FIRST_SECTION32(pNTHeader);

  // Get a pointer to the code block. The code block is found by adding the
  // base load address to the code RVA.
  PVOID codePtr = MakePtr(void *, hModule, baseCodeRVA);

  // Unprotect the code so we can encrypt it in place.
  DWORD oldProt = 0;
  BOOL diditwork = 
    VirtualProtect(codePtr, codeSize, PAGE_READWRITE, &oldProt);
    
  // Did the code unprotection work?
  if (!diditwork) {
    DWORD x = GetLastError();
    printf("Cannot unprotect code (error code %d).\n", x);
    return 0;
  }

  // Find the section containing the code. We have already
  // calculated the code relative virtual base address
  // (baseCodeRVA), so now we need to find the section whose base
  // RVA is the same as the RVA of the code.
  IMAGE_SECTION_HEADER* codeSection = 
    FindSection(firstSectionHeader, 
		pNTHeader->FileHeader.NumberOfSections, 
		(DWORD) baseCodeRVA);

  // Can we find the section with the code?
  if (codeSection == NULL) {
    printf("Cannot find code section.\n");
    return 0;
  }

  // Get the raw memory address (inside HMODULE) and size of the code.
  DWORD rawCodePosition = codeSection->PointerToRawData;
  DWORD virtualCodeSize = codeSection->Misc.VirtualSize;

  // Compute the actual, in-memory start and end address of the code
  // block.
  DWORD startpos = (DWORD)hModule + (DWORD)rawCodePosition;
  DWORD endpos = startpos + virtualCodeSize;
 
  // Encrypt the code by incrementing each byte in the code by 1.
  printf("Starting to encrypt code section...\n");
  DWORD currAddr;
  for (currAddr = startpos; currAddr < endpos; currAddr++) {
    (*((char *) currAddr))++;
  }
  printf("Done encrypting code section.\n");

  // Get the last section. We will be tacking the unpacking code
  // onto the end of the last section (if we have room).
  IMAGE_SECTION_HEADER* lastSection = 
    FindLastSection(firstSectionHeader, 
		    pNTHeader->FileHeader.NumberOfSections);

  char sectName[10];
  int i;
  for (i = 0; i < 8; i++) {
    sectName[i] = lastSection->Name[i];
  }
  sectName[8] = '\0';
  printf("Adding stub to section: name = %s, Raw size = %d, Raw Data ptr = %d\n", 
         sectName, 
	 lastSection->SizeOfRawData, 
	 lastSection->PointerToRawData);
    
  // We will be adding the unpacker stub to the last section in the
  // PE file. In that section, we will be adding the unpacker stub
  // code right after the existing data in the section.
  DWORD* storageSpot = (DWORD*)((DWORD)hModule + 
				(DWORD)lastSection->PointerToRawData +
				(DWORD)lastSection->SizeOfRawData);

  // Get the unpacking code and store it in a contiguous chunk of memory.
  char *decryptRoutine = CopyFunction(&DecryptAndStart);
  DWORD decryptSize = GetFunctionSize(&DecryptAndStart);
  if (decryptSize > 0x2000) {
    printf("Decryption routine too large.\n");
    return 1;
  }
  printf("Size of stub = %d\n", decryptSize);

  // Now start putting data into memory as it will be laid out in
  // the packed executable.

  // Track the # of pieces of initialization data added.
  int numInitData = 0;

  // Store the RVA of the packed code furthest away from the stub.
  *(storageSpot + numInitData++) = (DWORD)baseCodeRVA;
    
  // Store original code entry RVA at storagespot next closest to
  // the stub.
  *(storageSpot + numInitData++) = (DWORD)optionHeader->AddressOfEntryPoint;

  // Store the image base (used with the code base RVA to figure out
  // where the code to unpack is in memory in the unpacker stub) at
  // the next closest spot.
  *(storageSpot + numInitData++) = (DWORD)optionHeader->ImageBase;

  // Store size of code (in bytes) to unpack at the spot closest to
  // the stub.
  *(storageSpot + numInitData++) = virtualCodeSize;

  // Add our decryption routine to storageSpot + (# of data DWORDS
  // added). The +(# of data DWORDS written) is there so that the
  // unpacker stub data added above is not overwritten by the stub.
  //
  // Note that storageSpot is a pointer to a DWORD, so adding 1 to
  // storageSpot advances by 1 DWORD, not 1 BYTE.
  memcpy(storageSpot + numInitData, decryptRoutine, decryptSize);
    
  // Now update the header for the last section (i.e. one with
  // highest RVA) and extend this by size needed and set it to
  // executable.

  // Add the size of the unpacker stub and its initialization data
  // to all of the size tracking fields in the IMAGE_OPTIONAL_HEADER.
  optionHeader->SizeOfImage += 0x2000;
  optionHeader->SizeOfCode += 0x2000;
  optionHeader->SizeOfInitializedData += 0x2000;

  // Set the new initial entry point to the unpacker stub.
  optionHeader->AddressOfEntryPoint = 
    // Start in last section...
    (DWORD)lastSection->VirtualAddress + 
    // At the end of its original data...
    (DWORD)lastSection->SizeOfRawData + 
    // Past the unpacker stub initialization data (4 DWORDs).
    (sizeof(DWORD))*numInitData;

  // Add the size of the unpacker stub and its initialization data
  // to all of the size tracking fields the last section header.
  lastSection->Misc.VirtualSize += 0x2000;
  lastSection->SizeOfRawData += 0x2000;
  lastSection->Characteristics = IMAGE_SCN_MEM_WRITE|
    IMAGE_SCN_MEM_READ|
    IMAGE_SCN_MEM_EXECUTE|
    IMAGE_SCN_CNT_UNINITIALIZED_DATA |
    IMAGE_SCN_CNT_INITIALIZED_DATA|
    IMAGE_SCN_CNT_CODE;//0xE00000E0;

  // Finally we need to set the main code section as writable for
  // our stub to decrypt it. We will do this by making ALL sections
  // writable.
  MakeAllSectionsWritable(firstSectionHeader, 
			  pNTHeader->FileHeader.NumberOfSections); 

  // Close the packed file.
  CloseHandle(hFile);
}
