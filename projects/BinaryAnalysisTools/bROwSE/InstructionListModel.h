#ifndef bROwSE_InstructionListModel_H
#define bROwSE_InstructionListModel_H

#include <bROwSE/bROwSE.h>
#include <Wt/WAbstractTableModel>

namespace bROwSE {

class InstructionListModel: public Wt::WAbstractTableModel {
    Context &ctx_;
    std::vector<SgAsmInstruction*> insns_;
public:
    enum Column {
        C_ADDR,                                         // instruction address
        C_BYTES,                                        // instruction bytes in hexadecimal
        C_CHARS,                                        // instruction printable characters
        C_STACKDELTA,                                   // stack delta
        C_NAME,                                         // instruction mnemonic
        C_ARGS,                                         // instruction operands
        C_COMMENT,                                      // arbitrary text
        C_NCOLS                                         // MUST BE LAST
    };

    explicit InstructionListModel(Context &ctx): ctx_(ctx) {}

    void changeInstructions(const std::vector<SgAsmInstruction*> &insns);

    void clear();
    
    virtual int rowCount(const Wt::WModelIndex &parent=Wt::WModelIndex()) const ROSE_OVERRIDE;

    virtual int columnCount(const Wt::WModelIndex &parent=Wt::WModelIndex()) const ROSE_OVERRIDE;

    virtual boost::any headerData(int column, Wt::Orientation orientation=Wt::Horizontal,
                                  int role=Wt::DisplayRole) const ROSE_OVERRIDE;

    virtual boost::any data(const Wt::WModelIndex &index, int role=Wt::DisplayRole) const ROSE_OVERRIDE;
};

} // namespace
#endif
