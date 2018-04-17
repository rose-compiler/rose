
// This test code show the use of a SgArrowOp in a SgFunctionCall get_function() return value, as I understand it.
// This may violate the ASt consistancy rule so we might have to fixup the AST consistancy definition.
// This test code also failed to work with the callGraph.C application code (in this directory).


class locale {

    bool
    operator==(const locale& __other) const throw ();

    inline bool
    operator!=(const locale& __other) const throw ()
    { return !(this->operator==(__other));  }
};                                                                                                               
