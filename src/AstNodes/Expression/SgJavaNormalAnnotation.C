#include <sage3basic.h>

void
SgJavaNormalAnnotation::append_value_pair(SgJavaMemberValuePair *what)
   { what->set_parent(this); p_value_pair_list.push_back(what); }

void
SgJavaNormalAnnotation::prepend_value_pair(SgJavaMemberValuePair *what)
   { what->set_parent(this); p_value_pair_list.insert(p_value_pair_list.begin(), what); }
