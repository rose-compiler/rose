

#ifndef BDWY_CALLINGCONTEXT_H
#define BDWY_CALLINGCONTEXT_H


class callingContext
{
private:

  /** @brief Call site location */

  procLocation * _where;

  /** @brief The call in the application program */

  operandNode * _call;

  /** @brief Dataflow analysis object */

  propertyAnalyzer * _property_analyzer;

  /** @brief Annotations for the library call */

  procedureAnn * _procedure_annotations;

  /** @brief Caller info */

  procedureInfo * _caller;

  /** @brief All annotations */

  Annotations * _annotations;

public:

  /** @brief Create a new calling context */

  callingContext(procLocation * where,
                 operandNode * call,
                 propertyAnalyzer * property_analyzer,
                 procedureAnn * procedure_annotations,
                 procedureInfo * caller,
                 Annotations * annotations);

  /** @brief Get the call site location */

  inline procLocation * where() const { return _where; }

  /** @brief Get the callNode */

  inline operandNode * call() const { return _call; }

  /** @brief Get the dataflow analysis object */

  inline propertyAnalyzer * property_analyzer() const { return _property_analyzer; }
  
  /** @brief Get the library procedure annotations */

  inline procedureAnn * procedure_annotations() const
  { return _procedure_annotations; }

  /** @brief Get the caller info */

  inline procedureInfo * caller() const { return _caller; }

  /** @brief Get all annotations */

  inline Annotations * annotations() const { return _annotations; }

  /** @brief Get the special def/use location
   *
   * This is the stmtLocation used to attach all defs and uses that occur
   * within an annotation. */

  stmtLocation * def_use_location();

  /** @brief Get the special deallocation location
   *
   * This is the stmtLocation used to attach all deallocations. It is
   * chosen to dominate the def/use location so that
   * deallocation/allocation pairs work properly with the multiplicity
   * analysis. */

  stmtLocation * dealloc_location();

  /** @brief Set current defs and uses
   *
   * For the given pointerValue (set of memoryBlocks), set the current_def
   * and current_use for this context. */

  void set_current_def_and_use(pointerValue & ptr);

};

#endif /* BDWY_CALLINGCONTEXT_H */

