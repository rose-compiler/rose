/* Functions for determining how feasible it is that a region of memory is code rather than data. */
#include "rose.h"
#include "stringify.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cmath>

using namespace rose::BinaryAnalysis;

static double sigma(double variance) { return variance*variance; }

/* Reasonable mean and variance for statistics calculated over the entire executable region. */
static void
executable_region_stats(Partitioner::RegionStats &mean, Partitioner::RegionStats &variance)
{
    Partitioner::RegionStats &m=mean, &v=variance;

    m.add_sample(Partitioner::RegionStats::RA_RCOVERAGE,        0.9999);
    v.add_sample(Partitioner::RegionStats::RA_RCOVERAGE,        sigma(0.001));

    m.add_sample(Partitioner::RegionStats::RA_RFAILS,           0.0001);
    v.add_sample(Partitioner::RegionStats::RA_RFAILS,           sigma(0.001));

    m.add_sample(Partitioner::RegionStats::RA_ROVERLAPS,        0.004);
    v.add_sample(Partitioner::RegionStats::RA_ROVERLAPS,        sigma(0.001));

    m.add_sample(Partitioner::RegionStats::RA_RINCOMPLETE,      0.01);
    v.add_sample(Partitioner::RegionStats::RA_RINCOMPLETE,      sigma(0.03));

    m.add_sample(Partitioner::RegionStats::RA_RBRANCHES,        0.08);
    v.add_sample(Partitioner::RegionStats::RA_RBRANCHES,        sigma(0.05));

    //m.add_sample(Partitioner::RegionStats::RA_RCALLS,         0);             // no functions in raw buffers, so don't vote
    //v.add_sample(Partitioner::RegionStats::RA_RCALLS,         0);             // based on this analysis

    m.add_sample(Partitioner::RegionStats::RA_RNONCALLS,        0.025);         // external branches would be outside the whole
    v.add_sample(Partitioner::RegionStats::RA_RNONCALLS,        sigma(0.01));   // program address space, so there aren't many

    m.add_sample(Partitioner::RegionStats::RA_RINTERNAL,        0.9975);        // conversely, most branches should be inside
    v.add_sample(Partitioner::RegionStats::RA_RINTERNAL,        0.00010);       // the whole address space

    m.add_sample(Partitioner::RegionStats::RA_RICFGEDGES,       0.963);
    v.add_sample(Partitioner::RegionStats::RA_RICFGEDGES,       0.350);

    m.add_sample(Partitioner::RegionStats::RA_RCOMPS,           0.0004);
    v.add_sample(Partitioner::RegionStats::RA_RCOMPS,           sigma(0.0001));

    //m.add_sample(Partitioner::RegionStats::RA_RIUNIQUE,         0);           // FIXME: sensitive to total program size
    //v.add_sample(Partitioner::RegionStats::RA_RIUNIQUE,         0);

    m.add_sample(Partitioner::RegionStats::RA_RREGREFS,         2.5);
    v.add_sample(Partitioner::RegionStats::RA_RREGREFS,         sigma(0.5));

    m.add_sample(Partitioner::RegionStats::RA_REGSZ,            4.780);         // assuming 32-bit code
    v.add_sample(Partitioner::RegionStats::RA_REGSZ,            0.011);

    m.add_sample(Partitioner::RegionStats::RA_REGVAR,           0.174);
    v.add_sample(Partitioner::RegionStats::RA_REGVAR,           0.008);     // disassembling data.

    m.add_sample(Partitioner::RegionStats::RA_RPRIV,            0.00026);
    v.add_sample(Partitioner::RegionStats::RA_RPRIV,            0.00001);

    m.add_sample(Partitioner::RegionStats::RA_RFLOAT,           0.0016);
    v.add_sample(Partitioner::RegionStats::RA_RFLOAT,           0.0002);
}


int
main(int argc, char *argv[])
{
    assert(argc>1);
    std::string filename = argv[1];

    rose_addr_t start_va = 0;
    MemoryMap map;
    MemoryMap::Buffer::Ptr buffer = MemoryMap::MappedBuffer::instance(filename);
    map.insert(AddressInterval::baseSize(start_va, buffer->size()),
               MemoryMap::Segment(buffer, 0, MemoryMap::READABLE|MemoryMap::EXECUTABLE, filename));

    SgAsmGenericHeader *fake_header = new SgAsmPEFileHeader(new SgAsmGenericFile());
    Disassembler *disassembler = Disassembler::lookup(fake_header)->clone();
    disassembler->set_search(Disassembler::SEARCH_DEFAULT | Disassembler::SEARCH_DEADEND | Disassembler::SEARCH_UNUSED);
    Disassembler::AddressSet worklist;
    worklist.insert(start_va);
    Disassembler::BadMap bad;
    Disassembler::InstructionMap insns = disassembler->disassembleBuffer(&map, worklist, NULL, &bad);

    Partitioner *partitioner = new Partitioner();
    partitioner->add_instructions(insns);

    MemoryMap map2 = map;
    map2.require(MemoryMap::EXECUTABLE).keep();
    AddressIntervalSet extent(map);
    Partitioner::RegionStats *stats = partitioner->region_statistics(toExtentMap(extent));

    /* Initialize a code criteria object with some reasonable values. */
    Partitioner::RegionStats m, v;
    executable_region_stats(m, v);
    Partitioner::CodeCriteria *cc = partitioner->new_code_criteria(&m, &v, 0.5);
    
    double vote;
    cc->satisfied_by(stats, &vote, &std::cout);
    if (std::isnan(vote)) {
        std::cout <<"Code probability could not be determined.\n";
    } else {
        std::cout <<"There is a " <<floor(100.0*vote+0.5) <<"% probability that this file is x86 code.\n";
    }

    return 0;
}

