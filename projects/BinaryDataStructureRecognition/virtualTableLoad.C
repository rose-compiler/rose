#include "rose.h"

#include "astStructRecognition.h"

using namespace std;
using namespace Rose;
using namespace SageInterface;
using namespace SageBuilderAsm;

VirtualFunction::VirtualFunction (string n, size_t vFunctionAddress )
   {
     name = n; // "virtual_function";
     virtualFunctionTableEntry = 0;
     virtualFunctionAddress = vFunctionAddress;
   }

// Constructor
VirtualFunctionTable::VirtualFunctionTable ( SgProject* project, SgAsmElfSection* virtualFunctionTable, size_t vTableAddress )
   : virtualFunctionTablePosition(vTableAddress)
   {
     printf ("Building virtual function table for position %p \n",(void*)virtualFunctionTablePosition);

  // Get the boundaries of the section where the virtual function tables are located.
     ROSE_ASSERT(virtualFunctionTable != NULL);
     size_t section_address_base  = virtualFunctionTable->get_mapped_preferred_rva();

  // Compute the offset for the input virtual function table into the section.
     size_t offset                = vTableAddress - section_address_base;

  // This is the upper bound on the size of the table (stay within the section)
     size_t size                  = virtualFunctionTable->get_size();

     ROSE_ASSERT(offset < size);

     printf ("In printOutVirtualFunctionTableInformation(%p): offset = %p (size = %p) \n",(void*)vTableAddress,(void*)offset,(void*)size);

     printf ("START -- Virtual function table: \n");

  // Get the architecture specifici information required to the raw data in the section.
     SgAsmGenericHeader *hdr = virtualFunctionTable->get_header();
     ByteOrder::Endianness sex = hdr->get_sex();
     size_t wordsize = hdr->get_word_size();
     size_t virtualTableSize = (size-offset) / wordsize;
     printf ("size = %zu offset = %zu wordsize = %zu virtualTableSize = %zu \n",size,offset,wordsize,virtualTableSize);

     for (size_t i=0; i < virtualTableSize; i++)
        {
          uint64_t functionPointer; /*type must support 64-bit specimen pointers in a 32-bit ROSE library*/
       // printf ("i = %zu \n",i);
          switch (wordsize)
             {
               case 4:
                  {
                    uint32_t ptr_disk;
                 // virtualFunctionTable->read_content_local(offset+i*wordsize, &ptr_disk, sizeof ptr_disk);
                 // virtualFunctionTable->read_content_local(i*wordsize, &ptr_disk, sizeof ptr_disk);
                    virtualFunctionTable->read_content_local(offset+i*wordsize, &ptr_disk, sizeof ptr_disk);
                    functionPointer = ByteOrder::disk_to_host(sex, ptr_disk);
                    break;
                  }

               case 8:
                  {
                    uint64_t ptr_disk;
                 // virtualFunctionTable->read_content_local(offset+i*wordsize, &ptr_disk, sizeof ptr_disk);
                 // virtualFunctionTable->read_content_local(i*wordsize, &ptr_disk, sizeof ptr_disk);
                    virtualFunctionTable->read_content_local(offset+i*wordsize, &ptr_disk, sizeof ptr_disk);
                    functionPointer = ByteOrder::disk_to_host(sex, ptr_disk);
                    break;
                  }

               default:
                    ROSE_ASSERT(!"unsupported word size");
             }

          string sectionName;
          SgAsmElfSection* asmElfSection = getSection(project,functionPointer);
          if (asmElfSection != NULL)
             {
               sectionName = asmElfSection->get_name()->get_string();
             }
            else
             {
               sectionName = "NOT CONTAINED IN EXECUTABLE";

            // At the first sign of a pointer that is not in the executable then quit, this should be the end of the table.
            // Alternatively it appears that the end of the table has a code  0x4231 (for the first table).
               break;
             }

          printf ("Pointer 0x%"PRIx64" is to data in the %s section \n",functionPointer,sectionName.c_str());

          string name = "virtual_function_" + StringUtility::numberToString(i);

          VirtualFunction* virtualFunction = new VirtualFunction(name,functionPointer);
          ROSE_ASSERT(virtualFunction != NULL);
          virtualFunctionList.push_back(virtualFunction);
        }

     printf ("END -- Virtual function table: \n");

#if 0
     printf ("Exiting in VirtualFunctionTable constructor \n");
     ROSE_ASSERT(false);
#endif
   }

bool
VirtualFunctionTable::isVirtualMemberFunction ( SgAsmFunctionDeclaration* asmFunction )
   {
     ROSE_ASSERT(virtualFunctionList.empty() == false);

     bool result = false;

  // For now loop over the whole table.
     for (size_t i = 0; i < virtualFunctionList.size(); i++)
        {
          if (asmFunction->get_address() == virtualFunctionList[i]->virtualFunctionAddress)
             {
               result = true;
             }
        }

     return result;
   }

// Constructor
VirtualTableSection::VirtualTableSection ( SgAsmElfSection* vTableSection, SgProject* p )
   : virtualFunctionTable(vTableSection), project(p)
   {
     printf ("Building virtual function table section for position %p textSection = %p \n",(void*)virtualFunctionTable,project);
   }

#if 0
bool
VirtualFunctionSection::isVirtualMemberFunction ( SgAsmFunctionDeclaration* asmFunction )
   {
     ROSE_ASSERT(virtualFunctionTableList.empty() == false);

     bool result = false;

  // For now loop over the whole table.
     for (size_t i = 0; i < virtualFunctionTableList.size(); i++)
        {
           if (virtualFunctionTableList[i]->isVirtualMemberFunction(asmFunction) == true)
             {
               result = true;
             }
        }

     return result;
   }
#endif

bool
VirtualTableSection::tableExists ( VirtualFunctionTableAddress vTable )
   {
     bool returnResult = false;
     for (size_t i = 0; i < virtualFunctionTableList.size(); i++)
        {
       // Check if this is a virtual function table that has already been built
          ROSE_ASSERT(virtualFunctionTableList[i] != NULL);
          if (virtualFunctionTableList[i]->virtualFunctionTablePosition == vTable)
             {
               returnResult = true;
             }
        }

     return returnResult;
   }

void
VirtualTableSection::buildVirtualFunctionTable ( VirtualFunctionTableAddress vTable )
   {
  // This should be called only once per virtual function table.

   }


#if 0
void
VirtualTableSection::printOutVirtualFunctionTableInformation ( VirtualFunctionTableAddress vTableAddress )
   {
     ROSE_ASSERT(virtualFunctionTable != NULL);
     size_t section_address_base  = virtualFunctionTable->get_mapped_preferred_rva();
     size_t offset                = vTableAddress - section_address_base;
     size_t size                  = virtualFunctionTable->get_size();

     ROSE_ASSERT(offset < size);

     printf ("In printOutVirtualFunctionTableInformation(%p): offset = %p (size = %p) \n",(void*)vTableAddress,(void*)offset,(void*)size);

     printf ("START -- Virtual function table: \n");

     SgAsmGenericHeader *hdr = virtualFunctionTable->get_header();
     SgAsmGenericFormat::ByteOrder sex = hdr->get_sex();
     size_t wordsize = hdr->get_word_size();
     size_t virtualTableSize = (size-offset) / wordsize;
     printf ("size = %zu offset = %zu wordsize = %zu virtualTableSize = %zu \n",size,offset,wordsize,virtualTableSize);
     for (size_t i=0; i < virtualTableSize; i++)
        {
          uint64_t functionPointer; /*type must support 64-bit specimen pointers in a 32-bit ROSE library*/
       // printf ("i = %zu \n",i);
          switch (wordsize)
             {
               case 4:
                  {
                    uint32_t ptr_disk;
                 // virtualFunctionTable->read_content_local(offset+i*wordsize, &ptr_disk, sizeof ptr_disk);
                 // virtualFunctionTable->read_content_local(i*wordsize, &ptr_disk, sizeof ptr_disk);
                    virtualFunctionTable->read_content_local(offset+i*wordsize, &ptr_disk, sizeof ptr_disk);
                    functionPointer = SgAsmExecutableFileFormat::disk_to_host(sex, ptr_disk);
                    break;
                  }
               case 8:
                  {
                    uint64_t ptr_disk;
                 // virtualFunctionTable->read_content_local(offset+i*wordsize, &ptr_disk, sizeof ptr_disk);
                 // virtualFunctionTable->read_content_local(i*wordsize, &ptr_disk, sizeof ptr_disk);
                    virtualFunctionTable->read_content_local(offset+i*wordsize, &ptr_disk, sizeof ptr_disk);
                    functionPointer = SgAsmExecutableFileFormat::disk_to_host(sex, ptr_disk);
                    break;
                  }
               default:
                    ROSE_ASSERT(!"unsupported word size");
             }

          string sectionName;
          SgAsmElfSection* asmElfSection = getSection(project,functionPointer);
          if (asmElfSection != NULL)
             {
               sectionName = asmElfSection->get_name()->get_string();
             }
            else
             {
               sectionName = "NOT CONTAINED IN EXECUTABLE";

            // At the first sign of a pointer that is not in the executable then quit, this should be the end of the table.
            // Alternatively it appears that the end of the table has a code  0x4231 (for the first table).
               break;
             }

          printf ("Pointer 0x%"PRIx64" is to data in the %s section \n",functionPointer,sectionName.c_str());
        }

     printf ("END -- Virtual function table: \n");
   }
#endif


// Constructor
VirtualTableLoad::VirtualTableLoad( SgAsmInstruction* instruction )
   {
  // Constructor for VirtualTableLoad takes a SgAsmInstruction, and initializes the address of the virual table.
  // SgAsmX86Instruction* x86Instruction = isSgAsmX86Instruction(instruction);
     ROSE_ASSERT(instruction != NULL);
     ROSE_ASSERT(instruction->nOperands() == 2);
     ROSE_ASSERT(instruction->operand(0) != NULL);
     ROSE_ASSERT(instruction->operand(1) != NULL);

  // Initialize the virtualTableAddress
     virtualTableAddress = isSgAsmValueExpression(instruction->operand(1));
     ROSE_ASSERT(virtualTableAddress != NULL);
   }


SgAsmElfSection*
getVirtualTableSection ( SgProject* project )
   {
     return getSection(project,".rodata");
   }


pair<size_t,size_t>
getVirtualTableSectionBoundaries ( SgProject* project )
   {
     SgAsmElfSection* asmElfSection = getVirtualTableSection(project);
     ROSE_ASSERT(asmElfSection != NULL);

     size_t readOnlyDataSectionAddressBase  = asmElfSection->get_mapped_preferred_rva();
     unsigned long size                     = asmElfSection->get_size();
     size_t readOnlyDataSectionAddressBound = readOnlyDataSectionAddressBase + size;
     ROSE_ASSERT(asmElfSection->get_size() == asmElfSection->get_mapped_size());

  // printf ("asmElfSection = %s  address_base = %p size = %p address_bound = %p \n",asmElfSection->get_name()->c_str(),(void*)address_base,(void*)size,(void*)address_bound);

     return pair<size_t,size_t>(readOnlyDataSectionAddressBase,readOnlyDataSectionAddressBound);
   }


void
detectVirtualTableLoad( SgProject* project )
   {
  // This function detects the case of a virtual table address load at "address":
  //    mov edx, <address>
  // In the process of detecting the virtual table loads is adds attributes 
  // to the section where the virtual tables are put and builds a list of 
  // virtual tables.

  // This is a pointer to the ".rodata" section.
  // What happens if we process multiple DLL's where each has it's own ".rodata" section?
  // This case would suggest that we really do need to fram this as a top-down and bottom-up traversal.
     SgAsmElfSection* vTableSection = getVirtualTableSection(project);
     ROSE_ASSERT(vTableSection != NULL);

#if 0
  // This will return NULL (or generate NULL internally).
     SgAsmInterpretation* asmInterpretation = getAsmInterpretation(vTableSection);
     ROSE_ASSERT(asmInterpretation != NULL);
#endif

  // This is the base and bound of the mapped memory representing the ".rodata" section.
  // NOTE: Now that we compute the ".rodata" section, we should refactor this to be a 
  // way to get the bounding addresses of the mapped memory section (and use the 
  // vTableSection as a function argument).
     pair<size_t,size_t> readOnlyDataSectionAddressBoundary = getVirtualTableSectionBoundaries(project);
     printf ("Section .rodata boundary: address_base = %p address_bound = %p \n",(void*)readOnlyDataSectionAddressBoundary.first,(void*)readOnlyDataSectionAddressBoundary.second);

  // SgAsmElfSection* textSection = getSection(project,".text");
  // ROSE_ASSERT(textSection != NULL);

  // Pass the project so that we can provide for more analysis capability within the VirtualTableSection abstraction.
     VirtualTableSection* virtualTableSectionAttribute = new VirtualTableSection(vTableSection,project);
     ROSE_ASSERT(virtualTableSectionAttribute != NULL);

  // Add it to the AST (so it can be found later in another pass over the AST)
     vTableSection->addNewAttribute("VirtualFunctionTableSectionAttribute",virtualTableSectionAttribute);

  // Alternative approach using a example and searching the AST for a match against the example.
  // printf ("Alternative approach using a example and searching the AST for a match against the example. \n");

  // General template to use in matching against project.
  // Build a specific instruction to match a specific address (in .rodata, where the vitual function tables are located)
#ifdef USE_NEW_ISA_INDEPENDENT_REGISTER_HANDLING
     SgAsmInstruction* target = SageBuilderAsm::buildX86Instruction(x86_mov,
                   SageBuilderAsm::buildAsmRegisterReferenceExpression(x86_regclass_gpr,SgAsmRegisterReferenceExpression::e_edx),
                // Note that the address value is ignored in our "equivalenceTest()" function.
                   SageBuilderAsm::buildValueX86DWord(0x0));
     printf ("Target instruction = %s \n",unparseInstructionWithAddress(target).c_str());
#else
  // DQ (9/2/2013): This allows us to get the existing code compiled and then move to update the code seperately.
     printf ("This code needs to be updated to handle the new register handling (ISA independence) \n");
     ROSE_ASSERT(false);

     SgAsmInstruction* target = NULL;
#endif

     ROSE_ASSERT(target != NULL);

  // General function to find matching target AST in larger AST.
     std::vector<SgNode*> matchingSubtreeList = find ( project, target, equivalenceTest );

  // Use a set so that after processing all instructions we have the collection of unique entries.
     set<size_t> setOfVirtualFunctionTables;

     for (size_t i = 0; i < matchingSubtreeList.size(); i++)
        {
       // Get the SgAsmInstruction
          SgAsmInstruction* instruction = isSgAsmInstruction(matchingSubtreeList[i]);
          ROSE_ASSERT(instruction != NULL);
          ROSE_ASSERT(instruction != target);

          printf ("Processing instruction %s \n",unparseInstructionWithAddress(target).c_str());

       // Build an attribute (on the heap)
       // AstAttribute* newAttribute = new VirtualTableLoad(instruction);
          VirtualTableLoad* newAttribute = new VirtualTableLoad(instruction);
          ROSE_ASSERT(newAttribute != NULL);

       // Accumulate the set of virtual function table addresses.
          size_t virtualFunctionTableAddress = get_value(newAttribute->virtualTableAddress);
          printf ("virtualFunctionTableAddress = %p \n",(void*)virtualFunctionTableAddress);
          printf ("readOnlyDataSectionAddressBoundary.first = %p second = %p \n",(void*)readOnlyDataSectionAddressBoundary.first,(void*)readOnlyDataSectionAddressBoundary.second);

       // This absolutely needs the extra "()" around both of the inner predicates.
          bool inRange = ((virtualFunctionTableAddress > readOnlyDataSectionAddressBoundary.first) && 
                          (virtualFunctionTableAddress < readOnlyDataSectionAddressBoundary.second));

          printf ("inRange = %s \n",inRange ? "true" : "false");

       // If this is ever false then we will need to introduce a conditional to only handle the true cases.
       // ROSE_ASSERT(inRange == true); // can be false for GNU g++ 4.1.x compilers
          if (inRange == true)
             {
            // Add it to the AST (so it can be found later in another pass over the AST)
               instruction->addNewAttribute("VirtualTableLoad",newAttribute);

            // Collect the virtual table addresses into the set.
               setOfVirtualFunctionTables.insert(virtualFunctionTableAddress);
             }
            else
             {
            // If it is not used then delete the analysis attribute
               delete newAttribute;
               newAttribute = NULL;
             }

#if 0
          string comment = "Virtual Table Load: " + get_valueString(newAttribute->virtualTableAddress);

          printf ("Attaching comment = %s \n",comment.c_str());
          addComment(instruction,comment);
#endif
        }

     printf ("Number of unique virtual function tables = %zu \n",setOfVirtualFunctionTables.size());

     int counter = 0;
     for (set<size_t>::iterator i = setOfVirtualFunctionTables.begin(); i != setOfVirtualFunctionTables.end(); i++)
        {
       // This is the address of each virtual function table.
          size_t virtualFunctionTableAddress = *i;

       // These are the separate virtual function tables objects used to summarize analysis.
          ROSE_ASSERT(virtualTableSectionAttribute->tableExists(virtualFunctionTableAddress) == false);
          if (virtualTableSectionAttribute->tableExists(virtualFunctionTableAddress) == false)
             {
               VirtualFunctionTable* vt = new VirtualFunctionTable(project,vTableSection,virtualFunctionTableAddress);
               ROSE_ASSERT(vt != NULL);

               vt->name = "Class_From_vTable_" + StringUtility::numberToString(counter);

               printf ("vt->name = %s \n",vt->name.c_str());

            // This might be depricated in favor of putting the virtual function table list into globalScopeAttribute. 
               virtualTableSectionAttribute->virtualFunctionTableList.push_back(vt);

            // This is the more appropriate place for the list of virtual function tables.
               ROSE_ASSERT(globalScopeAttribute != NULL);
               globalScopeAttribute->virtualFunctionTableList.push_back(vt);

               for (size_t i = 0; i < matchingSubtreeList.size(); i++)
                  {
                 // Get the SgAsmInstruction
                    SgAsmInstruction* instruction = isSgAsmInstruction(matchingSubtreeList[i]);
                    ROSE_ASSERT(instruction != NULL);

                    VirtualTableLoad* attribute = dynamic_cast<VirtualTableLoad*>(instruction->getAttribute("VirtualTableLoad"));

                 // This can be false for GNU g++ 4.1.x compilers
                 // ROSE_ASSERT(attribute != NULL);

                    if (attribute != NULL)
                       {
                         size_t local_virtualFunctionTableAddress = get_value(attribute->virtualTableAddress);
                         if (local_virtualFunctionTableAddress == virtualFunctionTableAddress)
                            {
                              attribute->associatedVirtualTable = vt;
                            }
                           else
                            {
                              attribute->associatedVirtualTable = NULL;
                            }
                       }
                  }

               counter++;
             }

       // These are the separate virtual function tables.
       // virtualTableSectionAttribute->printOutVirtualFunctionTableInformation(virtualFunctionTableAddress);
        }

     printf ("Adding comments to the instructions \n");

  // Add comments
     for (size_t i = 0; i < matchingSubtreeList.size(); i++)
        {
          printf ("matchingSubtreeList -- i = %zu \n",i);

       // Get the SgAsmInstruction
          SgAsmInstruction* instruction = isSgAsmInstruction(matchingSubtreeList[i]);
          ROSE_ASSERT(instruction != NULL);

          printf ("Getting the VirtualTableLoad attribute \n");

          VirtualTableLoad* attribute = dynamic_cast<VirtualTableLoad*>(instruction->getAttribute("VirtualTableLoad"));

       // This can be false for GNU g++ 4.1.x compilers
       // ROSE_ASSERT(attribute != NULL);

          if (attribute != NULL)
             {
            // This can be false for GNU g++ 4.1.x compilers
            // ROSE_ASSERT(attribute->associatedVirtualTable != NULL);

               if (attribute->associatedVirtualTable != NULL)
                  {
                    printf ("Building a comment \n");
                    printf ("attribute->virtualTableAddress = %p \n",(void*)attribute->virtualTableAddress);
                    printf ("attribute->associatedVirtualTable->name = %s \n",attribute->associatedVirtualTable->name.c_str());

                    string comment = "Virtual Table Load: name = " + attribute->associatedVirtualTable->name + " " + get_valueString(attribute->virtualTableAddress);

                    printf ("Attaching comment = %s \n",comment.c_str());
                    addComment(instruction,comment);
                  }
             }
        }

     printf ("Leaving detectVirtualTableLoad() \n");
   }
