class SgAsmDwarfEnumerationType: public SgAsmDwarfConstruct {

    [[using Rosebud: rosetta, traverse]]
    SgAsmDwarfConstructList* body = NULL;

public:
    SgAsmDwarfConstructList* get_children() override;
};
