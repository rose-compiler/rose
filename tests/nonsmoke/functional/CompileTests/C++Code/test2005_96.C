// Problem pointed out by Peter Pirkelbauer.
// Multiple SgInitializedName objects are built where only one might be more eligant.

// History of how this happened:
// Why the Constructor initialization list stores a SgInitializedName instead of a Symbol and a SgInitializer?
// I think this is mostly historical :-).  The constructor initialization list must hold either 
// data fields, or class names and an initializer; the data field name and class name are not easily
// represented through a single IR node (one is a SgInitializedName and the otehr is a SgClassDeclaration).
// Thus a SgInitializedName is used and in effect the constructor initialization list is semi-stringified
// (that is that the data filed and the class name are represented via a string held in the SgInitializedName).
// since a SgInitializedName can hold a SgInitializer, this is used to hold the constructor initializers
// initializer.  The SgInitializedName is a not quite perfect IR node for the constructior initializer and
// just sort of works OK (as long as the unparser can output the correct generated code for each case of how 
// the SgInitializedName is used).

// Problem with current design:
// A side-effect of using the SgInitializedName for this purpose is that it does not reference the SgSymbol
// and we end up with two SgInitializedNames generated internally in the following code (see the source code 
// with this test code).

// Possible solution:
// A better solution might be to have a specialized Constructor Initialized IR node which holds a reference to the
// Symbol representing the SgVariableSymbol or SgClassSymbol and containing an additional pointer for the SgInitializer.
// Note that the grammar defined in Stroustrup's Third Edition Appendix B separates out the concept of "mem-initializer-id"
// and this is supporting justification for perhaps having a specialied IR node for this purpose :-).  Fixing this
// detail would allow for a reference to a common symbol for the variable instead of a copy which is perhaps problematic.

// Example code showing the problem:
class X
   {
  // The SgInitializedName here is referenced by any/every SgVarRefExp though the SgVariableSymbol
     int x;
  // The "x" appearing in the constructor's preinitialization list (what we should likely call the 
  // "constructor initializer list") is a new SgInitializedName. So there are now two!
     X() : x(1) {}
   };

