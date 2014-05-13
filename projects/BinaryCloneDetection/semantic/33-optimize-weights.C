#include "rose.h"
#include "rose_getline.h"
#include "LinearCongruentialGenerator.h"
#include "Map.h"

#include <boost/thread.hpp>
#include <cerrno>

#undef DEBUGGING

static std::string argv0;

static void usage(int exit_status) {
    std::cerr <<"SYNOPSIS\n"
              <<"    " <<argv0 <<" [SWITCHES]\n"
              <<"\n"
              <<"SWITCHES\n"
              <<"    --population=N\n"
              <<"        Number of individuals for each generation of the genetic algorithm.\n"
              <<"    --generations=N\n"
              <<"        Maximum number of generations before giving up.  Zero means that the best solution\n"
              <<"        from the initial random population is used as the result.\n"
              <<"    --threads=N\n"
              <<"        Number of threads used to calculate fitness levels for the population.\n"
              <<"\n"
              <<"DESCRIPTION\n"
              <<"    Reads function similarity measurements for multiple similarity metrics and calculates\n"
              <<"    the weightings and threshold that maximizes the F1 score.  The input is a text file with\n"
              <<"    one line per data point.  Each line starts with 'y' or 'n' to indicate whether the data\n"
              <<"    point represents two similar or dissimilar functions, respectively.  The remainder of\n"
              <<"    the line contains space-separated similarity measurements as floating point values. Each\n"
              <<"    line must have the same number of values.\n"
              <<"    \n"
              <<"    This program finds optimal values for a binary threshold T, and weights {W0, W1, ..., Wn}.\n"
              <<"    A function pair represented by a line of input is detected as similar or dissimilar by\n"
              <<"    the binary function:\n"
              <<"        detectedSimilar = (W0*S0 + W1*S1 + ... + Wn+Sn >= T)\n"
              <<"    where the set {S0, S1, ... Sn} are the similarity values specified on that line of input.\n"
              <<"    The detectedSimilar value is computed for all lines of input using the chosen threshold\n"
              <<"    and weightings to obtain an F1 score.  The final threshold and weightings are printed\n"
              <<"    to standard output one value per line.\n";
    exit(exit_status);
}


static const uint64_t TWO64M1 = 0xffffffffffffffffull;  // 2^64-1
static const size_t PRECISION = 6;

struct Switches {
    std::string samplesFileName;                        // name of file containing data points (or use stdin)
    size_t populationSize;                              // number of individuals in the population
    size_t nGenerations;                                // max number of generations to run after initial generation
    size_t nThreads;                                    // number of worker threads for computing fitness values
    Switches(): populationSize(1000), nGenerations(100), nThreads(1) {}
};

static LinearCongruentialGenerator uniform_random;
static double uniform_random_double() { return 1.0 * uniform_random() / TWO64M1; }

class Similarities {
    bool oracle_;
    std::vector<double> similarity_;
public:
    Similarities(bool b, const std::vector<double> &s)
        : oracle_(b), similarity_(s) {}
    bool oracle() const {
        return oracle_;
    }
    double operator[](size_t metric) const {
        return similarity_[metric];
    }
};

class Samples {
public:
    typedef std::vector<Similarities> SimilarityList;
private:
    SimilarityList metrics_;
public:
    void insert(const Similarities &similarities) {
        metrics_.push_back(similarities);
    }
    SimilarityList::const_iterator begin() const { return metrics_.begin(); }
    SimilarityList::const_iterator end() const { return metrics_.end(); }
};

struct SurfaceDomainValue {
    std::vector<double> weights;                        // weights between -1 and 1, inclusive
    double threshold;                                   // threshold of similarity
    void print(std::ostream &o) const {
        o <<"(t=" <<threshold;
        for (size_t i=0; i<weights.size(); ++i)
            o <<", w" <<i <<"=" <<weights[i];
        o <<")";
    }
};

std::ostream& operator<<(std::ostream &o, const SurfaceDomainValue &x) {
    x.print(o);
    return o;
}

class Surface {
    size_t nsim_;                                       // number of similarity metrics per line
    Samples samples_;                                   // data points where surface is sampled
public:
    explicit Surface(FILE *f): nsim_(0) { nsim_ = load(f); }
    size_t load(FILE*);                                 // initialize metrics
    size_t nsim() const { return nsim_; }
    double operator()(const SurfaceDomainValue&) const; // objective function
};

class Genome {
    uint64_t threshold;
    std::vector<uint64_t> weights;
public:
    explicit Genome(size_t nmetrics) {
        threshold = uniform_random();
        for (size_t i=0; i<nmetrics; ++i)
            weights.push_back(uniform_random());
    }
    Genome(const Genome &p1, const Genome &p2, const Genome &p3) {
        threshold = crossover(p1.threshold, p2.threshold, p3.threshold);
        assert(p1.weights.size()==p2.weights.size() && p1.weights.size()==p3.weights.size());
        for (size_t i=0; i<p1.weights.size(); ++i)
            weights.push_back(crossover(p1.weights[i], p2.weights[i], p3.weights[i]));
        mutate();
    }
    static uint64_t crossover(uint64_t a, uint64_t b, uint64_t c) {
        uint64_t x = a^b;
        return (x & c) | (~x & a); // bits of result are: c where a!=b; a where a==b
    }
    SurfaceDomainValue operator*() const {
        SurfaceDomainValue retval;
        retval.threshold = 1.0 * threshold / TWO64M1;
        for (size_t i=0; i<weights.size(); ++i)
            retval.weights.push_back(1.0 * weights[i] / TWO64M1);
        return retval;
    }
    void mutate() {
        size_t genomeSize = 64 + 64*weights.size();
        size_t nbits =  uniform_random_double() * 0.02 * genomeSize;
#ifdef DEBUGGING
        if (nbits>0)
            std::cerr <<"  mutating " <<nbits <<" of " <<genomeSize <<" bits\n";
#endif
        for (size_t i=0; i<nbits; ++i) {
            size_t idx = uniform_random() % genomeSize;
            size_t idx_major = idx / 64;
            size_t idx_minor = idx % 64;
            uint64_t swap = (uint64_t)1 << idx_minor;
#ifdef DEBUGGING
            std::cerr <<"    mutate: idx=(" <<idx_major <<"," <<idx_minor <<")\n";
#endif
            if (idx_major < weights.size()) {
                weights[idx_major] ^= swap;
            } else {
                assert(idx_major==weights.size());
                threshold ^= swap;
            }
        }
    }
    void print(std::ostream &o) const {
        char buf[32];
        snprintf(buf, sizeof buf, "%016"PRIx64, threshold);
        o <<buf;
        for (size_t i=0; i<weights.size(); ++i) {
            snprintf(buf, sizeof buf, "%016"PRIx64, threshold);
            o <<buf;
        }
    }
};

std::ostream& operator<<(std::ostream &o, const Genome &x) {
    x.print(o);
    return o;
}

struct Individual {
    Genome genome;                                      // genomic representation of a point in the objective function's domain
    double fitness;                                     // objective function evaluated at the point represented by the genome
    double cn_fitness;                                  // cumulative normalized fitness
    explicit Individual(const Genome &genome): genome(genome), fitness(0) {}
    bool operator<(const Individual &other) const {     // to sort by increasing fitness value
        return fitness < other.fitness;
    }
    void print(std::ostream &o) const {
        o <<"(genome=" <<genome <<", point=" <<*genome <<", fitness=" <<fitness <<", cn_fitness=" <<cn_fitness <<")";
    }
};

std::ostream& operator<<(std::ostream &o, const Individual &x) {
    x.print(o);
    return o;
}

typedef std::vector<Individual> Population;

// Load surface data from standard input.  The format is one data point per line, with each line being space-separated
// values. The first value is the character 'y' or 'n' to indicate the condition of similarity (the oracle) where 'y' means
// yes, they are similar and 'n' means no, they are not similar.  The next fields are the floating-point similarity values
// which are normally in the range [0..1].  Returns the number of similarity metrics per line.
size_t Surface::load(FILE *file) {
    using namespace StringUtility;
    char *line = NULL;
    size_t linesz = 0, linenum = 0, nsim = 0;
    while (rose_getline(&line, &linesz, file) > 0) {
        ++linenum;
        char *s=line;
        if (*s!='y' && *s!='n')
            throw std::runtime_error("line " + numberToString(linenum) + "." + numberToString(s-line+1) +
                                     ": condition 'y' or 'n' expected");
        bool oracle = 'y'== *s++;

        std::vector<double> sim;
        char *t = s;
        errno = 0;
        while (true) {
            while (isspace(*t)) ++t;
            if (!*t)
                break;
            sim.push_back(strtod(s=t, &t));
            if (errno || t==s)
                throw std::runtime_error("line " + numberToString(linenum) + "." + numberToString(s-line+1) +
                                         ": similarity value #" + numberToString(sim.size()+1) + " expected");
        }
        if (sim.empty())
            throw std::runtime_error("line " + numberToString(linenum) + "." + numberToString(t-line+1) +
                                     ": no similarity values");
        if (nsim>0 && sim.size()!=nsim)
            throw std::runtime_error("line " + numberToString(linenum) + "." + numberToString(t-line+1) +
                                     ": wrong number of similarity values; found " + numberToString(sim.size()) +
                                     " but expected " + numberToString(nsim));
        if (0==nsim)
            nsim = sim.size();

        samples_.insert(Similarities(oracle, sim));
    }
    return nsim;
}

double Surface::operator()(const SurfaceDomainValue &x) const {
    size_t tp=0, tn=0, fp=0, fn=0;                      // true positive, true negative, false positive, false negative counts
    double wtsum = 0.0;
    for (size_t i=0; i<x.weights.size(); ++i)
        wtsum += x.weights[i];
    for (Samples::SimilarityList::const_iterator mi=samples_.begin(); mi!=samples_.end(); ++mi) {
        const Similarities &sims = *mi;
        double avesim = 0.0;
        for (size_t i=0; i<x.weights.size(); ++i)
            avesim += x.weights[i] * sims[i];
        avesim /= wtsum;
        bool detected = avesim >= x.threshold;
        bool condition = sims.oracle();
        if (condition && detected) {
            ++tp;
        } else if (condition) {
            assert(!detected);
            ++fn;
        } else if (detected) {
            assert(!condition);
            ++fp;
        } else {
            assert(!detected && !condition);
            ++tn;
        }
    }

    double precision = 0==tp+fp ? 0.0 : 1.0 * tp / (tp + fp);
    double recall    = 0==tp+fn ? 0.0 : 1.0 * tp / (tp + fn);
    // F1 score: harmonic mean of precision and recall
    double f1 = (0==precision + recall) ? 0.0 : 2.0 * precision * recall / (precision + recall);
    return f1;
}

void sortAndNormalize(Population &population) {
    std::sort(population.begin(), population.end());
    double total_fitness = 0.0;
    for (Population::const_iterator pi=population.begin(); pi!=population.end(); ++pi)
        total_fitness += pi->fitness;
    double acc = 0.0;
    for (Population::iterator pi=population.begin(); pi!=population.end(); ++pi) {
        acc += pi->fitness / total_fitness;
        pi->cn_fitness = acc;
    }
#ifdef DEBUGGING
    for (size_t i=0; i<population.size(); ++i)
        std::cerr <<"  #" <<i <<": " <<population[i] <<"\n";
#endif
    std::cerr <<"  average fitness: " <<(total_fitness / population.size()) <<"\n";
    std::cerr <<"  best fitness " <<population.back().fitness <<" at " <<*population.back().genome <<"\n";
}

bool compareCNFitness(const Individual &individual, double cn_fitness) {
    return individual.cn_fitness < cn_fitness;
}

// Select a random individual from the population, preferring individuals with higher fitness.  This function requires
// that the population is sorted by increasing fitness and the cn_fitness data members have been initialized with cumulative
// normalized fitness values.
const Individual& selectByFitness(const Population &population) {
    double urcnf = uniform_random_double();
    Population::const_iterator pi = std::lower_bound(population.begin(), population.end(), urcnf, compareCNFitness);
#ifdef DEBUGGING
    size_t idx = std::min((size_t)(pi - population.begin()), population.size()-1);
    std::cerr <<"  selected #" <<idx <<": " <<population[idx] <<" urcnf=" <<urcnf <<"\n";
#endif
    return pi==population.end() ? population.back() : *pi;
}

struct FitnessWorker {
    const Surface &surface;
    Individual *individuals;
    size_t nIndividuals;
    FitnessWorker(const Surface &surface, Individual *p, size_t n): surface(surface), individuals(p), nIndividuals(n) {}
    void operator()() {
        for (size_t i=0; i<nIndividuals; ++i)
            individuals[i].fitness = surface(*individuals[i].genome);
    }
};

void computeFitness(Population &population, const Surface &surface, size_t maxThreads) {
    maxThreads = std::max((size_t)1, maxThreads);
    size_t worksize = std::max((size_t)1000, population.size()/maxThreads);
    size_t nworkers = (population.size() + worksize - 1) / worksize;// ceiling
    assert(population.empty() || (nworkers>0 && nworkers<=maxThreads));
    boost::thread *workers = new boost::thread[nworkers];

    size_t offset = 0;
    for (size_t i=0; i<nworkers; ++i) {
        size_t n = std::min(population.size()-offset, worksize);
        workers[i] = boost::thread(FitnessWorker(surface, &population[0] + offset, n));
        offset += n;
    }

    for (size_t i=0; i<nworkers; ++i)
        workers[i].join();
}

int main(int argc, char *argv[]) {
    std::ios::sync_with_stdio();
    argv0 = argv[0];
    {
        size_t slash = argv0.rfind('/');
        argv0 = slash==std::string::npos ? argv0 : argv0.substr(slash+1);
        if (0==argv0.substr(0, 3).compare("lt-"))
            argv0 = argv0.substr(3);
    }

    Switches opt;
    int argno = 1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else if (!strncmp(argv[argno], "--file=", 7)) {
            opt.samplesFileName = argv[argno]+7;
        } else if (!strncmp(argv[argno], "--population=", 13)) {
            opt.populationSize = strtoul(argv[argno]+13, NULL, 0);
        } else if (!strncmp(argv[argno], "--generations=", 14)) {
            opt.nGenerations = strtoul(argv[argno]+14, NULL, 0);
        } else if (!strncmp(argv[argno], "--threads=", 10)) {
            opt.nThreads = strtoul(argv[argno]+10, NULL, 0);
        } else {
            throw std::runtime_error(std::string("unrecognized command-line switch: ") + argv[argno]);
        }
    }
    if (argno<argc)
        throw std::runtime_error(std::string("invalid command-line argument: ") + argv[argno]);

    std::cerr.setf(std::ios::fixed, std::ios::floatfield);
    std::cerr.precision(PRECISION);

    FILE *samplesFile = opt.samplesFileName.empty() ? stdin : fopen(opt.samplesFileName.c_str(), "r");
    Surface f(samplesFile);

    // Initialize the vector of individuals.  The size of this vector is arbitrary
    std::cerr <<"building generation 0\n";
    Population population;
    population.reserve(opt.populationSize);
    for (size_t i=0; i<opt.populationSize; ++i) {
        Genome genome(f.nsim());
        population.push_back(Individual(genome));
    }
    computeFitness(population, f, opt.nThreads);
    sortAndNormalize(population);
    Individual maximum = population.back();

    // Optimization
    for (size_t generation=0; generation<opt.nGenerations && maximum.fitness<0.999999; ++generation) {
        std::cerr <<"building generation " <<(generation+1)
                  <<"; best fitness so far: " <<maximum.fitness <<" at " <<*maximum.genome <<"\n";
        Population nextPopulation;
        nextPopulation.reserve(population.size());
        for (size_t i=0; i<population.size(); ++i) {
            const Individual &p1 = selectByFitness(population);
            const Individual &p2 = selectByFitness(population);
            const Individual &p3 = selectByFitness(population);
            Genome genome(p1.genome, p2.genome, p3.genome);
#ifdef DEBUGGING
            std::cerr <<"  offspring     genome=" <<genome <<", point=" <<*genome <<"\n";
#endif
            nextPopulation.push_back(Individual(genome));
        }
        population = nextPopulation;
        computeFitness(population, f, opt.nThreads);
        sortAndNormalize(population);
        if (population.back().fitness > maximum.fitness)
            maximum = population.back();
    }

    // Final results
    std::cerr <<"best fitness overall: " <<maximum.fitness <<" at " <<*maximum.genome <<"\n";
    SurfaceDomainValue result = *maximum.genome;
    std::cout <<result.threshold <<"\n";
    for (size_t i=0; i<result.weights.size(); ++i)
        std::cout <<result.weights[i] <<"\n";

    return 0;
}
