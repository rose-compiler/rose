#include <sage3basic.h>

SgKeyDatumPairPtrList&
SgDictionaryExp::get_key_datum_pairs()
   { return p_key_datum_pairs; }

const SgKeyDatumPairPtrList&
SgDictionaryExp::get_key_datum_pairs() const
   { return p_key_datum_pairs; }

void
SgDictionaryExp::append_pair(SgKeyDatumPair *what)
   { what->set_parent(this); p_key_datum_pairs.push_back(what); }
