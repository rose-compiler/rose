#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"
#include "DwarfLineMapper.h"

namespace Rose {
namespace BinaryAnalysis {

std::ostream &
operator<<(std::ostream &o, const DwarfLineMapper::SrcInfo &x)
{
    x.print(o);
    return o;
}

std::ostream &
operator<<(std::ostream &o, const DwarfLineMapper &x)
{
    x.print(o);
    return o;
}

void
DwarfLineMapper::init()
{
#ifndef ROSE_HAVE_LIBDWARF
    static bool called = false;
    if (!called) {
        std::cerr <<"DwarfLineMapper::DwarfLineMapper: warning: DWARF support is not enabled in ROSE\n";
        called = true;
    }
#endif
}

void
DwarfLineMapper::init(SgNode *ast, Direction d)
{
    init();
    p_direction = d;
    clear();
    insert(ast);
}

void
DwarfLineMapper::insert(SgNode *ast)
{
    traverse(ast, preorder);
}

void
DwarfLineMapper::clear()
{
    p_addr2src.clear();
    p_src2addr.clear();
    up_to_date = true;
}

void
DwarfLineMapper::update() const
{
    if (!up_to_date) {
        p_src2addr.clear();
        for (AddressSourceMap::const_iterator ai=p_addr2src.begin(); ai!=p_addr2src.end(); ++ai)
            p_src2addr[ai->second.get()].insert(ai->first);
        up_to_date = true;
    }
}

DwarfLineMapper::SrcInfo
DwarfLineMapper::addr2src(rose_addr_t addr) const
{
    AddressSourceMap::const_iterator found = p_addr2src.find(addr);
    return found==p_addr2src.end() ? SrcInfo() : found->second.get();
}

ExtentMap
DwarfLineMapper::src2addr(const SrcInfo &srcinfo) const
{
    update();
    SourceAddressMap::const_iterator found = p_src2addr.find(srcinfo);
    return found==p_src2addr.end() ? ExtentMap() : found->second;
}

rose_addr_t
DwarfLineMapper::src2first_addr(const SrcInfo &srcinfo) const
{
    ExtentMap ex = src2addr(srcinfo);
    return ex.empty() ? 0 : ex.min();
}

std::set<int>
DwarfLineMapper::all_files() const
{
    std::set<int> retval;
    for (AddressSourceMap::const_iterator i=p_addr2src.begin(); i!=p_addr2src.end(); ++i)
        retval.insert(i->second.get().file_id);
    return retval;
}

DwarfLineMapper::SrcInfo
DwarfLineMapper::next_src(const SrcInfo &srcinfo) const
{
    update();
    SourceAddressMap::const_iterator found = p_src2addr.upper_bound(srcinfo);
    return found==p_src2addr.end() ? SrcInfo() : found->first;
}

void
DwarfLineMapper::fix_holes(size_t max_hole_size)
{
    AddressSourceMap to_add; // entries that will fill in the holes
    AddressSourceMap::iterator cur = p_addr2src.begin();
    AddressSourceMap::iterator next = cur==p_addr2src.end() ? cur : ++cur;
    while (next!=p_addr2src.end()) {
        const Extent &e1 = cur->first;
        const Extent &e2 = next->first;
        if (e1.last()+1 < e2.first()) {
            size_t hole_size = e2.first() - (e1.last()+1);
            if (hole_size < max_hole_size)
                to_add.insert(Extent::inin(e1.last()+1, e2.first()-1), cur->second);
        }
        cur = next++;
    }
    p_addr2src.insert_ranges(to_add);
    if (!to_add.empty())
        up_to_date = false;
}

void
DwarfLineMapper::print_addr2src(std::ostream &o) const {
    for (AddressSourceMap::const_iterator ai=p_addr2src.begin(); ai!=p_addr2src.end(); ++ai) {
        const Extent &ex = ai->first;
        const SrcInfo &li = ai->second.get();
        o <<ex <<" => " <<li <<"\n";
    }
}

void
DwarfLineMapper::print_src2addr(std::ostream &o) const
{
    update();
    for (SourceAddressMap::const_iterator si=p_src2addr.begin(); si!=p_src2addr.end(); ++si) {
        const SrcInfo &li = si->first;
        const ExtentMap &ex = si->second;
        o <<li <<" => " <<ex <<"\n";
    }
}

void
DwarfLineMapper::print(std::ostream &o) const
{
    if (0!=(p_direction & ADDR2SRC))
        print_addr2src(o);
    if (0!=(p_direction & SRC2ADDR))
        print_src2addr(o);
}

void
DwarfLineMapper::visit(SgNode *node)
{
    if (SgAsmDwarfLineList *ll = isSgAsmDwarfLineList(node)) {
        const SgAsmDwarfLinePtrList &lines = ll->get_line_list();
        for (SgAsmDwarfLinePtrList::const_iterator li=lines.begin(); li!=lines.end(); ++li) {
            SgAsmDwarfLine *line = *li;
            p_addr2src.insert(Extent(line->get_address()), SrcInfo(line->get_file_id(), line->get_line()));
            up_to_date = false;
        }
    }
}

} // namespace
} // namespace

#endif
