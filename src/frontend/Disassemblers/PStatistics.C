/* Partitioner statistics */
#include "sage3basic.h"
#include <cmath>

#include "Partitioner.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>

#ifdef _MSC_VER
#include <float.h>                              // for _isnan
#define isnan(x) _isnan(x)
#include <boost/math/special_functions/erf.hpp> // for erf
using boost::math::erf;
#define _USE_MATH_DEFINES                       // for M_LN2
#include <math.h>
#else
using std::isnan;
#endif

/******************************************************************************************************************************
 *                                      RegionStats
 ******************************************************************************************************************************/

std::vector<Partitioner::RegionStats::DictionaryEntry> Partitioner::RegionStats::dictionary;

void
Partitioner::RegionStats::init_class()
{
    static bool initialized = false;
    if (!initialized) {
        //               NAME            DESCRIPTION                                 WEIGHT  ID
        define_analysis("nbytes",       "bytes in region",                              0.0, RA_NBYTES);
        define_analysis("ninsns",       "instructions disassembled",                    0.0, RA_NINSNS);
        define_analysis("ncoverage",    "bytes disassembled",                           0.0, RA_NCOVERAGE);
        define_analysis("rcoverage",    "ratio of ncoverage to nbytes",                 1.0, RA_RCOVERAGE);
        define_analysis("nstarts",      "times recursive disassembler started",         0.0, RA_NSTARTS);
        define_analysis("nfails",       "times recursive disassembler failed",          0.0, RA_NFAILS);
        define_analysis("rfails",       "ratio of nfails to nbytes",                    1.0, RA_RFAILS);
        define_analysis("noverlaps",    "instructions that overlap with previous",      0.0, RA_NOVERLAPS);
        define_analysis("roverlaps",    "ratio of noverlap to ninsns",                  1.0, RA_ROVERLAPS);
        define_analysis("nincomplete",  "instructions with unknown CFG edges",          0.0, RA_NINCOMPLETE);
        define_analysis("rincomplete",  "ratio of nincomplete to ninsns",               1.0, RA_RINCOMPLETE);
        define_analysis("nbranches",    "non fall-through branches",                    0.0, RA_NBRANCHES);
        define_analysis("rbranches",    "ratio of nbranches to ninsns",                 1.0, RA_RBRANCHES);
        define_analysis("ncalls",       "calls to previously known functions",          0.0, RA_NCALLS);
        define_analysis("rcalls",       "ratio of ncalls to nbranches",                 1.0, RA_RCALLS);
        define_analysis("nnoncalls",    "external branches to non-functions",           0.0, RA_NNONCALLS);
        define_analysis("rnoncalls",    "ratio of nnoncalls to nbranches",              1.0, RA_RNONCALLS);
        define_analysis("ninternal",    "branches to inside this region",               0.0, RA_NINTERNAL);
        define_analysis("rinternal",    "ratio of ninternal to nbranches",              1.0, RA_RINTERNAL);
        define_analysis("nicfgedges",   "total internal instruction CFG edges",         0.0, RA_NICFGEDGES);
        define_analysis("ricfgedges",   "ratio of nicfgedges to ninsns",                1.0, RA_RICFGEDGES);
        define_analysis("ncomps",       "connected components of internal CFG",         0.0, RA_NCOMPS);
        define_analysis("rcomps",       "ratio of ncomps to ninsns",                    1.0, RA_RCOMPS);
        define_analysis("niunique",     "unique instructions by kind",                  0.0, RA_NIUNIQUE);
        define_analysis("riunique",     "ratio of niunique to ninsns",                  1.0, RA_RIUNIQUE);
        define_analysis("nregrefs",     "register references",                          0.0, RA_NREGREFS);
        define_analysis("rregrefs",     "ratio of nregrefs to ninsns",                  1.0, RA_RREGREFS);
        define_analysis("regsz",        "mean register size as power of two",           1.0, RA_REGSZ);
        define_analysis("regvar",       "variance in regsz",                            1.0, RA_REGVAR);
        define_analysis("npriv",        "privileged instructions",                      0.0, RA_NPRIV);
        define_analysis("rpriv",        "ratio of npriv to ninsns",                     1.0, RA_RPRIV);
        define_analysis("nfloat",       "floating-point instructions",                  0.0, RA_NFLOAT);
        define_analysis("rfloat",       "ratio of nfloat to ninsns",                    1.0, RA_RFLOAT);
        initialized = true;
    }
}

Partitioner::RegionStats *
Partitioner::RegionStats::create() const
{
    return new RegionStats();
}

size_t
Partitioner::RegionStats::define_analysis(const std::string &name, const std::string &desc, double weight, size_t id)
{
    if (-1==(ssize_t)id)
        id = dictionary.size();
    assert((ssize_t)id >= 0);

    dictionary.resize(id+1);
    assert(dictionary[id].name.empty() || dictionary[id].name==name);
    dictionary[id].name = name;
    dictionary[id].desc = desc;
    dictionary[id].weight = weight;
    return id;
}

size_t
Partitioner::RegionStats::find_analysis(const std::string &name)
{
    for (size_t id=0; id<dictionary.size(); ++id) {
        if (dictionary[id].name == name)
            return id;
    }
    return (size_t)(-1);
}

size_t
Partitioner::RegionStats::get_nanalyses()
{
    return dictionary.size();
}

const std::string &
Partitioner::RegionStats::get_name(size_t id)
{
    assert(id<dictionary.size());
    return dictionary[id].name;
}

const std::string &
Partitioner::RegionStats::get_desc(size_t id)
{
    assert(id<dictionary.size());
    return dictionary[id].desc;
}

double
Partitioner::RegionStats::get_weight(size_t id)
{
    assert(id<dictionary.size());
    return dictionary[id].weight;
}

void
Partitioner::RegionStats::add_sample(size_t id, double x, size_t n)
{
    assert(id<dictionary.size());
    results.resize(dictionary.size());
    if (!isnan(x)) {
        results[id].sum += x;
        results[id].nsamples += n;
    }
}

void
Partitioner::RegionStats::add_samples(const RegionStats *x)
{
    assert(x!=NULL);
    for (size_t id=0; id<dictionary.size(); ++id)
        add_sample(id, x->get_sum(id), x->get_nsamples(id));
}

void
Partitioner::RegionStats::square_diff(const RegionStats *x)
{
    assert(x!=NULL);
    for (size_t id=0; id<dictionary.size(); ++id) {
        double v1 = get_value(id);
        double v2 = x->get_value(id);
        set_value(id, (v1-v2)*(v1-v2));
    }
}

size_t
Partitioner::RegionStats::get_nsamples(size_t id) const
{
    assert(id<dictionary.size());
    return id<results.size() ? results[id].nsamples : 0;
}

double
Partitioner::RegionStats::get_sum(size_t id) const
{
    assert(id<dictionary.size());
    return id<results.size() ? results[id].sum : 0.0;
}

double
Partitioner::RegionStats::get_value(size_t id) const
{
    assert(id<dictionary.size());
    return id>=results.size() || 0==results[id].nsamples ? NAN : results[id].sum / results[id].nsamples;
}

void
Partitioner::RegionStats::set_value(size_t id, double value)
{
    assert(id<dictionary.size());
    results.resize(dictionary.size());
    if (isnan(value)) {
        results[id].sum = 0;
        results[id].nsamples = 0;
    } else {
        results[id].sum = value;
        results[id].nsamples = 1;
    }
}

double
Partitioner::RegionStats::divnan(size_t num_id, size_t den_id) const
{
    double num = get_value(num_id);
    double den = get_value(den_id);

    if (isnan(num) || isnan(den) || 0.0==den)
        return NAN;
    return num/den;
}

void
Partitioner::RegionStats::compute_ratios()
{
    set_value(RA_RCOVERAGE,   divnan(RA_NCOVERAGE,   RA_NBYTES));
    set_value(RA_RFAILS,      divnan(RA_NFAILS,      RA_NBYTES));

    set_value(RA_RCOMPS,      divnan(RA_NCOMPS,      RA_NINSNS));
    set_value(RA_ROVERLAPS,   divnan(RA_NOVERLAPS,   RA_NINSNS));
    set_value(RA_RINCOMPLETE, divnan(RA_NINCOMPLETE, RA_NINSNS));
    set_value(RA_RBRANCHES,   divnan(RA_NBRANCHES,   RA_NINSNS));
    set_value(RA_RICFGEDGES,  divnan(RA_NICFGEDGES,  RA_NINSNS));
    set_value(RA_RIUNIQUE,    divnan(RA_NIUNIQUE,    RA_NINSNS));
    set_value(RA_RREGREFS,    divnan(RA_NREGREFS,    RA_NINSNS));
    set_value(RA_RPRIV,       divnan(RA_NPRIV,       RA_NINSNS));
    set_value(RA_RFLOAT,      divnan(RA_NFLOAT,      RA_NINSNS));

    set_value(RA_RCALLS,      divnan(RA_NCALLS,      RA_NBRANCHES));
    set_value(RA_RNONCALLS,   divnan(RA_NNONCALLS,   RA_NBRANCHES));
    set_value(RA_RINTERNAL,   divnan(RA_NINTERNAL,   RA_NBRANCHES));
}

void
Partitioner::RegionStats::print(std::ostream &o) const
{
    std::ios_base::fmtflags orig = o.flags();
    o <<"  "
      <<std::setw(16) <<std::left  <<"Name"        <<" "
      <<std::setw(40) <<std::left  <<"Description" <<" "
      <<std::setw(11) <<std::right <<"Weight"      <<" "
      <<std::setw(11) <<std::right <<"Sum"         <<" "
      <<std::setw(11) <<std::right <<"NSamples"    <<" "
      <<std::setw(11) <<std::right <<"Value"       <<"\n";
    for (size_t id=0; id<dictionary.size(); ++id) {
        o <<"  "
          <<std::setw(16) <<std::left  <<dictionary[id].name   <<" "
          <<std::setw(40) <<std::left  <<dictionary[id].desc   <<" "
          <<std::setw(11) <<std::right <<dictionary[id].weight <<" "
          <<std::setw(11) <<std::right <<results[id].sum       <<" "
          <<std::setw(11) <<std::right <<results[id].nsamples  <<" "
          <<std::setw(11) <<std::right <<get_value(id)         <<"\n";
    }
    o.flags(orig);
}

std::ostream &
operator<<(std::ostream &o, const Partitioner::RegionStats &stats)
{
    stats.print(o);
    return o;
}


/******************************************************************************************************************************
 *                                      CodeCriteria
 ******************************************************************************************************************************/

std::vector<Partitioner::CodeCriteria::DictionaryEntry> Partitioner::CodeCriteria::dictionary;

void
Partitioner::CodeCriteria::init_class()
{}

void
Partitioner::CodeCriteria::init(const RegionStats *mean, const RegionStats *variance, double threshold)
{
    assert(mean!=NULL && variance!=NULL);
    assert(mean->get_nanalyses()==variance->get_nanalyses());
    this->threshold = threshold;
    size_t n = mean->get_nanalyses();
    for (size_t analysis_id=0; analysis_id<n; ++analysis_id) {
        if (!mean->get_name(analysis_id).empty() && mean->get_weight(analysis_id)>0.0) {
            /* Make sure the CodeCriteria class knows about all the current region statistics analyses */
            size_t criterion_id = find_criterion(mean->get_name(analysis_id));
            if (-1==(ssize_t)criterion_id)
                criterion_id = define_criterion(mean->get_name(analysis_id), mean->get_desc(analysis_id));

            /* Initialize the criteria */
            criteria.resize(criterion_id+1);
            criteria[criterion_id].mean = mean->get_value(analysis_id);
            criteria[criterion_id].variance = variance->get_value(analysis_id);
            criteria[criterion_id].weight = mean->get_weight(analysis_id);
        }
    }
}

Partitioner::CodeCriteria *
Partitioner::CodeCriteria::create() const
{
    return new CodeCriteria;

}

size_t
Partitioner::CodeCriteria::define_criterion(const std::string &name, const std::string &desc, size_t id)
{
    if (-1==(ssize_t)id)
        id = dictionary.size();
    assert((ssize_t)id >= 0);

    dictionary.resize(id+1);
    assert(dictionary[id].name.empty() || dictionary[id].name==name);
    dictionary[id].name = name;
    dictionary[id].desc = desc;
    return id;
}

size_t
Partitioner::CodeCriteria::find_criterion(const std::string &name)
{
    for (size_t id=0; id<dictionary.size(); ++id) {
        if (dictionary[id].name == name)
            return id;
    }
    return (size_t)(-1);
}

size_t
Partitioner::CodeCriteria::get_ncriteria()
{
    return dictionary.size();
}

const std::string &
Partitioner::CodeCriteria::get_name(size_t id)
{
    assert(id<dictionary.size());
    return dictionary[id].name;
}

const std::string &
Partitioner::CodeCriteria::get_desc(size_t id)
{
    assert(id<dictionary.size());
    return dictionary[id].desc;
}

double
Partitioner::CodeCriteria::get_mean(size_t id) const
{
    assert(id<dictionary.size());
    return id>=criteria.size() ? NAN : criteria[id].mean;
}

void
Partitioner::CodeCriteria::set_mean(size_t id, double mean)
{
    assert(id<dictionary.size());
    criteria.resize(dictionary.size());
    criteria[id].mean = mean;
}

double
Partitioner::CodeCriteria::get_variance(size_t id) const
{
    assert(id<dictionary.size());
    return id>=criteria.size() ? NAN : criteria[id].variance;
}

void
Partitioner::CodeCriteria::set_variance(size_t id, double variance)
{
    assert(id<dictionary.size());
    criteria.resize(dictionary.size());
    criteria[id].variance = variance;
}

double
Partitioner::CodeCriteria::get_weight(size_t id) const
{
    assert(id<dictionary.size());
    return id>=criteria.size() ? NAN : criteria[id].weight;
}

void
Partitioner::CodeCriteria::set_weight(size_t id, double weight)
{
    assert(id<dictionary.size());
    criteria.resize(dictionary.size());
    criteria[id].weight = weight;
}

double
Partitioner::CodeCriteria::get_vote(const RegionStats *stats, std::vector<double> *votes) const
{
    if (votes!=NULL)
        votes->resize(dictionary.size(), NAN);

    double sum=0.0, total_wt=0.0;
    for (size_t cc_id=0; cc_id<criteria.size(); ++cc_id) {
        if (criteria[cc_id].weight <= 0.0)
            continue;

        size_t stat_id = stats->find_analysis(get_name(cc_id));
        if (-1==(ssize_t)stat_id)
            continue;

        double stat_val = stats->get_value(stat_id);
        if (!isnan(stat_val)) {
            double c = 0.0==criteria[cc_id].variance ?
                       (stat_val==criteria[cc_id].mean ? 1.0 : 0.0) :
                       1 + erf(-fabs(stat_val-criteria[cc_id].mean) / sqrt(2*criteria[cc_id].variance));
            if (votes)
                (*votes)[cc_id] = c;
            if (!isnan(c)) {
                sum += c * criteria[cc_id].weight;
                total_wt += criteria[cc_id].weight;
            }
        }
    }
    return total_wt>0.0 ? sum / total_wt : NAN;
}

bool
Partitioner::CodeCriteria::satisfied_by(const RegionStats *stats, double *raw_vote_ptr, std::ostream *debug) const
{
    std::vector<double> votes;
    std::vector<double> *votes_ptr = debug ? &votes : NULL;
    double vote = get_vote(stats, votes_ptr);
    if (raw_vote_ptr)
        *raw_vote_ptr = vote;
    if (debug)
        print(*debug, stats, votes_ptr, &vote);
    return vote >= threshold;
}

void
Partitioner::CodeCriteria::print(std::ostream &o, const RegionStats *stats, const std::vector<double> *votes,
                                 const double *total_vote) const
{
    std::ios_base::fmtflags oldflags = o.flags();

    /* Get the list of data member names (dups ok here) */
    std::vector<std::string> names;
    if (stats) {
        for (size_t id=0; id<stats->get_nanalyses(); ++id)
            names.push_back(stats->get_name(id));
    }
    for (size_t id=0; id<dictionary.size(); ++id)
        names.push_back(dictionary[id].name);
    if (names.empty())
        return;

    /* Column headers */
    o <<"  " <<std::setw(16) <<std::left  <<"Name"
      <<" "  <<std::setw(40) <<std::left  <<"Description"
      <<" "  <<std::setw(11) <<std::right <<"Mean"
      <<" "  <<std::setw(11) <<std::right <<"Variance"
      <<" "  <<std::setw(11) <<std::right <<"Weight";
    if (stats)
        o <<" " <<std::setw(11) <<std::right <<"Stats";
    if (votes)
        o <<" " <<std::setw(4)  <<std::right <<"Vote";
    o <<"\n";

    /* Process names in the order we found them */
    std::set<std::string> processed;
    for (size_t i=0; i<names.size(); i++) {

        /* Avoid duplicate lines */
        if (processed.find(names[i])!=processed.end())
            continue;
        processed.insert(names[i]);

        size_t cc_id = find_criterion(names[i]);
        size_t stat_id = stats ? stats->find_analysis(names[i]) : (size_t)(-1);
        assert(-1!=(ssize_t)cc_id || -1!=(ssize_t)stat_id);

        /* Name and description, prefering criterion to stats */
        std::string desc = -1!=(ssize_t)cc_id ? get_desc(cc_id) : stats->get_desc(stat_id);
        o <<"  " <<std::setw(16) <<std::left  <<names[i]
          <<" "  <<std::setw(40) <<std::left  <<desc;

        /* Mean, Variance, and Weight criterion members */
        if (-1!=(ssize_t)cc_id) {
            o <<" "  <<std::setw(11) <<std::right <<criteria[cc_id].mean
              <<" "  <<std::setw(11) <<std::right <<criteria[cc_id].variance
              <<" "  <<std::setw(11) <<std::right <<criteria[cc_id].weight;
        } else {
            o <<" "  <<std::setw(11) <<std::right <<""
              <<" "  <<std::setw(11) <<std::right <<""
              <<" "  <<std::setw(11) <<std::right <<stats->get_weight(stat_id);
        }

        /* Statistics value optional column */
        if (-1!=(ssize_t)stat_id) {
            o <<" "  <<std::setw(11) <<std::right <<stats->get_value(stat_id);
        } else if (stats) {
            o <<" "  <<std::setw(11) <<std::right <<"none";
        }

        /* Vote optional column */
        if (votes) {
            if (-1==(ssize_t)cc_id || isnan((*votes)[cc_id])) {
                o <<" " <<std::setw(5) <<std::right <<"";
            } else {
                o <<" " <<std::setw(4)  <<std::right <<floor(100.0*(*votes)[cc_id]+0.5) <<"%";
            }
        }

        o <<"\n";
    }

    /* Average vote */
    if (votes && total_vote) {
        o <<"  " <<std::setw(16) <<std::left  <<"voteave"
          <<" "  <<std::setw(40) <<std::left  <<"average of all votes"
          <<" "  <<std::setw(11)              <<""
          <<" "  <<std::setw(11)              <<""
          <<" "  <<std::setw(11)              <<"";
        if (stats)
            o <<" " <<std::setw(11) <<"";
        if (isnan(*total_vote)) {
            o <<" " <<std::setw(5) <<std::right <<"";
        } else {
            o <<" "  <<std::setw(4)  <<std::right <<floor(100.0*(*total_vote)+0.5) <<"%";
        }
        o <<"\n";
    }

    /* Threshold and final Boolean answer */
    o <<"  " <<std::setw(16) <<std::left  <<"threshold"
      <<" "  <<std::setw(40) <<std::left  <<"total vote required for YES result"
      <<" "  <<std::setw(11)              <<""
      <<" "  <<std::setw(11)              <<""
      <<" "  <<std::setw(11) <<std::right <<threshold;
    if (stats)
        o <<" " <<std::setw(11) <<"";
    if (votes && total_vote) {
        if (isnan(*total_vote)) {
            o <<" " <<std::setw(5) <<std::right <<"NaN";
        } else {
            o <<" "  <<std::setw(5)  <<std::right <<(*total_vote>=threshold?"YES":"NO");
        }
    }
    o <<"\n";

    o.flags(oldflags);
}

std::ostream &
operator<<(std::ostream &o, const Partitioner::CodeCriteria &cc)
{
    cc.print(o);
    return o;
}



/******************************************************************************************************************************
 *                                      Partitioner methods
 ******************************************************************************************************************************/

/* FIXME: Stupid SgAsmInstruction interface necessitates a "switch". [RPM 2011-11-11] */
size_t
Partitioner::count_kinds(const InstructionMap &insns)
{
    std::set<int> kinds;
    for (InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
        SgAsmInstruction *insn = ii->second->node;
        int kind = -1;
        switch (insn->variantT()) {
            case V_SgAsmx86Instruction:     kind = isSgAsmx86Instruction(insn)    ->get_kind(); break;
            case V_SgAsmPowerpcInstruction: kind = isSgAsmPowerpcInstruction(insn)->get_kind(); break;
            case V_SgAsmArmInstruction:     kind = isSgAsmArmInstruction(insn)    ->get_kind(); break;
            default: break; // to shut up compiler warnings
        }
        if (-1!=kind)
            kinds.insert(kind);
    }
    return kinds.size();
}

/* FIXME: Stupid SgAsmInstruction interface necessitates a "switch". [RPM 2011-11-11] */
size_t
Partitioner::count_privileged(const InstructionMap &insns)
{
    size_t retval = 0;
    for (InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
        SgAsmInstruction *insn = ii->second->node;
        switch (insn->variantT()) {
            case V_SgAsmx86Instruction:
                if (x86InstructionIsPrivileged(isSgAsmx86Instruction(insn)))
                    ++retval;
                break;
            default:    // to shut up compiler warnings
                break;
        }
    }
    return retval;
}

/* FIXME: Stupid SgAsmInstruction interface necessitates a "switch". [RPM 2011-11-11] */
size_t
Partitioner::count_floating_point(const InstructionMap &insns)
{
    size_t retval = 0;
    for (InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
        SgAsmInstruction *insn = ii->second->node;
        switch (insn->variantT()) {
            case V_SgAsmx86Instruction:
                if (x86InstructionIsFloatingPoint(isSgAsmx86Instruction(insn)))
                    ++retval;
                break;
            default:    // to shut up compiler warnings
                break;
        }
    }
    return retval;
}

size_t
Partitioner::count_registers(const InstructionMap &insns, double *mean_ptr, double *variance_ptr)
{
    struct T1: public AstSimpleProcessing {
        long n;
        bool do_variance;
        double sum, mean;
        T1(): n(0), do_variance(false), sum(0.0), mean(0.0) {}
        T1(double mean): n(0), do_variance(true), sum(0.0), mean(mean) {}
        void visit(SgNode *node) {
            SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(node);
            if (rre) {
                size_t nbits = rre->get_descriptor().get_nbits();
                double v = log((double)nbits) / M_LN2;
                sum += do_variance ? (v-mean)*(v-mean) : v;
                ++n;
            }
        }
    };

    T1 mean;
    for (InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii)
        mean.traverse(ii->second->node, preorder);

    T1 variance(mean.sum/mean.n);
    if (variance_ptr) {
        for (InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii)
            variance.traverse(ii->second->node, preorder);
    }

    /*results*/
    if (mean_ptr)
        *mean_ptr = variance.mean;
    if (variance_ptr)
        *variance_ptr = variance.sum / variance.n;

    return mean.n;
}

/* FIXME: Only works for x86 instructions. [RPM 2011-11-11] */
double
Partitioner::count_size_variance(const InstructionMap &insns)
{
    double mean=NAN, variance=NAN;
    for (size_t pass=0; pass<2; ++pass) {
        double sum=0.0;
        int npoints=0;
        for (InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
            SgAsmx86Instruction *insn_x86 = isSgAsmx86Instruction(ii->second);
            if (insn_x86) {
                npoints += 3;
                for (size_t i=0; i<3; i++) {
                    X86InstructionSize sz;
                    switch (i) {
                        case 0:
                            sz = insn_x86->get_baseSize();
                            break;
                        case 1:
                            sz = insn_x86->get_operandSize();
                            break;
                        case 2:
                            sz = insn_x86->get_addressSize();
                            break;
                    }
                    int point;
                    switch (sz) {
                        case x86_insnsize_16:
                            point = 0;
                            break;
                        case x86_insnsize_32:
                            point = 1;
                            break;
                        case x86_insnsize_64:
                            point = 2;
                            break;
                        default:
                            point = 3;
                            break;
                    }
                    switch (pass) {
                        case 0:
                            sum += point;
                            break;
                        case 1:
                            sum += (point-mean)*(point-mean);
                            break;
                    }
                }
            }
        }

        if (0==npoints)
            return NAN;
        switch (pass) {
            case 0:
                mean = (double)sum / npoints;
                break;
            case 1:
                variance = (double)sum / npoints;
                break;
        }
    }
    return variance;
}

Partitioner::RegionStats *
Partitioner::region_statistics()
{
    MemoryMap mymap = *map;
    mymap.prune(MemoryMap::MM_PROT_EXEC);
    return region_statistics(mymap.va_extents());
}

Partitioner::RegionStats *
Partitioner::region_statistics(Function *func)
{
    assert(func);
    FunctionRangeMap f_extent;
    function_extent(func, &f_extent);
    return region_statistics(f_extent);
}

Partitioner::RegionStats *
Partitioner::region_statistics(const ExtentMap &addresses)
{
    RegionStats *stats = new_region_stats();
    assert(stats!=NULL);
    size_t nbytes = addresses.size();
    if (0==nbytes)
        return stats;
    stats->add_sample(RegionStats::RA_NBYTES, nbytes);
    ExtentMap not_addresses = addresses.invert<ExtentMap>();

    Disassembler::AddressSet worklist;          // addresses waiting to be disassembled recursively
    InstructionMap insns_found;                 // all the instructions we found herein
    ExtentMap insns_extent;                     // memory used by the instructions we've found
    ExtentMap pending = addresses;              // addresses we haven't looked at yet

    /* Undirected local control flow graph used to count connected components */
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> CFG;
    typedef boost::graph_traits<CFG>::vertex_descriptor CFGVertex;
    typedef std::map<rose_addr_t, CFGVertex> Addr2Vertex;
    CFG cfg;
    Addr2Vertex va2id;

    /* Statistics */
    size_t nstarts=0;                           // number of times the recursive disassembler was started
    size_t nfails=0;                            // number of disassembler failures
    size_t noverlaps=0;                         // instructions overlapping with a previously found instruction
    size_t nincomplete=0;                       // number of instructions with unknown successors
    size_t nfallthrough=0;                      // number of branches to fall-through address within our "addresses"
    size_t ncalls=0;                            // number of function calls outside our "addresses"
    size_t nnoncalls=0;                         // number of branches to non-functions outside our "addresses"
    size_t ninternal=0;                         // number of non-fallthrough internal branches

    while (!pending.empty()) {
        rose_addr_t start_va = pending.min();
        worklist.insert(start_va);
        ++nstarts;

        while (!worklist.empty()) {
            rose_addr_t va = *worklist.begin();
            worklist.erase(worklist.begin());

            /* Obtain (disassemble) the instruction and make sure it falls entirely within the "addresses" */
            Instruction *insn = find_instruction(va);
            if (!insn) {
                ++nfails;
                pending.erase(Extent(va));
                continue;
            }
            Extent ie(va, insn->get_size());
            if (not_addresses.overlaps(ie)) {
                ++nfails;
                pending.erase(Extent(va));
                continue;
            }

            /* The disassembler can also return an "unknown" instruction when failing, depending on how it is invoked. */
            if (insn->node->is_unknown()) {
                ++nfails;
                pending.erase(Extent(va, insn->get_size()));
                continue;
            }

            insns_found.insert(std::make_pair(va, insn));
            rose_addr_t fall_through_va = va + insn->get_size();

            /* Does this instruction overlap with any we've already found? */
            if (insns_extent.overlaps(ie))
                ++noverlaps;
            pending.erase(Extent(va, insn->get_size()));
            insns_extent.insert(ie);

            /* Find instruction successors by looking only at the instruction itself.  This is simpler, but less rigorous
             * method than finding successors a basic block at a time.  For instance, we'll find both sides of a branch
             * instruction even if the more rigorous method determined that one side or the other is always taken.  But this is
             * probably what we want here anyway for determining whether something looks like code. */
            bool complete;
            Disassembler::AddressSet succs = insn->get_successors(&complete);
            if (!complete)
                ++nincomplete;

            /* Add instruction as vertex to CFG */
            std::pair<Addr2Vertex::iterator, bool> inserted = va2id.insert(std::make_pair(va, va2id.size()));
            if (inserted.second) {
                CFGVertex vertex __attribute__((unused)) = add_vertex(cfg);
                assert(vertex==inserted.first->second);
            }

            /* Classify the various successors. */
            for (Disassembler::AddressSet::const_iterator si=succs.begin(); si!=succs.end(); ++si) {
                rose_addr_t succ_va = *si;


                if (succ_va==fall_through_va) {
                    ++nfallthrough;
                    if (pending.find(succ_va)!=pending.end())
                        worklist.insert(succ_va);
                    /* Add edge to CFG graph */
                    va2id.insert(std::make_pair(succ_va, va2id.size()));
                    add_edge(va2id[va], va2id[succ_va], cfg);
                } else if (addresses.find(succ_va)==addresses.end()) {
                    /* A non-fallthrough branch to something outside this memory region */
                    if (functions.find(succ_va)!=functions.end()) {
                        /* A branch to a function entry point we've previously discovered. */
                        ++ncalls;
                    } else {
                        ++nnoncalls;
                    }
                } else {
                    /* A non-fallthrough branch to something in our address range. */
                    ++ninternal;
                    if (pending.find(succ_va)!=pending.end())
                        worklist.insert(succ_va);

                    /* Add edge to CFG graph */
                    va2id.insert(std::make_pair(succ_va, va2id.size()));
                    add_edge(va2id[va], va2id[succ_va], cfg);
                }
            }
        }
    }

    /* Statistics */
    stats->add_sample(RegionStats::RA_NFAILS, nfails);
    stats->add_sample(RegionStats::RA_NINSNS, insns_found.size());
    stats->add_sample(RegionStats::RA_NOVERLAPS, noverlaps);
    stats->add_sample(RegionStats::RA_NSTARTS, nstarts);
    stats->add_sample(RegionStats::RA_NCOVERAGE, insns_extent.size());
    stats->add_sample(RegionStats::RA_NINCOMPLETE, nincomplete);
    stats->add_sample(RegionStats::RA_NBRANCHES, ncalls+nnoncalls+ninternal);
    stats->add_sample(RegionStats::RA_NCALLS, ncalls);
    stats->add_sample(RegionStats::RA_NNONCALLS, nnoncalls);
    stats->add_sample(RegionStats::RA_NINTERNAL, ninternal);
    stats->add_sample(RegionStats::RA_NICFGEDGES, ninternal + nfallthrough);
    stats->add_sample(RegionStats::RA_NIUNIQUE, count_kinds(insns_found));
    stats->add_sample(RegionStats::RA_NPRIV, count_privileged(insns_found));
    stats->add_sample(RegionStats::RA_NFLOAT, count_floating_point(insns_found));
    double regsz, regvar;
    stats->add_sample(RegionStats::RA_NREGREFS, count_registers(insns_found, &regsz, &regvar));
    stats->add_sample(RegionStats::RA_REGSZ, regsz);
    stats->add_sample(RegionStats::RA_REGVAR, regvar);

    /* Count the number of connected components in the undirected CFG */
    if (!va2id.empty()) {
        std::vector<int> component(num_vertices(cfg));
        stats->add_sample(RegionStats::RA_NCOMPS, connected_components(cfg, &component[0]));
    }

    stats->compute_ratios();
    return stats;
}

Partitioner::RegionStats *
Partitioner::aggregate_statistics(bool do_variance)
{
    /* Return old values if we have them, or else start over for both. */
    if (aggregate_mean!=NULL && (!do_variance || aggregate_variance!=NULL))
        return aggregate_mean;
    clear_aggregate_statistics();
    aggregate_mean = new_region_stats();
    aggregate_variance = new_region_stats();
    if (functions.empty())
        return aggregate_mean;

    /* Average */
    std::vector<RegionStats*> stats;
    for (Functions::iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
        Function *func = fi->second;
        if (0!=(func->reason & (SgAsmFunction::FUNC_PADDING | SgAsmFunction::FUNC_LEFTOVERS | SgAsmFunction::FUNC_THUNK)))
            continue;
        RegionStats *s = region_statistics(func);
        stats.push_back(s);
        aggregate_mean->add_samples(s);
    }
    aggregate_mean->compute_ratios();

    /* Variance */
    if (do_variance) {
        for (size_t i=0; i<stats.size(); ++i) {
            stats[i]->square_diff(aggregate_mean);
            aggregate_variance->add_samples(stats[i]);
        }
    }

    /* Cleanup */
    for (size_t i=0; i<stats.size(); ++i)
        delete stats[i];
    return aggregate_mean;
}

bool
Partitioner::is_code(const ExtentMap &region, double *raw_vote_ptr, std::ostream *debug)
{
    bool retval = false;

    CodeCriteria *cc = code_criteria;
    if (!cc) {
        /* Use cached values if possible. */
        RegionStats *mean = aggregate_statistics();
        RegionStats *variance = get_aggregate_variance();
        cc = new_code_criteria(mean, variance, 0.5);
    }

    /* Compare region analysis results with aggregate statistics */
    RegionStats *stats = region_statistics(region);
    retval = cc->satisfied_by(stats, raw_vote_ptr, debug);
    delete stats;
    if (!code_criteria)
        delete cc;

    return retval;
}
