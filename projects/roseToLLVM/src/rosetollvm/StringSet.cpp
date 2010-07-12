#include <rosetollvm/StringSet.h>
#include <iostream>
using namespace std;

int HashPrimes::primes[] = {DEFAULT_HASH_SIZE, 8191, 16411, MAX_HASH_SIZE};


StringSet::StringSet() : hash_size(primes[prime_index]) {
    base.resize(hash_size, NULL);
    assert(base.size() == hash_size);
}

StringSet::~StringSet() {
    for (int i = 0; i < element_pool.size(); i++)
         delete element_pool[i];
}


void StringSet::Rehash() {
    base.resize(0); // remove previous elements.
    hash_size = primes[++prime_index]; // compute new size
    base.resize(hash_size, NULL);
    assert(base.size() == hash_size);
    for (int i = 0; i < element_pool.size(); i++) {
        Element *ns = element_pool[i];
        int k = ns -> HashAddress() % hash_size;
        ns -> next = base[k];
        base[k] = ns;
    }

    return;
}

int StringSet::insert(const char *str) {
    unsigned hash_address = Hash(str);
    int k = hash_address % hash_size,
        len = strlen(str);

    Element *element;
    for (element = base[k]; element; element = (Element *) element -> next) {
        if (len == element -> Length() && memcmp(element -> Name(), str, len * sizeof(char)) == 0)
            return element -> Index();
    }

    element = new Element(str, element_pool.size(), hash_address);
    element_pool.push_back(element);

    element -> next = base[k];
    base[k] = element;

    //
    // If the number of unique elements in the hash table exceeds 2 times
    // the size of the base, and we have not yet reached the maximum
    // allowable size for a base, reallocate a larger base and rehash
    // the elements.
    //
    if ((element_pool.size() > (hash_size << 1)) && (hash_size < MAX_HASH_SIZE))
        Rehash();

    return element -> Index();
}

bool StringSet::contains(const char *str)  { return getIndex(str) >= 0; }

int StringSet::getIndex(const char *str) {
    unsigned hash_address = Hash(str);
    int k = hash_address % hash_size,
        len = strlen(str);
    for (Element *element = base[k]; element; element = (Element *) element -> next) {
        if (len == element -> Length() && memcmp(element -> Name(), str, len * sizeof(char)) == 0)
           return element -> Index();
    }

    return -1;
}

