// compile: CXX -Wall -Wextra -pedantic -I$ROSE_SOURCE/src/3rdPartyLibraries/json/ src/json2rosetta.C -o json2rosetta.bin

#include <iostream>
#include <fstream>
#include <limits>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <nlohmann/json.hpp>


namespace json = nlohmann;

namespace
{
  const char* description = "Generates parts of ROSETTA files according to a model text-file.";
  const char* usage = "usage: json2rosetta.bin model_file.txt description.json >output.txt\n"
                      "       Model files are located under $ROSE_SOURCE/src/frontend/Disassemblers/cilgen/blueprint.\n"
                      "       e.g., the SgAsmCil* nodes in binaryInstruction.C can be generated with:\n"
                      "          json2rosetta $ROSE_SOURCE/src/frontend/Disassemblers/cilgen/blueprint/binaryInstruction_C.txt $ROSE_SOURCE/src/frontend/Disassemblers/cilgen/cil.json >binaryInstruction_cilpart.C\n"
                      "       currently (parts) of the following files can be generated:\n"
                      "         * $ROSE_SOURCE/src/ROSETTA/src/binaryInstruction.C (parts)\n"
                      "         * $ROSE_SOURCE/src/ROSETTA/astNodeList (parts)\n"
                      "         * $ROSE_SOURCE/src/frontend/BinaryFormats/CilImpl.C (full)\n";


  enum class Mode
  {
    undef     = 0,
    cFile     = 1 << 0,
    defaultMode = cFile  // only supported mode
  };

  const char        delimch{'%'};
  const std::string delimstr(2, delimch); // note: do not use {} ctor on strings

  struct Parameters
  {
    Mode m = Mode::defaultMode;
  };

  template <class RandomAccessOperator, class CharT>
  RandomAccessOperator
  trimBackward(RandomAccessOperator lim, RandomAccessOperator pos, CharT what, int n = std::numeric_limits<int>::max())
  {
    assert(pos >= lim);

    if (pos == lim) return pos;

    while ((std::prev(pos) != lim) && (n > 0) && (*std::prev(pos) == what)) { --pos; --n; }

    return pos;
  }

  template <class RandomAccessOperator, class CharT>
  RandomAccessOperator
  trimForward(RandomAccessOperator pos, RandomAccessOperator lim, CharT what, int n = std::numeric_limits<int>::max())
  {
    assert(pos <= lim);

    while ((pos != lim) && (n > 0) && (*pos == what)) { ++pos; --n; }

    return pos;
  }

  std::string json2txt(const json::json& o)
  {
    std::string res = o.dump();

    if ((res.size() > 6) && boost::algorithm::starts_with(res, "\"\\\"") && boost::algorithm::ends_with(res, "\\\"\""))
      return res.substr(3, res.size()-6);

    return res;
  }


  void printText( std::ostream& os,
                  std::string::const_iterator pos,
                  std::string::const_iterator lim
                )
  {
    while (pos != lim) { os << *pos; ++pos; }
  }


  std::string
  loadTxtFile(std::string filename)
  {
    std::ifstream ifs{filename};
    std::string   str(std::istreambuf_iterator<char>{ifs}, {});

    return str;
  }

  struct ModelToken : std::tuple<int, std::string::const_iterator, std::string::const_iterator>
  {
    enum Kind { undef = 0, root = 1, text = 2, forall = 3, endfor = 4, field = 5, eof = 6 };

    using base = std::tuple<int, std::string::const_iterator, std::string::const_iterator>;
    using base::base;

    int                          kind()  const { return std::get<0>(*this); }
    std::string::const_iterator& begin()       { return std::get<1>(*this); }
    std::string::const_iterator& end()         { return std::get<2>(*this); }
    std::string::const_iterator  begin() const { return std::get<1>(*this); }
    std::string::const_iterator  end()   const { return std::get<2>(*this); }
    std::string                  value() const { return std::string{begin(), end()}; }
  };

  struct ModelTokenizer
  {
      ModelTokenizer(std::string::const_iterator first, std::string::const_iterator limit)
      : aa{first}, zz{limit}
      {}

      ModelToken specialToken(std::string::const_iterator pos, bool fast = false)
      {
        static const std::string kw_forall = "forall ";
        static const std::string kw_endfor = "end_for";

        if (pos == zz) return ModelToken{ModelToken::eof, pos, zz};
        if (*pos != delimch) return ModelToken{ModelToken::undef, pos, pos};
        ++pos;

        if (pos == zz) return ModelToken{ModelToken::undef, pos, pos};
        if (*pos != delimch) return ModelToken{ModelToken::undef, pos, pos};
        ++pos;

        if (fast) return ModelToken{ModelToken::root, pos, zz};

        // consumed %%
        std::string::const_iterator lim = std::search(pos, zz, delimstr.begin(), delimstr.end());

        assert(lim != zz);
        const std::string           word{pos, lim};

        aa = std::next(lim, delimstr.size());

        if (boost::algorithm::starts_with(word, kw_forall))
          return ModelToken{ModelToken::forall, std::next(pos, kw_forall.size()), lim};

        if (word == kw_endfor)
          return ModelToken{ModelToken::endfor, lim, lim};

        return ModelToken{ModelToken::field, pos, lim};
      }

      ModelToken nextToken()
      {
        ModelToken res = specialToken(aa);

        if (res.kind() != ModelToken::undef) return res;

        std::string::const_iterator pos = aa;
        while (pos != zz && specialToken(pos, true).kind() == ModelToken::undef)
        {
          ++pos;
        }

        res = ModelToken{ModelToken::text, aa, pos};

        aa = pos;
        return res;
      }


    private:
      std::string::const_iterator       aa;
      const std::string::const_iterator zz;
  };

  struct ModelBlock2 : private std::vector<ModelBlock2>
  {
      using children = std::vector<ModelBlock2>;

      explicit
      ModelBlock2(ModelToken token)
      : children(), tok(token)
      {}

      using children::emplace_back;
      using children::begin;
      using children::end;
      using children::size;
      using children::back;

      std::string::const_iterator string_begin() const { return tok.begin(); }
      std::string::const_iterator string_end()   const { return tok.end(); }
      std::string                 string_value() const { return tok.value(); }
      int                         kind()         const { return tok.kind(); }
      ModelToken&                 string_token()       { return tok; }

    private:
      ModelToken               tok;
  };

  void removeTrailingWhitespace(ModelToken& tok)
  {
    if (tok.kind() != ModelToken::text)
      return;

    std::string::const_iterator beg = tok.begin();
    std::string::const_iterator lim = tok.end();

    lim = trimBackward(beg, lim, ' ');
    lim = trimBackward(beg, lim, '\n', 1);

    tok.end() = lim;
  }


  ModelBlock2
  parseModel2(const std::string& model)
  {
    std::vector<ModelBlock2>    modelStack;
    std::string::const_iterator aa = model.begin();
    std::string::const_iterator zz = model.end();
    ModelTokenizer              tokenizer{aa, zz};
    ModelToken                  tok{ModelToken::root, model.begin(), model.begin()};

    while (tok.kind() != ModelToken::eof)
    {
      switch (tok.kind())
      {
        case ModelToken::field:
        case ModelToken::text:
          {
            modelStack.back().emplace_back(tok);
            break;
          }

        case ModelToken::root:
        case ModelToken::forall:
          {
            modelStack.emplace_back(tok);
            break;
          }

        case ModelToken::endfor:
          {
            assert(modelStack.size() > 1);
            ModelBlock2 child = std::move(modelStack.back());

            if (child.size())
              removeTrailingWhitespace(child.back().string_token());

            modelStack.pop_back();
            modelStack.back().emplace_back(std::move(child));
            break;
          }

        default:
          assert(false);
      }

      tok = tokenizer.nextToken();
    }

    assert(modelStack.size() == 1);
    return std::move(modelStack.back());
  }

  std::vector<std::string>
  partitionName(const std::string& name)
  {
    std::vector<std::string>    res;
    std::string::const_iterator aaa = name.begin();
    std::string::const_iterator zzz = name.end();

    for (;;)
    {
      std::string::const_iterator pos = std::find(aaa, zzz, '.');

      res.emplace_back(aaa, pos);
      if (pos == zzz) break;
      aaa = std::next(pos);
    }

    return res;
  }

  json::json&
  fieldValue(const std::vector<std::pair<std::string, json::json*> >& objs, const std::string& name)
  {
    const std::vector<std::string>  path = partitionName(name);
    assert(path.size() > 0);

    const std::string&              prefix = path.front();
    assert(prefix.size() > 0);

    auto                            pos = std::find_if( objs.rbegin(), objs.rend(),
                                                        [&prefix](const std::pair<std::string, json::json*>& cand) -> bool
                                                        {
                                                          return cand.first == prefix;
                                                        }
                                                      );

    //~ if (pos == objs.rend())
      //~ std::cerr << "Unable to find " << name << std::endl;

    assert(pos != objs.rend());
    json::json* root = pos->second;
    assert(root);

    for (int i = 1; i < int(path.size()); ++i)
      root = &((*root)[path[i]]);

    assert(root);
    return *root;
  }

  struct empty_object {};

  json::json&
  arrayObject(const std::vector<std::pair<std::string, json::json*> >& objs, const std::string& field)
  {
    assert(objs.size() && objs.back().second);

    json::json& obj = *(objs.back().second);

    if (obj.is_array())
      return obj;

    if (!obj.contains(field))
      throw empty_object{};

    json::json& res = obj[field];

    if (!res.is_array())
    {
      std::cerr << "field is not an array in obj: " << obj << std::endl;
      assert(false);
    }

    return res;
  }


  void runModel(std::ostream& os, const ModelBlock2& m, std::vector<std::pair<std::string, json::json*> >& objs)
  {
    assert(m.kind() != ModelToken::root);

    switch (m.kind())
    {
      case ModelToken::text:
        printText(os, m.string_begin(), m.string_end());
        break;

      case ModelToken::field:
      {
        std::string s   = json2txt(fieldValue(objs, m.string_value()));

        if (s.size() && s[0] == '"')
          printText(os, s.begin()+1, s.end()-1);
        else
          printText(os, s.begin(), s.end());

        break;
      }

      case ModelToken::forall:
      {
        try
        {
          std::string var = m.string_value();
          json::json& arr = arrayObject(objs, var);

          for (const auto& el : arr.items())
          {
            objs.emplace_back(var, &el.value());

            for (const ModelBlock2& child : m)
              runModel(os, child, objs);

            objs.pop_back();
          }
        }
        catch (const empty_object&) {}

        break;
      }

      default:
        std::cerr << "ModelKind: " << m.kind() << std::endl;
        assert(false);
    }
  }


  void runModel(std::ostream& os, const ModelBlock2& m, json::json& j)
  {
    assert(m.kind() == ModelToken::root);

    std::vector<std::pair<std::string, json::json*> > objs;

    objs.emplace_back("", &j);

    for (const ModelBlock2 child : m)
      runModel(os, child, objs);

    assert(objs.size() == 1);
  }


  void generateRosetta( std::ostream& os,
                        json::json& all,
                        const std::string& modelFile = "blueprint/model.txt"
                      )
  {
    std::string modelTxt = loadTxtFile(modelFile);
    ModelBlock2 modelBlk = parseModel2(modelTxt);

    runModel(os, modelBlk, all);
  }
} // anonymous namespace


int main(int argc, char** argv)
{
  if (argc < 3)
  {
    std::cout << description << std::endl
              << usage << std::endl
              << std::endl;
    return 0;
  }

  std::string modlFile = argv[1];
  std::string jsonFile = argv[2];

  json::json  js;

  {
    std::ifstream jsInp{jsonFile};

    jsInp >> js;
  }

  generateRosetta(std::cout, js, modlFile);
}

