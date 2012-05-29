
template <class T>
Expression<T>::Expression() :
  p_symbols(),
  p_coefficients()
{}

template <class T>
Expression<T>::~Expression() {}

template <class T>
void Expression<T>::addTerm(Expression<T>::Term & term) {
    p_coefficients.push_back(term);

    typename std::vector<std::pair<T *, unsigned> >::iterator it;
    for (it = term.second.begin(); it != term.second.end(); it++) {
        p_symbols.push_back(it->first);
    }
}

template <class T>
template <class ID_src>
bool Expression<T>::linearForm(LinearExpression_ppl & lin_exp, ID_src * id_src) const {
    bool is_linear = true;
    typename std::vector<Term>::const_iterator it_term;
    for (it_term = p_coefficients.begin(); it_term != p_coefficients.end() && is_linear; it_term++) {
        is_linear = is_linear
                   && (
                        (it_term->second.size() == 0)
                     || (it_term->second.size() == 1 && it_term->second[0].second == 1)
                   );
    }
    if (!is_linear) return false;

    for (it_term = p_coefficients.begin(); it_term != p_coefficients.end(); it_term++) {
        if (it_term->second.size() == 1)
	    lin_exp += it_term->first * id_src->getID(it_term->second[0].first);
        else
            lin_exp += it_term->first;
    }

    return true;
}

template <class T>
void Expression<T>::print(std::ostream & out) const {
    typename std::vector<Term>::const_iterator it_term;
    for (it_term = p_coefficients.begin(); it_term != p_coefficients.end(); it_term++) {
        if (it_term != p_coefficients.begin()) out << " + ";
        out << it_term->first;
        if (it_term->second.size() != 0) out << " * ";
        typename std::vector<std::pair<T *, unsigned> >::const_iterator it_factor;
        for (it_factor = it_term->second.begin(); it_factor != it_term->second.end(); it_factor++) {
            if (it_factor != it_term->second.begin()) out << " * ";
            it_factor->first->print(out);
            if (it_factor->second > 1) out << "^" << it_factor->second;
        }
    }
}

