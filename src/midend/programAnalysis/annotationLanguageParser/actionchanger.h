
#ifndef BDWY_ACTIONCHANGER_H
#define BDWY_ACTIONCHANGER_H

// ----------------------------------------------------------------------
//  Action changer
// ----------------------------------------------------------------------

/** @brief Action changer
 *
 * This changer visits all the statements in the target program looking for
 * action annotations that apply. Conflicts are resolved by choosing the
 * first one that applies. */

class actionChanger : public Changer
{
private:

  Annotations * _annotations;

  propertyAnalyzer * _analyzer;

  Linker &linker;

public:

  actionChanger(Analyzer * pointer_analyzer, Annotations * annotations);

  virtual Node * at_threeAddr(threeAddrNode * the_3addr, Order ord);

};

#endif /* BDWY_ACTIONCHANGER_H */
