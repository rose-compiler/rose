class SgAsmDwarfVariable: public SgAsmDwarfConstruct
   {
     public:
       // DQ (11/21/2025): Added support for Dwarf 4.
          [[using Rosebud: rosetta]]  
          std::string linkage_name;
     
       // DQ (11/21/2025): Added support for Dwarf 4.
          [[using Rosebud: rosetta]]  
          bool is_const_expr = false;  
   };

