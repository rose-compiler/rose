
#ifndef BDWY_PROPERTY_H
#define BDWY_PROPERTY_H

#include <string>

/** @brief A dataflow analysis property
 *
 * This is a base class for both the enum properties and the set
 * properties. */

class propertyAnn : public Ann
{
public:

  typedef enum { EnumProperty, SetProperty } propertyKind;

protected:

  /** @brief Name of the property */

  std::string _name;

  /** @brief ID
   *
   * This number is used to index into the vectors of enumValues used
   * by the property analyzer. The value for this property will always
   * occupy the position given by this number. */

  int _id;

  /** @brief Enabled
   *
   * This flag allows us to turn analysis on or off. */

  bool _is_enabled;

  /** @brief Analysis direction */

  Direction _direction;

  /** @brief The kind of property
   *
   * There are current two kinds: enumerated properties and setProperties. */

  propertyKind _kind;

public:

  /** @brief Create a new property */

  propertyAnn(const parserID * name,
              Direction direction,
              propertyKind kind)
    : Ann(name->line()),
      _name(name->name()),
      _is_enabled(true),
      _direction(direction),
      _kind(kind)
  {}

  /** @brief Set ID
   *
   * This should only be called once, by the annotations method
   * add_property(). */

  inline void set_id(int id) { _id = id; }

  /** @brief Get the ID */

  inline int get_id() const { return _id; }

  /** @brief Property name */

  inline std::string & name() { return _name; }
  inline const std::string & name() const { return _name; }

  /** @brief Is enabled? */

  inline bool is_enabled() const { return _is_enabled; }

  /** @brief Disable this analysis */

  inline void disable() { _is_enabled = false; }

  /** @brief Enable this analysis */

  inline void enable() { _is_enabled = true; }

  /** @brief Direction */

  inline Direction direction() const { return _direction; }

  /** @brief Kind */

  inline propertyKind kind() const { return _kind; }
};

#endif

