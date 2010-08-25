
typedef map< memoryDef *, ipConstant > constant_map;
typedef constant_map::iterator constant_map_p;

// ------------------------------------------------------------
// ipConstant
// ------------------------------------------------------------

// This class represents a single constant value associated with one
// def of a variable. It can hold either a constant value, or have the
// special lattice TOP value. The lattice BOTTOM is represented by a
// constant no_val (which is built into the constant class).

class ipConstant
{
private:

  constant _value;
  bool _top;
  bool _internal;

  ipConstant(constant & value);
  ipConstant();
  ipConstant(const ipConstant & other);
   ~ipConstant();

  // -- Fields

  void to_bottom();
  bool is_top() const;
  bool is_bottom() const;

  inline const constant & value() const { return _value; }
  inline constant & value() { return _value; }

public:

  inline void intern() { _internal = true; }

  void assign(const ipConstant & other);

  // -- Comparison

  bool diff(ipConstant & other) const;

  // -- Meet "^" operator
  // Must satisfy the following:
  //    Associative: x ^ (y ^ z) = (x ^ y) ^ z
  //    Commutative: x ^ y = y ^ x
  //    Idempotent:  x ^ x = x

  void meet_with(const ipConstant & other);

  // -- Computational functions

  void binary_operator(const Operator * op,
                       const ipConstant & right_operand);

  void unary_operator(const Operator * op);

  void cast_operator(const typeNode * type);

  // -- Output

  void print(ostream & o);

};

class constantProp
{
private:

  Pointers * _pointers;
  constant_map _constants;
  bool _debug;

public:

  constantProp(Pointers * pointers);

  void analyze();

  void eval(stmtLocation * current,
	    exprNode * expr, constant & val);

private:

  bool merge_constants(memorydef_set & defs,
                       memoryuse_set & uses);

  void rebuild_constant(memoryuse_set & uses,
			ipConstant & value);

  void rebuild_constant(stmtLocation * where,
			pointerValue & pointer,
			ipConstant & value);
}
