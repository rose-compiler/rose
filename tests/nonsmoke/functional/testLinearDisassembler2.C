#include <rose.h>
#include <rosePublicConfig.h>

#include <Rose/BinaryAnalysis/Disassembler.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherM68k.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/TraceSemantics.h>

#include <map>
#include <Sawyer/Assert.h>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Message.h>
#include <string>
#include <cinttypes> /* for uintptr_t and PRIxxx format specifiers*/

using namespace Rose;
using namespace Rose::BinaryAnalysis::InstructionSemantics;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

// DQ (12/16/2021): Added to support use of STL types.
using namespace std;

Sawyer::Message::Facility mlog;

namespace // anonymous
{
#if 0
// Round X up to the next multiple of ALIGNMENT
rose_addr_t
alignUp(rose_addr_t x, rose_addr_t alignment)
{
    return alignment>1 ? ((x+alignment-1)/alignment)*alignment : x;
}
#endif /* 0 */

// Convenient struct to hold settings from the command-line all in one place.
struct Settings {
    rose_addr_t startVa;
    rose_addr_t alignment;
    bool runSemantics;
    Settings(): startVa(0), alignment(1), runSemantics(false) {}
};

// Describe and parse the command-line

vector<string>
parseCommandLine(int argc, char *argv[], P2::Engine &engine, Settings &settings)
{
    using namespace Sawyer::CommandLine;

    std::string purpose = "disassembles files one address at a time";
    std::string description =
        "This program is a very simple disassembler that tries to disassemble in instruction at each executable "
        "address, one instruction after the next.";

    // The parser is the same as that created by Engine::commandLineParser except we don't need any partitioning switches since
    // this tool doesn't partition.
    Parser parser;
    parser
        .purpose(purpose)
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .doc("Synopsis",
             "@prop{programName} [@v{switches}] @v{specimen_names}")
        .doc("Description", description)
        .doc("Specimens", engine.specimenNameDocumentation())
        .with(engine.engineSwitches())
        .with(engine.loaderSwitches())
        .with(engine.disassemblerSwitches());

    SwitchGroup switches("Tool-specific switches");
    switches.name("tool");

    switches.insert(Switch("start")
                    .argument("virtual-address", nonNegativeIntegerParser(settings.startVa))
                    .doc("Address at which disassembly will start.  The default is to start at the lowest mapped "
                         "address."));
    switches.insert(Switch("alignment")
                    .argument("align", nonNegativeIntegerParser(settings.alignment))
                    .doc("Alignment for instructions.  The default is 1 (no alignment).  Values larger than one will "
                         "cause each candidate address to be rounded up to the next multiple of @v{align}.  If this "
                         "rounding up causes addresses after a valid instruction to be skipped then a warning is printed. "
                         "No warning is printed if the alignment skips addresses after a disassembly failure."));
    switches.insert(Switch("semantics")
                    .intrinsicValue(true, settings.runSemantics)
                    .doc("Run semantics for each basic block. This is only useful to debug instruction semantics."));
    switches.insert(Switch("no-semantics")
                    .key("semantics")
                    .intrinsicValue(false, settings.runSemantics)
                    .hidden(true));

    return parser.with(switches).parse(argc, argv).apply().unreachedArgs();
}


#if 0
void f() {

    uint8_t buf[1024];
    while (AddressInterval where = map->atOrAfter(va).require(MemoryMap::EXECUTABLE).limit(sizeof buf).read(buf)) {
        ....
        va += where.size();
    }
}
#endif


template <class SageAsmCilNode>
SageAsmCilNode*
parseAsmCilNode(uint8_t* buf, size_t& index, uint8_t expected)
{
    SageAsmCilNode* res = new SageAsmCilNode;
    ASSERT_not_null(res);

    ::mlog[INFO] << "Creating " << res->class_name() << " node at " << index
                 << std::endl;

    res->parse(buf, index, expected);
    return res;
}

}


// Data structures to support .net metadata layout within the .text section.



enum Table_kinds
  {
 // Values of enum fields are the bit positions in the valid array of valid tables (for each table).
    e_unknown_table_kind     = 0xFF,
    e_error_table_kind       = 0xFE,
    e_assembly               = 0x20,
    e_assemblyOS             = 0x22,
    e_assemblyProcessor      = 0x21,
    e_assemblyRef            = 0x23,
    e_assemblyRefOS          = 0x25,
    e_assemblyRefProcessor   = 0x24,
    e_classLayout            = 0x0F,
    e_constant               = 0x0B,
    e_customAttribute        = 0x0C,
    e_declSecurity           = 0x0E,
    e_eventMap               = 0x12,
    e_event                  = 0x14,
    e_exportedType           = 0x27,
    e_field                  = 0x04,
    e_fieldLayout            = 0x10,
    e_fieldMarshal           = 0x0D,
    e_fieldRVA               = 0x1D,
    e_file                   = 0x26,
    e_genericParam           = 0x2A,
    e_genericParamConstraint = 0x2C,
    e_implMap                = 0x1C,
    e_interfaceImpl          = 0x09,
    e_manifestResource       = 0x28,
    e_memberRef              = 0x0A,
    e_methodDef              = 0x06,
    e_methodImpl             = 0x19,
    e_methodSemantics        = 0x18,
    e_methodSpec             = 0x2B,
    e_module                 = 0x00,
    e_moduleRef              = 0x1A,
    e_nestedClass            = 0x29,
    e_param                  = 0x08,
    e_property               = 0x17,
    e_propertyMap            = 0x15,
    e_standAloneSig          = 0x11,
    e_typeDef                = 0x02,
    e_typeRef                = 0x01,
    e_typeSpec               = 0x1B,
    e_last_table_kind        = 0xFD
  };

string table_kind_to_string ( enum Table_kinds e )
   {
     string s;
     switch (e)
       {
         case e_unknown_table_kind:     s = "e_unknown_table_kind"; break;
         case e_error_table_kind:       s = "e_error_table_kind"; break;
         case e_assembly:               s = "e_assembly"; break;
         case e_assemblyProcessor:      s = "e_assemblyProcessor"; break;
         case e_assemblyOS:             s = "e_assemblyOS"; break;
         case e_assemblyRef:            s = "e_assemblyRef"; break;
         case e_assemblyRefProcessor:   s = "e_assemblyRefProcessor"; break;
         case e_assemblyRefOS:          s = "e_assemblyRefOS"; break;
         case e_classLayout:            s = "e_classLayout"; break;
         case e_constant:               s = "e_constant"; break;
         case e_customAttribute:        s = "e_customAttribute"; break;
         case e_declSecurity:           s = "e_declSecurity"; break;
         case e_eventMap:               s = "e_eventMap"; break;
         case e_event:                  s = "e_event"; break;
         case e_exportedType:           s = "e_exportedType"; break;
         case e_field:                  s = "e_field"; break;
         case e_fieldLayout:            s = "e_fieldLayout"; break;
         case e_fieldMarshal:           s = "e_fieldMarshal"; break;
         case e_fieldRVA:               s = "e_fieldRVA"; break;
         case e_file:                   s = "e_file"; break;
         case e_genericParam:           s = "e_genericParam"; break;
         case e_genericParamConstraint: s = "e_genericParamConstraint"; break;
         case e_implMap:                s = "e_implMap"; break;
         case e_interfaceImpl:          s = "e_interfaceImpl"; break;
         case e_manifestResource:       s = "e_manifestResource"; break;
         case e_memberRef:              s = "e_memberRef"; break;
         case e_methodDef:              s = "e_methodDef"; break;
         case e_methodImpl:             s = "e_methodImpl"; break;
         case e_methodSemantics:        s = "e_methodSemantics"; break;
         case e_methodSpec:             s = "e_methodSpec"; break;
         case e_module:                 s = "e_module"; break;
         case e_moduleRef:              s = "e_moduleRef"; break;
         case e_nestedClass:            s = "e_nestedClass"; break;
         case e_param:                  s = "e_param"; break;
         case e_property:               s = "e_property"; break;
         case e_propertyMap:            s = "e_propertyMap"; break;
         case e_standAloneSig:          s = "e_standAloneSig"; break;
         case e_typeDef:                s = "e_typeDef"; break;
         case e_typeRef:                s = "e_typeRef"; break;
         case e_typeSpec:               s = "e_typeSpec"; break;
         case e_last_table_kind:        s = "e_last_table_kind"; break;

         default:
            {
              printf ("In table_kind_to_string(): default reached: e = %d \n",e);
            }
       }

     return s;
   }

uint8_t read8bitValue (uint8_t* buf, size_t & index)
   {
  // This function abstracts the details of reading 2 byte values from the disk image.
     uint8_t value = ByteOrder::le_to_host(*((uint8_t*)(buf+index)));
     index += 1;

     return value;
   }


// DQ (12/18/2021): Temporary global RVA list for each method.
vector<uint32_t> rvaList;

// This is the header for the top of the #~ stream.
class MetadataTable
   {
     public:
          uint32_t reserved_always_zero;
          uint8_t majorVersion;
          uint8_t minorVersion;
          uint8_t heapSizes;
          uint8_t reserved_always_one;

       // This is a bit vector telling us which tables are available.
          uint64_t valid;
          uint64_t sorted;

       // This is the array of rows for each of the tables that are valid (as indicated by the valid bit vector).
          std::vector<uint32_t> rows;

       // The tables are arranged as an "sequence of physical tables".
       // Note that metadata tables are described on page 209 of the standard.
       // There is a sequence of tables, but I'm not clear which tables come first, second, third, etc.
       // Assembly_0x20 assemblyTable;
       // std::vector<uint32_t> tables;

          bool uses4byteIndexing;

       // List of the valid tables (in order given by the bits in the valid data member).
          vector<enum Table_kinds> table_list;

          vector<SgAsmCilAssembly*>               assembly_table;
          vector<SgAsmCilAssemblyOS*>             assemblyOS_table;
          vector<SgAsmCilAssemblyProcessor*>      assemblyProcessor_table;
          vector<SgAsmCilAssemblyRef*>            assemblyRef_table;
          vector<SgAsmCilAssemblyRefOS*>          assemblyRefOS_table;
          vector<SgAsmCilAssemblyRefProcessor*>   assemblyRefProcessor_table;
          vector<SgAsmCilClassLayout*>            classLayout_table;
          vector<SgAsmCilConstant*>               constant_table;
          vector<SgAsmCilCustomAttribute*>        customAttribute_table;
          vector<SgAsmCilDeclSecurity*>           declSecurity_table;
          vector<SgAsmCilEventMap*>               eventMap_table;
          vector<SgAsmCilEvent*>                  event_table;
          vector<SgAsmCilExportedType*>           exportedType_table;
          vector<SgAsmCilField*>                  field_table;
          vector<SgAsmCilFieldLayout*>            fieldLayout_table;
          vector<SgAsmCilFieldMarshal*>           fieldMarshal_table;
          vector<SgAsmCilFieldRVA*>               fieldRVA_table;
          vector<SgAsmCilFile*>                   file_table;
          vector<SgAsmCilGenericParam*>           genericParam_table;
          vector<SgAsmCilGenericParamConstraint*> genericParamConstraint_table;
          vector<SgAsmCilImplMap*>                implMap_table;
          vector<SgAsmCilInterfaceImpl*>          interfaceImpl_table;
          vector<SgAsmCilManifestResource*>       manifestResource_table;
          vector<SgAsmCilMemberRef*>              memberRef_table;
          vector<SgAsmCilMethodDef*>              methodDef_table;
          vector<SgAsmCilMethodImpl*>             methodImpl_table;
          vector<SgAsmCilMethodSemantics*>        methodSemantics_table;
          vector<SgAsmCilMethodSpec*>             methodSpec_table;
          vector<SgAsmCilModule*>                 module_table;
          vector<SgAsmCilModuleRef*>              moduleRef_table;
          vector<SgAsmCilNestedClass*>            nestedClass_table;
          vector<SgAsmCilParam*>                  param_table;
          vector<SgAsmCilProperty*>               property_table;
          vector<SgAsmCilPropertyMap*>            propertyMap_table;
          vector<SgAsmCilStandAloneSig*>          standAloneSig_table;
          vector<SgAsmCilTypeDef*>                typeDef_table;
          vector<SgAsmCilTypeRef*>                typeRef_table;
          vector<SgAsmCilTypeSpec*>               typeSpec_table;

     public:
          MetadataTable(uint8_t* buf, size_t & index)
             {
               printf ("In MetadataTable constructor: buf = 0x%" PRIxPTR " index = %zu \n",(uintptr_t)buf,index);

            // The default is false, but set to true if any of the bits in the heapSizes data member are set (true).
               uses4byteIndexing = false;

               uint32_t tmp_offset_value = ByteOrder::le_to_host(*((uint32_t*)(buf+index)));

               reserved_always_zero = tmp_offset_value;

               printf ("reserved_always_zero = %u \n",reserved_always_zero);

               ROSE_ASSERT(reserved_always_zero == 0);

               index += 4;

               uint8_t tmp_majorVersion_value = ByteOrder::le_to_host(*((uint8_t*)(buf+index)));
               majorVersion = tmp_majorVersion_value;

               printf ("index = %zu majorVersion = %u \n",index,majorVersion);

               index += 1;

               uint8_t tmp_minorVersion_value = ByteOrder::le_to_host(*((uint8_t*)(buf+index)));
               majorVersion = tmp_minorVersion_value;

               printf ("index = %zu minorVersion = %u \n",index,minorVersion);

               index += 1;

               uint8_t tmp_heapsizes_value = ByteOrder::le_to_host(*((uint8_t*)(buf+index)));
               heapSizes = tmp_heapsizes_value;

               printf ("index = %zu heapSizes = %u \n",index,heapSizes);

               index += 1;

            // The default is false, but set to true if any of the bits in the heapSizes data member are set (true).
               if (heapSizes > 0)
                  {
                    uses4byteIndexing = true;
                  }

               printf ("uses4byteIndexing = %s \n",uses4byteIndexing ? "true" : "false");

               uint8_t tmp_reserved_always_one_value = ByteOrder::le_to_host(*((uint8_t*)(buf+index)));

               reserved_always_one = tmp_reserved_always_one_value;

               printf ("reserved_always_one = %u \n",reserved_always_one);

            // DQ (12/13/2021): I thought this was supposed to be the value 1.
               //~ ROSE_ASSERT(reserved_always_one == 16);

            // PP (06/16/22): Seems to be 1 on some simple test code
               ROSE_ASSERT(reserved_always_one == 1);

               index += 1;

               uint64_t tmp_valid_value = ByteOrder::le_to_host(*((uint64_t*)(buf+index)));

               valid = tmp_valid_value;
               printf ("valid = %" PRIu64 "\n",valid);
#if 0
               printf ("valid = %zu \n",valid);
               printf ("valid = 0x%x \n",valid);
#endif
               size_t true_bits = 0;
               for (size_t i = 0; i < 64; i++)
                  {
                    bool bit = Rose::BitOps::bit(valid,i);
                    if (bit == true)
                       {
                         true_bits++;
                         string table_name = table_kind_to_string(Table_kinds(i));

                         printf ("Table %s is present \n",table_name.c_str());

                         table_list.push_back(Table_kinds(i));
                       }
#if 0
                    printf ("valid: bit %2zu value = %5s true_bits = %2zu \n",i,bit ? "true" : "false",true_bits);
#endif
                  }

               printf ("true_bits = %zu \n",true_bits);

#if 0
               std::vector<bool> valid_bitvector(64,valid);

               std::cout << "valid_bitvector = " << valid_bitvector << std::endl;
#endif
               index += 8;

               uint64_t tmp_sorted_value = ByteOrder::le_to_host(*((uint64_t*)(buf+index)));

               sorted = tmp_sorted_value;

               printf ("sorted = %zu \n",sorted);
               printf ("sorted = 0x%lx \n",sorted);
#if 0
               std::vector<bool> sorted_bitvector(64,sorted);

               std::cout << "sorted_bitvector = " << sorted_bitvector << std::endl;
#endif
               index += 8;

            // Save the number of rows associated with each table (in order matching the table_list).
               vector<size_t> number_of_rows;

               printf ("Output the number of rows for each table: \n");
               for (size_t i = 0; i < true_bits; i++)
                  {
                 // uint32_t tmp_rows_value = ByteOrder::le_to_host(*((uint32_t*)(buf+index+(i*4))));
                    uint32_t tmp_rows_value = ByteOrder::le_to_host(*((uint32_t*)(buf+index)));
                    printf ("--- table %2zu: tmp_rows_value = %u \n",i,tmp_rows_value);

                    index += 4;

                    number_of_rows.push_back(tmp_rows_value);
                  }

               ROSE_ASSERT(table_list.size() == number_of_rows.size());

               printf ("Output the valid tables present: \n");
               for (Table_kinds kind : table_list)
                  {
                 // Output the tables that are present.
                    printf (" --- found valid table: %s \n",table_kind_to_string(kind).c_str());
                  }

               printf ("Build the tables: \n");
               for (size_t i = 0; i < table_list.size(); ++i)
                  {
                    Table_kinds kind = table_list[i];
                    size_t rows = number_of_rows[i];

                 // printf ("table %s rows = %zu \n",table_kind_to_string(kind).c_str(),rows);

                 // Build the associated table.
                    switch (kind)
                       {
                         case e_assembly:
                            {
                              printf ("Build the e_assembly table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilAssembly* table_rose = parseAsmCilNode<SgAsmCilAssembly>(buf,index,uses4byteIndexing);
                                   assembly_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_assembly table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_assemblyProcessor:
                            {
                              printf ("Build the e_assemblyProcessor table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilAssemblyProcessor* table_rose = parseAsmCilNode<SgAsmCilAssemblyProcessor>(buf,index,uses4byteIndexing);
                                   assemblyProcessor_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_assemblyProcessor table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_assemblyOS:
                            {
                              printf ("Build the e_assemblyOS table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilAssemblyOS* table_rose = parseAsmCilNode<SgAsmCilAssemblyOS>(buf,index,uses4byteIndexing);
                                   assemblyOS_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_assemblyOS table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_assemblyRef:
                            {
                              printf ("Build the e_assemblyRef table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilAssemblyRef* table_rose = parseAsmCilNode<SgAsmCilAssemblyRef>(buf,index,uses4byteIndexing);
                                   assemblyRef_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_assemblyRef table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_assemblyRefProcessor:
                            {
                              printf ("Build the e_assemblyRefProcessor table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilAssemblyRefProcessor* table_rose = parseAsmCilNode<SgAsmCilAssemblyRefProcessor>(buf,index,uses4byteIndexing);
                                   assemblyRefProcessor_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_assemblyRefProcessor table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_assemblyRefOS:
                            {
                              printf ("Build the e_assemblyRef table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilAssemblyRefOS* table_rose = parseAsmCilNode<SgAsmCilAssemblyRefOS>(buf,index,uses4byteIndexing);
                                   assemblyRefOS_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_assemblyRefOS table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_classLayout:
                            {
                              printf ("Build the e_classLayout table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilClassLayout* table_rose = parseAsmCilNode<SgAsmCilClassLayout>(buf,index,uses4byteIndexing);
                                   classLayout_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_classLayout table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_constant:
                            {
                              printf ("Build the e_constant table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilConstant* table_rose = parseAsmCilNode<SgAsmCilConstant>(buf,index,uses4byteIndexing);
                                   constant_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_constant table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }


                         case e_customAttribute:
                            {
                              printf ("Build the e_customAttribute table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilCustomAttribute* table_rose = parseAsmCilNode<SgAsmCilCustomAttribute>(buf,index,uses4byteIndexing);
                                   customAttribute_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_customAttribute table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_declSecurity:
                            {
                              printf ("Build the e_declSecurity table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilDeclSecurity* table_rose = parseAsmCilNode<SgAsmCilDeclSecurity>(buf,index,uses4byteIndexing);
                                   declSecurity_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_declSecurity table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_eventMap:
                            {
                              printf ("Build the e_eventMap table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilEventMap* table_rose = parseAsmCilNode<SgAsmCilEventMap>(buf,index,uses4byteIndexing);
                                   eventMap_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_eventMap table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_event:
                            {
                              printf ("Build the e_assemblyRef table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilAssemblyRef* table_rose = parseAsmCilNode<SgAsmCilAssemblyRef>(buf,index,uses4byteIndexing);
                                   assemblyRef_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_assemblyRef table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_exportedType:
                            {
                              printf ("Build the e_exportedType table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilExportedType* table_rose = parseAsmCilNode<SgAsmCilExportedType>(buf,index,uses4byteIndexing);
                                   exportedType_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_exportedType table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_field:
                            {
                              printf ("Build the e_field table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilField* table_rose = parseAsmCilNode<SgAsmCilField>(buf,index,uses4byteIndexing);
                                   field_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_field table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_fieldLayout:
                            {
                              printf ("Build the e_fieldLayout table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilFieldLayout* table_rose = parseAsmCilNode<SgAsmCilFieldLayout>(buf,index,uses4byteIndexing);
                                   fieldLayout_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_fieldLayout table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_fieldMarshal:
                            {
                              printf ("Build the e_fieldMarshal table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilFieldMarshal* table_rose = parseAsmCilNode<SgAsmCilFieldMarshal>(buf,index,uses4byteIndexing);
                                   fieldMarshal_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_fieldMarshal table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_fieldRVA:
                            {
                              printf ("Build the e_fieldRVA table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilFieldRVA* table_rose = parseAsmCilNode<SgAsmCilFieldRVA>(buf,index,uses4byteIndexing);
                                   fieldRVA_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_fieldRVA table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_file:
                            {
                              printf ("Build the e_file table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilFile* table_rose = parseAsmCilNode<SgAsmCilFile>(buf,index,uses4byteIndexing);
                                   file_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_file table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_genericParam:
                            {
                              printf ("Build the e_genericParam table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilGenericParam* table_rose = parseAsmCilNode<SgAsmCilGenericParam>(buf,index,uses4byteIndexing);
                                   genericParam_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_genericParam table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_genericParamConstraint:
                            {
                              printf ("Build the e_genericParamConstraint table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilGenericParamConstraint* table_rose = parseAsmCilNode<SgAsmCilGenericParamConstraint>(buf,index,uses4byteIndexing);
                                   genericParamConstraint_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_genericParamConstraint table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_implMap:
                            {
                              printf ("Build the e_implMap table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilImplMap* table_rose = parseAsmCilNode<SgAsmCilImplMap>(buf,index,uses4byteIndexing);
                                   implMap_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_implMap table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_interfaceImpl:
                            {
                              printf ("Build the e_interfaceImpl table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilInterfaceImpl* table_rose = parseAsmCilNode<SgAsmCilInterfaceImpl>(buf,index,uses4byteIndexing);
                                   interfaceImpl_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_interfaceImpl table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_manifestResource:
                            {
                              printf ("Build the e_manifestResource table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilManifestResource* table_rose = parseAsmCilNode<SgAsmCilManifestResource>(buf,index,uses4byteIndexing);
                                   manifestResource_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_manifestResource table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_memberRef:
                            {
                              printf ("Build the e_memberRef table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilMemberRef* table_rose = parseAsmCilNode<SgAsmCilMemberRef>(buf,index,uses4byteIndexing);
                                   memberRef_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_memberRef table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_methodDef:
                            {
                              printf ("Build the e_methodDef table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilMethodDef* table_rose = parseAsmCilNode<SgAsmCilMethodDef>(buf,index,uses4byteIndexing);
                                   methodDef_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_methodDef table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_methodImpl:
                            {
                              printf ("Build the e_methodImpl table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilMethodImpl* table_rose = parseAsmCilNode<SgAsmCilMethodImpl>(buf,index,uses4byteIndexing);
                                   methodImpl_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_methodImpl table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_methodSemantics:
                            {
                              printf ("Build the e_methodSemantics table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilMethodSemantics* table_rose = parseAsmCilNode<SgAsmCilMethodSemantics>(buf,index,uses4byteIndexing);
                                   methodSemantics_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_methodSemantics table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_methodSpec:
                            {
                              printf ("Build the e_methodSpec table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilMethodSpec* table_rose = parseAsmCilNode<SgAsmCilMethodSpec>(buf,index,uses4byteIndexing);
                                   methodSpec_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_methodSpec table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_module:
                            {
                              printf ("Build the e_module table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                // vector<SgAsmCilModule*> module_table;

                                // MetadataRoot(uint8_t* buf, size_t index)
                                   SgAsmCilModule* table_rose = parseAsmCilNode<SgAsmCilModule>(buf,index,uses4byteIndexing);
                                   module_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_module table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_moduleRef:
                            {
                              printf ("Build the e_moduleRef table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilModuleRef* table_rose = parseAsmCilNode<SgAsmCilModuleRef>(buf,index,uses4byteIndexing);
                                   moduleRef_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_moduleRef table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_nestedClass:
                            {
                              printf ("Build the e_nestedClass table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilNestedClass* table_rose = parseAsmCilNode<SgAsmCilNestedClass>(buf,index,uses4byteIndexing);
                                   nestedClass_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_nestedClass table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_param:
                            {
                              printf ("Build the e_param table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilParam* table_rose = parseAsmCilNode<SgAsmCilParam>(buf,index,uses4byteIndexing);
                                   param_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_param table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_property:
                            {
                              printf ("Build the e_property table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilProperty* table_rose = parseAsmCilNode<SgAsmCilProperty>(buf,index,uses4byteIndexing);
                                   property_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_property table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_propertyMap:
                            {
                              printf ("Build the e_propertyMap table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilPropertyMap* table_rose = parseAsmCilNode<SgAsmCilPropertyMap>(buf,index,uses4byteIndexing);
                                   propertyMap_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_propertyMap table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_standAloneSig:
                            {
                              printf ("Build the e_standAloneSig table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilStandAloneSig* table_rose = parseAsmCilNode<SgAsmCilStandAloneSig>(buf,index,uses4byteIndexing);
                                   standAloneSig_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_standAloneSig table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_typeDef:
                            {
                              printf ("Build the e_typeDef table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilTypeDef* table_rose = parseAsmCilNode<SgAsmCilTypeDef>(buf,index,uses4byteIndexing);
                                   typeDef_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_typeDef table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }

                         case e_typeRef:
                            {
                              printf ("Build the e_typeRef table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilTypeRef* table_rose = parseAsmCilNode<SgAsmCilTypeRef>(buf,index,uses4byteIndexing);
                                   typeRef_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_typeRef table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }


                         case e_typeSpec:
                            {
                              printf ("Build the e_typeSpec table: rows = %zu \n",rows);

                              for (size_t j=0; j < rows; j++)
                                 {
                                   printf (" --- processing row j = %zu \n",j);

                                   SgAsmCilTypeSpec* table_rose = parseAsmCilNode<SgAsmCilTypeSpec>(buf,index,uses4byteIndexing);
                                   typeSpec_table.push_back(table_rose);
#if 1
                                   printf ("DONE: processing row j = %zu \n",j);
                                // ROSE_ASSERT(false);
#endif
                                 }
#if 1
                              printf ("DONE: Build the e_typeSpec table: rows = %zu \n",rows);
                           // ROSE_ASSERT(false);
#endif
                              break;
                            }


                         default:
                            {
                              printf ("default reached \n");

                              printf ("parsing of table %u %s is not implemented \n",kind,table_kind_to_string(kind).c_str());
                              ROSE_ASSERT(false);

                              break;
                            }
                       }
                  }

               printf ("Generate the RVAs for each method: \n");
               for (size_t i=0; i < methodDef_table.size(); i++)
                  {
                    SgAsmCilMethodDef* row = methodDef_table[i];
                    uint32_t RVA = row->get_RVA();

                 // Save the RVA.
                    rvaList.push_back(RVA);

                    printf ("rvaList.size() = %zu RVA = %u \n",rvaList.size(),RVA);
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }
   };


enum StreamKindEnum
   {
     unknown_kind,
     error_kind,
     hash_tilda,
     hash_Strings,
     hash_US,
     hash_GUID,
     hash_Blob,
     last_kind
   };

std::string trim_trailing_null_terminator(const std::string& str)
   {
  // printf ("trim_trailing_null_terminator(): str = %s \n",str.c_str());

     std::string null_character = "\0";
     const auto strEnd = str.find_last_not_of(null_character);
     std::string return_string = str.substr(0, strEnd);

  // printf ("return_string = %s \n",return_string.c_str());

     return return_string;
   }

// Data structures to support .net metadata layout within the .text section.
class StreamHeader
   {
     public:
          uint32_t offset;
          uint32_t size;
          std::string name;
          enum StreamKindEnum streamKind;

     public:
          StreamHeader(uint8_t* buf, size_t & index, const size_t & start_of_MetadataRoot)
             {
               printf ("In StreamHeader constructor: buf = 0x%" PRIxPTR " index = %zu \n",(uintptr_t)buf,index);

               uint32_t tmp_offset_value = ByteOrder::le_to_host(*((uint32_t*)(buf+index)));

               offset = tmp_offset_value;

               printf ("offset = %u \n",offset);

               index += 4;

               uint32_t tmp_size_value = ByteOrder::le_to_host(*((uint32_t*)(buf+index)));

               size = tmp_size_value;

               printf ("size = %u \n",size);

               index += 4;

            // Save the version string
               std::string s;
               bool endOfString = false;
            // for (size_t i = 0; i < size; i++)
               size_t i = 0;
               while (endOfString == false)
                  {
                 // printf ("index = %zu i = %zu \n",index,i);

                    uint8_t* tmp_name_value = (uint8_t*)(buf+index+i);

                 // printf ("tmp_version_value = %c \n",tmp_version_value);
                 // printf ("tmp_version_value = 0x%xc \n",tmp_version_value);

                    char char_value = (char) *tmp_name_value;

                    if (char_value == 0)
                       {
                         if (index % 4 == 0)
                            {
                              endOfString = true;
                            }
                       }

                    i++;

                 // printf ("char_value = %c \n",char_value);

                 // s += *tmp_version_value;
                    s += char_value;
                  }

            // DQ (12/13/2021): We need to trim off the trailing null termator that is a part of the std::string object.
            // Doing so allows us to do the comparisions below to set the streamKind data member.
            // name = s;
               name = trim_trailing_null_terminator(s);

               printf ("name string = |%s| length = %zu \n",name.c_str(),name.length());
#if 0
               for (size_t i = 0; i < name.length(); i++)
                  {
                    printf ("name[%d] = %d \n",i,name[i]);
                  }
#endif
               streamKind = unknown_kind;

               if (name == "#~")
                  {
                    printf ("Is #~ \n");
                    streamKind = hash_tilda;
                  }
                 else if (name == "#Strings")
                  {
                    printf ("Is #Strings \n");
                    streamKind = hash_Strings;
                  }
                 else if (name == "#US")
                  {
                    printf ("Is #US \n");
                    streamKind = hash_US;
                  }
                 else if (name == "#GUID")
                  {
                    printf ("Is #GUID \n");
                    streamKind = hash_GUID;
                  }
                 else if (name == "#Blob")
                  {
                    printf ("Is #Blob \n");
                    streamKind = hash_Blob;
                  }
                 else
                  {
                    printf ("Is error_kind \n");
                    streamKind = error_kind;
                  }

               ROSE_ASSERT(streamKind != error_kind);
               ROSE_ASSERT(streamKind != unknown_kind);

            // DQ (12/13/2021): We need to add 1 to account for the trailing null terminator that was trimmed off.
               size_t name_length = name.length() + 1;

               printf ("name_length = %zu \n",name_length);

               printf ("Before adjustment for string size: index = %zu \n",index);

            // DQ (12/12/2021): I think that the size may include the size of the name.
            // Skip over the string
            // index += 4 + size + name_length;
            // index += 4 + size + name_length;
            // index += 4 + size; // + name_length;
            // index += size; // + name_length;
            // index += size + name_length;
            // index += size;
            // index += name_length;
            // index += (name_length / 4) + (name_length % 4);
            // index += (name_length / 4) + (name_length % 4) + (4 - (name_length % 4));
            // size_t increment = (name_length / 4) + ((name_length % 4) == 0) ? 0 : (4 - (name_length % 4));
               size_t increment = ((name_length % 4) == 0) ? (name_length / 4) : ((name_length / 4) + 1);

               printf ("Before multiping by 4: increment = %zu \n",increment);

               increment *= 4;

               printf ("final increment = %zu \n",increment);

               index += increment;

               printf ("size = %u name_length = %zu index = %zu \n",size,name_length,index);

            // index = offset;
#if 0
            // Copy the bytes in the stream.
               for (size_t i = 0; i < size; i++)
                  {
                 // uint8_t* tmp_byte_value = (uint8_t*)(buf+index+i);
                    uint8_t* tmp_byte_value = (uint8_t*)(buf+offset+i);
                    printf ("*tmp_byte_value = 0x%x \n",*tmp_byte_value);
                  }
#endif

               // ******************************************************************************
               //                           FIX ME IN THE MORNING
               // ******************************************************************************


               printf ("start_of_MetadataRoot = %zu \n",start_of_MetadataRoot);

            // Or maybe we need to address of the magic number???
            // size_t lower_bound = (uint32_t)(&(buf[0])) + offset;
            // size_t lower_bound = ((size_t) buf) + offset;
            // size_t lower_bound = start_of_MetadataRoot + offset;
            // long offset_of_MetadataRoot = start_of_MetadataRoot - (size_t)buf;
            // printf ("offset_of_MetadataRoot = %dl \n",offset_of_MetadataRoot);
            // size_t lower_bound = ((size_t) buf) + offset_of_MetadataRoot + offset;
               size_t lower_bound = start_of_MetadataRoot + offset;

               size_t upper_bound = lower_bound + size;

               printf ("lower_bound = %zu upper_bound = %zu size = %" PRIu32 " \n",lower_bound,upper_bound,size);

               ROSE_ASSERT(lower_bound <= upper_bound);

            // The standard says this should be true as well.
               ROSE_ASSERT(size % 4 == 0);

               vector<uint8_t> string_heap;
               if (streamKind == hash_Strings)
                  {
                    printf ("Gathering data from the #Strings heap \n");

                 // Copy the bytes in the stream.
                    for (size_t i = lower_bound; i < upper_bound; i++)
                       {
                      // uint8_t* tmp_byte_value = (uint8_t*)(buf+index+i);
                      // uint8_t* tmp_byte_value = (uint8_t*)(buf+offset+i);
                      // uint8_t value = ByteOrder::le_to_host(*((uint8_t*)(buf+i)));
                      // uint8_t value = ByteOrder::le_to_host(*((uint8_t*)i));
                         uint8_t value = ByteOrder::le_to_host(*((uint8_t*)(buf+i)));
#if 0
                         printf ("value = 0x%x %c \n",value, isprint(value) ? value : '.');
#endif
                         string_heap.push_back(value);
                       }

#if 0
                    printf ("Exiting after test! \n");
                    ROSE_ASSERT(false);
#endif
                  }

               vector<uint8_t> US_heap;
               if (streamKind == hash_US)
                  {
                    printf ("Gathering data from the #US heap \n");

                 // Copy the bytes in the stream.
                    for (size_t i = lower_bound; i < upper_bound; i++)
                       {
                      // uint8_t* tmp_byte_value = (uint8_t*)(buf+index+i);
                      // uint8_t* tmp_byte_value = (uint8_t*)(buf+offset+i);
                      // uint8_t value = ByteOrder::le_to_host(*((uint8_t*)(buf+i)));
                      // uint8_t value = ByteOrder::le_to_host(*((uint8_t*)i));
                         uint8_t value = ByteOrder::le_to_host(*((uint8_t*)(buf+i)));
#if 0
                         printf ("value = 0x%x \n",value);
#endif
                      // printf ("value = 0x%x %c \n",value, isprint(value) ? value : '.');

                         US_heap.push_back(value);
                       }

#if 0
                    printf ("Exiting after test! \n");
                    ROSE_ASSERT(false);
#endif
                  }

               vector<uint8_t> Blob_heap;
               if (streamKind == hash_Blob)
                  {
                    printf ("Gathering data from the #Blob heap \n");

                 // Copy the bytes in the stream.
                    for (size_t i = lower_bound; i < upper_bound; i++)
                       {
                      // uint8_t* tmp_byte_value = (uint8_t*)(buf+index+i);
                      // uint8_t* tmp_byte_value = (uint8_t*)(buf+offset+i);
                      // uint8_t value = ByteOrder::le_to_host(*((uint8_t*)(buf+i)));
                      // uint8_t value = ByteOrder::le_to_host(*((uint8_t*)i));
                         uint8_t value = ByteOrder::le_to_host(*((uint8_t*)(buf+i)));
#if 0
                         printf ("value = 0x%x \n",value);
#endif
                         Blob_heap.push_back(value);
                       }
#if 0
                    printf ("Exiting after test! \n");
                    ROSE_ASSERT(false);
#endif
                  }

            // DQ (12/18/2021): #GUID is in terms of 128-bit values.
            // vector<uint64_t> GUID_heap;
               vector<long long unsigned int> GUID_heap;
               if (streamKind == hash_GUID)
                  {
                    printf ("Gathering data from the #GUID heap \n");

                    printf ("sizeof(long long unsigned int) = %zu \n",sizeof(long long unsigned int));

                 // Copy the bytes in the stream.
                    for (size_t i = lower_bound; i < upper_bound; i++)
                       {
                      // uint8_t* tmp_byte_value = (uint8_t*)(buf+index+i);
                      // uint8_t* tmp_byte_value = (uint8_t*)(buf+offset+i);
                      // uint8_t value = ByteOrder::le_to_host(*((uint8_t*)(buf+i)));
                      // uint8_t value = ByteOrder::le_to_host(*((uint8_t*)i));
                         uint8_t value = ByteOrder::le_to_host(*((uint8_t*)(buf+i)));
#if 0
                         printf ("value = 0x%x \n",value);
#endif
                         GUID_heap.push_back(value);
                       }
#if 0
                    printf ("Exiting after test! \n");
                    ROSE_ASSERT(false);
#endif
                  }



             }
   };


class MetadataRoot
   {
     public:
          uint32_t signature;
          uint16_t majorVersion;
          uint16_t minorVersion;
       // uint32_t reservedSpace_0;
          uint32_t length;
          std::string version;
       // uint32_t padding;
          uint16_t flags;
          uint16_t numberOfStreams;
          std::vector<StreamHeader*> streamHeaders;

#if 0
          String_Heap* string_heap;
          US_Heap* US_heap;
          Blob_Heap* blob_heap;
          GUID_Heap* GUID_heap;
#endif

     public:
          MetadataRoot(uint8_t* buf, size_t& index)
             {
               size_t start_of_MetadataRoot = index;

            // Initialize the elements of the data structure.
               printf ("Initialize the elements of the data structure \n");

               uint32_t tmp_signature_value = ByteOrder::le_to_host(*((uint32_t*)(buf+index)));
               signature = tmp_signature_value;

               printf ("index = %zu signature = %u \n",index,signature);
               printf ("index = %zu signature = 0x%x \n",index,signature);

               index += 4;

               uint16_t tmp_majorVersion_value = ByteOrder::le_to_host(*((uint16_t*)(buf+index)));
               majorVersion = tmp_majorVersion_value;

               printf ("index = %zu majorVersion = %u \n",index,majorVersion);

               index += 2;

               uint16_t tmp_minorVersion_value = ByteOrder::le_to_host(*((uint16_t*)(buf+index)));
               majorVersion = tmp_minorVersion_value;

               printf ("index = %zu minorVersion = %u \n",index,minorVersion);

               index += 2;

            // Skip over the reservedSpace
               index += 4;

               uint32_t tmp_length_value = ByteOrder::le_to_host(*((uint32_t*)(buf+index)));
               length = tmp_length_value;

               printf ("index = %zu length = %u \n",index,length);

               index += 4;

               printf ("Save the version string \n");

            // Save the version string
               std::string s;
               for (size_t i = 0; i < length; i++)
                  {
                 // printf ("index = %zu i = %zu \n",index,i);

                    uint8_t* tmp_version_value = (uint8_t*)(buf+index+i);

                 // printf ("tmp_version_value = %c \n",tmp_version_value);
                 // printf ("tmp_version_value = 0x%xc \n",tmp_version_value);

                    char char_value = (char) *tmp_version_value;

                 // printf ("char_value = %c \n",char_value);

                 // s += *tmp_version_value;
                    s += char_value;
                  }

               version = s;
               printf ("version string = %s \n",s.c_str());

            // Skip over the string
               index += length;

            // DQ (12/6/21): The lenght being zero is incorrect and likely because the padding is not computed correctly.
            // Skip over the padding
            // index += 4;
               int remainder = length % 4;
               if (remainder != 0)
                  {
                    index += remainder;
                  }

               uint16_t tmp_flags_value = ByteOrder::le_to_host(*((uint16_t*)(buf+index)));
               flags = tmp_flags_value;

            // printf ("index = %zu flags = %u \n",index,flags);
               printf ("index = %zu flags = 0x%x \n",index,flags);

               index += 2;

               uint16_t tmp_numberOfStreams_value = ByteOrder::le_to_host(*((uint16_t*)(buf+index)));
               numberOfStreams = tmp_numberOfStreams_value;

               printf ("index = %zu numberOfStreams = %u \n",index,numberOfStreams);

               index += 2;

               printf ("streamHeaders.size() = %zu \n",streamHeaders.size());

               for (size_t i = 0; i < numberOfStreams; i++)
                 {
                   printf ("START: stream header %zu of %u: index = %zu \n",i,numberOfStreams,index);

                   StreamHeader* stream_header = new StreamHeader(buf,index,start_of_MetadataRoot);

                   printf ("END: stream header %zu of %u: index = %zu \n",i,numberOfStreams,index);

                   streamHeaders.push_back(stream_header);

                // Now fill in the top level #~ data structure.
                   if (stream_header->name == "#~")
                      {
                        size_t offset = start_of_MetadataRoot + stream_header->offset;

                        printf ("start_of_MetadataRoot = %zu stream_header->offset = %" PRIu32 " offset = %zu \n",start_of_MetadataRoot,stream_header->offset,offset);

                        // \todo PP (06/16/22): Where shall we store the MetadataTable object?
                        //       talk with Robb where this should go.
                        MetadataTable* metadataTable = new MetadataTable(buf,offset);
                      }
                 }

             }
   };


// Do a traversal over the AST, get the sections, look for the section with name ".text" and get its mapped address (get_mapped_actual_va()) and size (get_size()).

void look_for_cil_streams(Settings & settings,MemoryMap::Ptr map)
   {
  // DQ (11/7/2021): This function is exploring the .text section for Cil streams (somthing like subsections of the .text section).

  // The layout of the metadata parts of the .text section are found on page 272 of the CIL standard.

  // Calling the disassembler:
  // ../../testLinearDisassembler --isa=cil /home/quinlan1/ROSE/git_ldrd2_development/tests/nonsmoke/functional/CompileTests/experimental_dotNet_tests/hello_world.exe

  // Build the RBA file:
  // /data1/ROSE_CompileTree/git-LINUX-64bit-6.1.0-EDG60-BOOST_1_61-dq-development-ldrd2-rc-binary-only/tools/BinaryAnalysis/bat-ana --no-disassemble -o hello_world.rba  /home/quinlan1/ROSE/git_ldrd2_development/tests/nonsmoke/functional/CompileTests/experimental_dotNet_tests/hello_world.exe

  // Calling the hex dump tool:
  // /data1/ROSE_CompileTree/git-LINUX-64bit-6.1.0-EDG60-BOOST_1_61-dq-development-ldrd2-rc-binary-only/tools/BinaryAnalysis/bat-mem --where 0x00402004+0x000003fc -Fhexdump hello_world.rba

     rose_addr_t va = settings.startVa;

     printf ("va = %zx \n",va);

#if 0
     int counter = 0;
     while (map->atOrAfter(va).require(MemoryMap::EXECUTABLE).next().assignTo(va))
        {
          va = alignUp(va, settings.alignment);

          if (*((char*)va) == '#')
             {
               printf ("Found the start of a Cil stream in the .text section \n");
               char* current_char   = (char*)va;
               char* next_char      = (current_char+1);
               char* next_next_char = (current_char+2);
               printf (" --- counter %d stream name = %x %x %c%c \n",counter,*current_char,*next_char,*next_next_char,*current_char,*next_char,*next_next_char);
             }

          va++;
          counter++;
        }
#endif

     size_t max_section_size = 1024*100;
     uint8_t buf[max_section_size];
     AddressInterval where = map->atOrAfter(va).require(MemoryMap::EXECUTABLE).limit(sizeof buf).read(buf);

  // DQ (12/2/2021): Location of .text section and it's size: 0x00402004 + 0x000003fc
     ::mlog[INFO] << "where: " << where << std::endl;

  // printf ("Output the value of where: \n");
  // Rose::StringUtility::addrToString(where);

  // Generate the upper and lower bounds.
  // rose_addr_t least    = where.least();
  // rose_addr_t greatest = where.greatest();

  // Might overflow to zero if the section is the whole space.
     rose_addr_t size = where.size();
     max_section_size = size;

  // Make sure this is not a result of overlow and set to zero.
     ROSE_ASSERT(max_section_size > 0);

  // Now we can iterate through the buffer directly.
     size_t index = 0;

     bool found_magic_number      = false;
     bool metadata_root_processed = false;

     while (index < max_section_size)
        {
       // Find the magic number:
       // 42 53 4a 42
       // uint32_t integer_value = ByteOrder::le_to_host(*((uint32_t*)(raw_bytes.data()+1)));
       // uint32_t integer_value = ByteOrder::le_to_host(*((uint32_t*)(buf+1)));
          uint32_t integer_value = ByteOrder::le_to_host(*((uint32_t*)(buf+index)));
       // if (buf[index] == 0x42 && buf[index+1] == 0x53 && buf[index+2] == 0x4a && buf[index+3] == 0x42)
          if (integer_value == 0x424a5342)
             {
            // printf ("Found .net magic number at index = %zu integer_value = %u \n",index,integer_value);
            // printf ("Found .net magic number at index = %zu integer_value = %x \n",index,integer_value);
               printf ("Found .net magic number at index = %zu integer_value = 0x%x \n",index,integer_value);
               found_magic_number = true;
             }

          if (found_magic_number == true && metadata_root_processed == false)
             {
               printf ("Found the start of a Cil Metadata Root in the .text section \n");

               MetadataRoot* metadata_root = new MetadataRoot(buf,index);
               metadata_root_processed = true;

               ROSE_ASSERT(metadata_root != NULL);
             }
#if 0
          if (found_magic_number == true && buf[index] == '#')
             {
               printf ("Found the start of a Cil stream in the .text section \n");
               char current_char = buf[index];
               char next_char    = buf[index+1];
               printf (" --- index %zu stream name = %x %x %c%c \n",index,current_char,next_char,current_char,next_char);
             }
#endif
          ++index;
        }

#if 0
     printf ("Exiting as a test at the bottom of look_for_cil_streams() \n");
     ROSE_ASSERT(false);
#endif
   }


int main(int argc, char *argv[])
{
    ROSE_INITIALIZE;

#if 1
    printf ("In TOP of main() \n");
#endif

    Diagnostics::initAndRegister(&::mlog, "tool");

    // Parse the command-line
    P2::Engine engine;
    Settings settings;
    std::vector<std::string> specimenNames = parseCommandLine(argc, argv, engine, settings);

    // Load the specimen as raw data or an ELF or PE container
    MemoryMap::Ptr map = engine.loadSpecimens(specimenNames);
    map->dump(::mlog[INFO]);
    map->dump(std::cout);
    Disassembler *disassembler = engine.obtainDisassembler();

    // Obtain an unparser suitable for this disassembler
    AsmUnparser unparser;
    unparser.set_registers(disassembler->registerDictionary());

 // DQ (12/18/2021): See if we can comment this out, I think we are not using it.
 // Build semantics framework; only used when settings.runSemantics is set
    BaseSemantics::DispatcherPtr dispatcher;

#if 1
    if (settings.runSemantics) {
        BaseSemantics::RiscOperatorsPtr ops = SymbolicSemantics::RiscOperators::instance(disassembler->registerDictionary());
        ops = TraceSemantics::RiscOperators::instance(ops);
        dispatcher = DispatcherM68k::instance(ops, disassembler->wordSizeBytes()*8);
        dispatcher->currentState()->memoryState()->set_byteOrder(ByteOrder::ORDER_MSB);
    }
#endif

#if 1
    // DQ (11/7/2021): Start looking for the streams (Cil terminology) within the .text section.
    look_for_cil_streams(settings, map);
#endif

#if 0
    printf ("Exiting after unparsing the CIL streams (skipping disassembly of instructions until we can resolve RVA) \n");
    return 0;
#endif

    printf ("Generated rvaList.size() = %zu \n",rvaList.size());
    for (size_t i=0; i < rvaList.size(); i++)
       {
      // Note: the location of the instructions for each method are at the 0x00400000 (base of PE Header file) + RVA.
         uint32_t RVA = rvaList[i];
         printf ("rvaList[%zu] = RVA = %u 0x%x \n",i,RVA,RVA);
       }

    // Disassemble at each valid address, and show disassembly errors

    // rose_addr_t va = settings.startVa;
    // while (map->atOrAfter(va).require(MemoryMap::EXECUTABLE).next().assignTo(va)) {
    // Start at the first address of the RVA (what ever that is).

    rose_addr_t va = settings.startVa;

    printf ("Before reset using RVA: va = 0x%" PRIu64 " \n",va);

#if 1
 // Reset the va to incude the generated RVA
 // va = rvaList[0];
#if 1
    va = 0x00400000 + rvaList[0];
#else
 // DQ (12/23/2021): After discussion with Robb, here is the way to compute the constant value.
 // Need to compute the value 0x00400000
    SgAsmInterpretation* interpretation = engine.interpretation();

 // Find the
    SgAsmGenericList* headerList = interpretation.get_headers();

 // Look for the SgAsmPEFileHeader
    SgAsmPEFileHeader* PeFileHeader = (found in headerList)

 // Should generate constant: 0x00400000
    rose_addr_t base_va = PeFileHeader->get_base_va();

    va = base_va + rvaList[0];
#endif

    printf ("After explicitly setting va(using 0x00400000 + RVA): va = 0x%" PRIu64 " \n",va);

 // DQ (12/20/2021): Robb sent me this line of code that is correct since our offset is into the map.
 // uint8_t method_header_leading_byte_value = ByteOrder::le_to_host(*((uint8_t*)va));
    uint8_t method_header_leading_byte_value = 0;
    size_t nRead = map->at(va).limit(1).read(&method_header_leading_byte_value).size();

    ROSE_ASSERT(nRead == 1);

    printf ("method_header_leading_byte_value = 0x%x \n",method_header_leading_byte_value);

    // uint8_t method_header_leading_byte_value_alt_1 = ByteOrder::le_to_host(*((uint8_t*)rvaList[0]));
    // printf ("method_header_leading_byte_value_alt_1 = 0x%x \n",method_header_leading_byte_value_alt_1);

 // These are zero, could it be that we want the 2nd byte because of little-endian ordering?
    bool bit_1 = Rose::BitOps::bit(method_header_leading_byte_value,0);
    bool bit_2 = Rose::BitOps::bit(method_header_leading_byte_value,1);

    printf ("bit_1 = %s \n",bit_1 ? "true" : "false");
    printf ("bit_2 = %s \n",bit_2 ? "true" : "false");

#if 0
 // I get a different result if these are commented out?????
    bool bit_3 = Rose::BitOps::bit(method_header_leading_byte_value,2);
    bool bit_4 = Rose::BitOps::bit(method_header_leading_byte_value,3);
    bool bit_5 = Rose::BitOps::bit(method_header_leading_byte_value,4);
    bool bit_6 = Rose::BitOps::bit(method_header_leading_byte_value,5);
    bool bit_7 = Rose::BitOps::bit(method_header_leading_byte_value,6);
    bool bit_8 = Rose::BitOps::bit(method_header_leading_byte_value,7);

    printf ("bit_3 = %s \n",bit_3 ? "true" : "false");
    printf ("bit_4 = %s \n",bit_4 ? "true" : "false");
    printf ("bit_5 = %s \n",bit_5 ? "true" : "false");
    printf ("bit_6 = %s \n",bit_6 ? "true" : "false");
    printf ("bit_7 = %s \n",bit_7 ? "true" : "false");
    printf ("bit_8 = %s \n",bit_8 ? "true" : "false");
#endif

    if (bit_2 == true)
       {
         if (bit_1 == false)
            {
           // This is a tiny header
            }
           else
            {
           // This is a fat header
            }
       }

    printf ("After reset using RVA: va = 0x%" PRIu64 "\n",va);
#endif

#if 1
    printf ("Exiting before processing instructions \n");
    return 0;
#endif

 // DQ (12/17/2021): Robb is sending a new while loop.
 // while (map->atOrAfter(va).require(MemoryMap::EXECUTABLE).next().assignTo(va))
 // while (map->at(va).require(MemoryMap::EXECUTABLE).next())

 // DQ (12/18/2021): Use the original while loop.
 // while (map->atOrAfter(va).require(MemoryMap::EXECUTABLE).next().assignTo(va))
    while (map->at(va).require(MemoryMap::EXECUTABLE).next())
       {
     // We likely don't need this for .NET
     // va = alignUp(va, settings.alignment);

     // DQ (12/17/2021): Need to accumulate the instructions into a list.
     // And attach the list to the MethodDef table (or that is an intermediate solution).
        try {
#if 0
            printf ("Before calling disassembler->disassembleOne(map, va): va = 0x%x \n",va);
#endif
#if 0
            printf ("Increment the va using the RVA for the first method \n");
            va = va + rvaList[0];
            printf ("Before calling disassembler->disassembleOne(map, va): va = 0x%x \n",va);
#endif
#if 0
            printf ("Reset the va using the RVA for the first method \n");
            va = rvaList[0];
            printf ("Before calling disassembler->disassembleOne(map, va): va = 0x%x \n",va);
#endif
            SgAsmInstruction *insn = disassembler->disassembleOne(map, va);
            ASSERT_not_null(insn);
            unparser.unparse(std::cout, insn);

            if (settings.runSemantics) {
                if (isSgAsmM68kInstruction(insn)) {
                    bool skipThisInstruction = false;
#if 0 // [Robb P. Matzke 2014-07-29]
                    switch (isSgAsmM68kInstruction(insn)->get_kind()) {
                        case m68k_cpusha:
                        case m68k_cpushl:
                        case m68k_cpushp:
                            std::cout <<"    No semantics yet for privileged instructions\n";
                            skipThisInstruction = true;
                            break;

                        case m68k_fbeq:
                        case m68k_fbne:
                        case m68k_fboge:
                        case m68k_fbogt:
                        case m68k_fbule:
                        case m68k_fbult:
                        case m68k_fcmp:
                        case m68k_fdabs:
                        case m68k_fdadd:
                        case m68k_fddiv:
                        case m68k_fdiv:
                        case m68k_fdmove:
                        case m68k_fdmul:
                        case m68k_fdneg:
                        case m68k_fdsqrt:
                        case m68k_fdsub:
                        case m68k_fintrz:
                        case m68k_fmove:
                        case m68k_fmovem:
                        case m68k_fsadd:
                        case m68k_fsdiv:
                        case m68k_fsmove:
                        case m68k_fsmul:
                        case m68k_fsneg:
                        case m68k_fssub:
                        case m68k_ftst:
                            std::cout <<"    No semantics yet for floating-point instructions\n";
                            skipThisInstruction = true;
                            break;

                        case m68k_nbcd:
                        case m68k_rtm:
                        case m68k_movep:
                            std::cout <<"    No semantics yet for this odd instruction\n";
                            skipThisInstruction = true;
                            break;

                        default:
                            break;
                    }
#endif

                    if (!skipThisInstruction) {
                        //ops->currentState()->clear();
                        dispatcher->processInstruction(insn);
                        std::ostringstream ss;
                        ss <<*dispatcher->currentState();
                        std::cout <<StringUtility::prefixLines(ss.str(), "    ") <<"\n";
                    }
                }
            }


            va += insn->get_size();
            if (0 != va % settings.alignment)
                std::cerr <<StringUtility::addrToString(va) <<": invalid alignment\n";
#if 0 // [Robb P. Matzke 2014-06-19]: broken
            deleteAST(insn);
#endif
        } catch (const Disassembler::Exception &e) {
            std::cerr <<StringUtility::addrToString(va) <<": " <<e.what() <<"\n";
            ++va;
        }
    }

#if 1
    printf ("Leaving main() \n");
#endif

    exit(0);
}
