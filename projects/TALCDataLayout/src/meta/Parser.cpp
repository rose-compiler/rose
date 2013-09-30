/*
 * parser.cpp
 *
 *  Created on: Oct 11, 2012
 *      Author: kamalsharma
 */

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>


#include "Parser.hpp"

namespace metaparser
{
	using namespace std;
    namespace fusion = boost::fusion;
    namespace phoenix = boost::phoenix;
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    // To store single field element
    struct field_elem_struct
	{
		string name;
		string type;
	};

    // To store multiple field elements e.g. Field x , y
    struct field_block_struct
	{
    	vector<field_elem_struct> fieldElements;
    	int blocksize;
	};

    // To store a single View which contains
    // other views and fields
    struct view_struct
    {
        string name;
        vector<field_block_struct> fieldList;
        vector<view_struct> childrenViews;
    };

    struct meta_struct
    {
    	vector<view_struct> viewList;
    };
}

BOOST_FUSION_ADAPT_STRUCT(
	metaparser::field_elem_struct,
	(std::string, name)
	(std::string, type)
)

BOOST_FUSION_ADAPT_STRUCT(
	metaparser::field_block_struct,
	(std::vector<metaparser::field_elem_struct>, fieldElements)
	(int, blocksize)
)

BOOST_FUSION_ADAPT_STRUCT(
	metaparser::view_struct,
    (std::string, name)
    (std::vector<metaparser::field_block_struct>, fieldList)
    (std::vector<metaparser::view_struct>, childrenViews)
)

BOOST_FUSION_ADAPT_STRUCT(
	metaparser::meta_struct,
    (std::vector<metaparser::view_struct>, viewList)
)

namespace metaparser
{
    int const tabsize = 4;

    void tab(int indent)
    {
        for (int i = 0; i < indent; ++i)
        	trace << ' ';
    }

    static void printField(field_elem_struct const& field_elem, int indent)
	{
    	tab(indent);
    	trace << "elem:" << field_elem.name << " , type: " << field_elem.type << std::endl;
	}

    static void printFieldBlock(field_block_struct const& field_block, int indent)
	{
		tab(indent);
		trace << "Field: " << std::endl;
		tab(indent);
		trace << '{' << std::endl;

		foreach(field_elem_struct const& field_elem, field_block.fieldElements)
		{
			printField(field_elem, indent+tabsize);
		}

		tab(indent);
		trace << "blocksize: " << field_block.blocksize << std::endl;

		tab(indent);
		trace << '}' << std::endl;
	}

    static void printView(view_struct const& view, int indent)
	{
		tab(indent);
		trace << "View: " << view.name  << std::endl;
		tab(indent);
		trace << '{' << std::endl;

		foreach(view_struct const& childView, view.childrenViews)
        {
        	printView(childView, indent+tabsize);
        }

		foreach(field_block_struct const& field_block, view.fieldList)
		{
        	printFieldBlock(field_block, indent+tabsize);
		}

		tab(indent);
		trace << '}' << std::endl;
	}

    static void printMeta(meta_struct const& meta)
    {
    	foreach(view_struct const& view, meta.viewList)
        {
        	printView(view, tabsize);
        }
    }

    // Builds the View Class
	Field *buildField(field_elem_struct const& fieldStruct, int position)
	{
		Field *field = new Field(fieldStruct.name);

		if(fieldStruct.type.compare("i")==0)
		{
			field->setType(FieldTypes::INT);
		}
		else if(fieldStruct.type.compare("f")==0)
		{
			field->setType(FieldTypes::FLOAT);
		}
		else if(fieldStruct.type.compare("d")==0)
		{
			field->setType(FieldTypes::DOUBLE);
		}
		else
		{
			trace << " Illegal Type found for Field " << fieldStruct.name << endl;
			exit(1);
		}

		// Set Position
		field->setPosition(position);

		return field;
	}

    // Builds the Field Block Class
    FieldBlock *buildFieldBlock(field_block_struct const& fieldBlockStruct)
    {
    	int position = 0;
    	FieldBlock *fieldBlock = new FieldBlock();

    	foreach(field_elem_struct const& field_elem, fieldBlockStruct.fieldElements)
		{
    		fieldBlock->addField(buildField(field_elem,position));
    		position++;
		}

    	fieldBlock->setBlockSize(fieldBlockStruct.blocksize);

    	return fieldBlock;
    }

    // Builds the View Class
	View *buildView(view_struct const& viewStruct)
	{
		View *view = new View(viewStruct.name);

		foreach(view_struct const& childView, viewStruct.childrenViews)
        {
			view->addSubView(buildView(childView));
        }

		foreach(field_block_struct const& field_block, viewStruct.fieldList)
		{
			view->addFieldBlock(buildFieldBlock(field_block));
		}
		return view;
	}

    // Builds the Meta Class
    Meta *buildMeta(meta_struct const& metaStruct)
    {
    	Meta *meta = new Meta();
    	foreach(view_struct const& view, metaStruct.viewList)
		{
			meta->addView(buildView(view));
		}
    	meta->buildFieldMap();
    	return meta;
    }

    /// Meta File Grammar
    template <typename Iterator>
    struct meta_grammar
      : qi::grammar<Iterator, meta_struct(), ascii::blank_type>
    {
    	  meta_grammar()
          : meta_grammar::base_type(meta_tag)
        {
            using qi::lit;
            using qi::lexeme;
            using ascii::char_;
            using qi::int_;
            using ascii::blank;
			using qi::eol;
			using qi::eoi;
            using ascii::string;
            using namespace qi::labels;

            using phoenix::at_c;
            using phoenix::push_back;

            comma = ',';
            field_text = lexeme[+(char_ - (lit(':')|lit('}'))) [_val += _1]];
            type_text = lexeme[+(char_ - (lit('}')|lit(',')|blank)) [_val += _1]];
            node = view_node_tag [_val = _1] ;

            start_tag =
                    "View"
                >>  lexeme[+(char_ - (lit('{')|eol))		[_val += _1]]
                >> *(eol)
                >> "{"
                >> *(eol)
            ;

            end_tag =
            		*(eol)
                    >> "}"
            		>> *(eol)
            ;

            field_elem_tag =
            		field_text [at_c<0>(_val) = _1]
            		>> ":"
            		>> type_text [at_c<1>(_val) = _1]
            		>> -(comma)
            		>> *(blank)
            ;

            field_blocksize_tag =
            		"["
            		>> int_ [_val = _1]
            		>> "]"
			 ;

            field_block =
					"Field {"
				>>  *(field_elem_tag	[push_back(at_c<0>(_val), _1)])
				>> "}"
				>> -(eol)
				>> -(field_blocksize_tag  [at_c<1>(_val) += _1])
				>> -(eol)
			;

            field_block_tag = (field_block)            [_val = _1];

            view_node_tag =
                    start_tag		[at_c<0>(_val) = _1]
                >>  *(field_block_tag [push_back(at_c<1>(_val), _1)])
                >>  (*node [push_back(at_c<2>(_val), _1)])
                >>  *(field_block_tag [push_back(at_c<1>(_val), _1)])
                >>  end_tag
            ;

            view_tree_tag = (view_node_tag)            [_val = _1];

            meta_tag =
            		+(view_tree_tag	[push_back(at_c<0>(_val), _1)])
			;

//            meta_tag.name("meta_tag");
//            view_tree_tag.name("view_tree_tag");
//            view_node_tag.name("view_node_tag");
//            field_elem_tag.name("field_elem_tag");
//            field_block.name("field_block");
//            field_block_tag.name("field_block_tag");
//            start_tag.name("start_tag");
//            end_tag.name("end_tag");
//            field_text.name("field_text");
//            debug(meta_tag);
//			debug(view_tree_tag);
//			debug(view_node_tag);
//			debug(field_elem_tag);
//			debug(field_block);
//			debug(field_block_tag);
//			debug(start_tag);
//			debug(end_tag);
        }

    	qi::rule<Iterator, meta_struct(), ascii::blank_type> meta_tag;
    	qi::rule<Iterator, view_struct(), ascii::blank_type> view_tree_tag;
        qi::rule<Iterator, view_struct(), ascii::blank_type> view_node_tag;
        qi::rule<Iterator, field_elem_struct(), ascii::blank_type> field_elem_tag;
        qi::rule<Iterator, int(), ascii::blank_type> field_blocksize_tag;
        qi::rule<Iterator, field_block_struct(), ascii::blank_type> field_block;
        qi::rule<Iterator, field_block_struct(), ascii::blank_type> field_block_tag;
        qi::rule<Iterator, view_struct(), ascii::blank_type> node;
        qi::rule<Iterator, std::string(), ascii::blank_type> field_text;
        qi::rule<Iterator, std::string(), ascii::blank_type> type_text;
        qi::rule<Iterator, void(), ascii::blank_type> comma;
        qi::rule<Iterator, std::string(), ascii::blank_type> start_tag;
        qi::rule<Iterator, void(), ascii::blank_type> end_tag;
    };
    //]
}

/**
 * Parses the meta file
 * @return
 */
Meta* Parser::parseFile()
{
    std::ifstream in(filename.c_str(), std::ios_base::in);

    if (!in)
    {
        trace << "Error: Could not open input file: "
            << filename << std::endl;
        exit(1);
    }

    std::string storage; // We will read the contents here.
    in.unsetf(std::ios::skipws); // No white space skipping!
    std::copy(
        std::istream_iterator<char>(in),
        std::istream_iterator<char>(),
        std::back_inserter(storage));

    typedef metaparser::meta_grammar<std::string::const_iterator> meta_grammar;
    meta_grammar metaGrammar; // meta file grammar
    metaparser::meta_struct meta; // view tree

    using boost::spirit::ascii::space;
    using boost::spirit::ascii::blank;
    std::string::const_iterator iter = storage.begin();
    std::string::const_iterator end = storage.end();
    bool r = phrase_parse(iter, end, metaGrammar, blank, meta);

    if (r && iter == end)
    {
        trace << "-------------------------\n";
        trace << "Parsing succeeded\n";
        trace << "-------------------------\n";
        metaparser::printMeta(meta);
        return metaparser::buildMeta(meta);
        //return 0;
    }
    else
    {
        std::string::const_iterator some = iter+30;
        std::string context(iter, (some>end)?end:some);
        trace << "-------------------------\n";
        trace << "Parsing failed\n";
        trace << "stopped at: \": " << context << "...\"\n";
        trace << "-------------------------\n";
        exit(1);
    }
    return NULL;
}

///**
// * Constructor
// * @param args
// */
//Parser::Parser(Rose_STL_Container<string> &args)
//{
//	if (CommandlineProcessing::isOptionWithParameter(args, "--meta", "*",filename,
//			true)) {
//		trace << "Filename set" << filename << std::endl;
//	}
//	else
//	{
//		traceerror << " Meta File not specified " << std::endl;
//		exit(1);
//	}
//
//}

/**
 * Constructor with filename parameter
 * @param filename
 */
Parser::Parser(string filename)
{
	this->filename = filename;
}
