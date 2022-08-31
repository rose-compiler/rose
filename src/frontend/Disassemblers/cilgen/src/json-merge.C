#include <iostream>
#include <iomanip>
#include <fstream>
#include <limits>

#include <nlohmann/json.hpp>

//~ namespace scl = Sawyer::CommandLine;
namespace json = nlohmann;

namespace
{
  const char* description = "Merges two json arrays after sorting their elements by Name if Name is available."

  /// pseudo type to indicate that an element is not in a sequence
  struct unavailable_t {};

  /// \brief  traverses two ordered associative sequences in order of their elements.
  ///         The elements in the sequences must be convertible. A merge object
  ///         is called with sequence elements in order of their keys in [aa1, zz1[ and [aa2, zz2[.
  /// \tparam _Iterator1 an iterator of an ordered associative container
  /// \tparam _Iterator2 an iterator of an ordered associative container
  /// \tparam BinaryOperator a merge object that provides three operator()
  ///         functions.
  ///         - void operator()(_Iterator1::value_type, unavailable_t);
  ///           called when an element is in sequence 1 but not in sequence 2.
  ///         - void operator()(unavailable_t, _Iterator2::value_type);
  ///           called when an element is in sequence 2 but not in sequence 1.
  ///         - void operator()(_Iterator1::value_type, _Iterator2::value_type);
  ///           called when an element is in both sequences.
  /// \tparam Comparator compares elements in sequences.
  ///         called using both (_Iterator1::key_type, _Iterator2::key_type)
  //          and (_Iterator2::key_type, _Iterator1::key_type).
  template <class _Iterator1, class _Iterator2, class BinaryOperator, class Comparator>
  BinaryOperator
  merge_assoc( _Iterator1 aa1, _Iterator1 zz1,
               _Iterator2 aa2, _Iterator2 zz2,
               BinaryOperator binop,
               Comparator comp
             )
  {
    static constexpr unavailable_t unavail;

    while (aa1 != zz1 && aa2 != zz2)
    {
      if (comp(aa1.key(), (aa2.key())))
      {
        binop(aa1, unavail);
        ++aa1;
      }
      else if (comp(aa2.key(), (aa1.key())))
      {
        binop(unavail, aa2);
        ++aa2;
      }
      else
      {
        binop(aa1, aa2);
        ++aa1; ++aa2;
      }
    }

    while (aa1 != zz1)
    {
      binop(aa1, unavail);
      ++aa1;
    }

    while (aa2 != zz2)
    {
      binop(unavail, aa2);
      ++aa2;
    }

    return binop;
  }

  template <class _Iterator1, class _Iterator2, class BinaryOperator>
  BinaryOperator
  merge_seq( _Iterator1 aa1, _Iterator1 zz1,
             _Iterator2 aa2, _Iterator2 zz2,
             BinaryOperator binop
           )
  {
    static constexpr unavailable_t unavail;

    while (aa1 != zz1 && aa2 != zz2)
    {
      binop(aa1, aa2);
      ++aa1; ++aa2;
    }

    while (aa1 != zz1)
    {
      binop(aa1, unavail);
      ++aa1;
    }

    while (aa2 != zz2)
    {
      binop(unavail, aa2);
      ++aa2;
    }

    return binop;
  }


  json::json
  read_json(std::string fn)
  {
    std::ifstream inp{fn};
    json::json    res;

    inp >> res;
    return res;
  }

  json::json
  merge_json(const json::json& lhs, const json::json& rhs);


  struct ArrayMerger
  {
    template <class JsonIterator>
    void operator()(JsonIterator lhs, JsonIterator rhs)
    {
      o.push_back(merge_json(lhs.value(), rhs.value()));
    }

    template <class JsonIterator>
    void operator()(JsonIterator lhs, const unavailable_t&)
    {
      o.push_back(lhs.value());
    }

    template <class JsonIterator>
    void operator()(const unavailable_t&, JsonIterator rhs)
    {
      o.push_back(rhs.value());
    }

    json::json result() && { return std::move(o); }

    json::json o;
  };


  json::json
  merge_arrays(const json::json& lhs, const json::json& rhs)
  {
    return merge_seq( lhs.begin(), lhs.end(),
                      rhs.begin(), rhs.end(),
                      ArrayMerger{}
                    ).result();
  }

  struct ObjectMerger
  {
    template <class JsonIterator>
    void operator()(JsonIterator lhs, JsonIterator rhs)
    {
      o[lhs.key()] = merge_json(lhs.value(), rhs.value());
    }

    template <class JsonIterator>
    void operator()(JsonIterator lhs, const unavailable_t&)
    {
      o[lhs.key()] = lhs.value();
    }

    template <class JsonIterator>
    void operator()(const unavailable_t&, JsonIterator rhs)
    {
      o[rhs.key()] = rhs.value();
    }

    json::json result() && { return std::move(o); }

    json::json o;
  };


  json::json
  merge_objects(const json::json& lhs, const json::json& rhs)
  {
    return merge_assoc( lhs.begin(), lhs.end(),
                        rhs.begin(), rhs.end(),
                        ObjectMerger{},
                        std::less<std::string>{}
                      ).result();
  }



  json::json
  merge_json(const json::json& lhs, const json::json& rhs)
  {
    if (lhs.is_array())
    {
      if (!rhs.is_array())
      {
        std::cerr << "L: " << lhs << std::endl;
        std::cerr << "R: " << rhs << std::endl;

        throw std::runtime_error{"incompatible structure <lhs=array>"};
      }

      return merge_arrays(lhs, rhs);
    }

    if (lhs.is_object())
    {
      if (!rhs.is_object())
      {
        std::cerr << "L: " << lhs << std::endl;
        std::cerr << "R: " << rhs << std::endl;

        throw std::runtime_error{"incompatible structure <lhs=object>"};
      }

      return merge_objects(lhs, rhs);
    }

    if (lhs != rhs)
    {
      std::cerr << "L: " << lhs << std::endl;
      std::cerr << "R: " << rhs << std::endl;

      //~ throw std::runtime_error{"incompatible basic object"};
    }

    return lhs;
  }

  void sort_array(json::json& o)
  {
    if (!o.is_array()) return;

    std::sort( o.begin(), o.end(),
               [](json::json& lhs, json::json& rhs) -> bool
               {
                 if (!lhs.is_object()) return true;
                 if (!lhs.contains("Name")) return true;
                 if (!lhs["Name"].is_string()) return true;

                 if (!rhs.is_object()) return false;
                 if (!rhs.contains("Name")) return false;
                 if (!rhs["Name"].is_string()) return false;

                 return lhs["Name"] < rhs["Name"];
               }
             );
  }
} // anonymous namespace

int main(int argc, char** argv)
{
  if (argc < 3)
  { 
    std::cerr << description << std::endl; 
    return 1;
  }

  json::json lhs = read_json(argv[1]);
  json::json rhs = read_json(argv[2]);

  sort_array(lhs);
  sort_array(rhs);

  json::json o = merge_json(lhs, rhs);

  std::cout << std::setw(2) << o << std::endl;
  return 0;
}

