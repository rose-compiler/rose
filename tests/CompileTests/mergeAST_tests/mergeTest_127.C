// This will build a SgClassDeclaration and a SgClassDefinition
// where the class definition is marked as compiler generated.
template <typename T> class Y;

// forward declaration of template specialization
class Y<int>;
// class Y<int>{};
class Y<int>;
