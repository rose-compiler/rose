#ifdef ROSE_IMPL
#include <SgAsmCoffSymbolList.h>
#endif

/** COFF symbol table.
 *
 *  This is a symbol table used by Microsoft PE format. */
class SgAsmCoffSymbolTable: public SgAsmGenericSection {
public:
    /** Property: String table.
     *
     *  Table that holds the strings for the symbol names. */
    [[using Rosebud: rosetta]]
    SgAsmGenericSection* strtab = nullptr;

    /** Property: List of symbols. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCoffSymbolList* symbols = createAndParent<SgAsmCoffSymbolList>(this);

public:
    explicit SgAsmCoffSymbolTable(SgAsmPEFileHeader*);
    size_t get_nslots() const;
    virtual SgAsmCoffSymbolTable *parse() override;
    virtual void unparse(std::ostream&) const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
