// DQ (10/5/2014): This is more strict now that we include rose_config.h in the sage3basic.h.
// #include "rose.h"
#include "sage3basic.h"

// #include "general_token_defs.h"

// DQ (11/29/2013): Added to support marking of redundant mappings of statements to token streams.
#include "tokenStreamMapping.h"

// DQ (12/4/2014): This is redundant with being included in "tokenStreamMapping.h".
// #include "frontierDetection.h"

#include "previousAndNextNode.h"

// DQ (11/29/2013): Added to support marking of redundant mappings of statements to token streams.
// #include "tokenStreamMapping.h"


using namespace std;
using namespace Rose;

// DQ (12/1/2013): Added switch to control testing mode for token unparsing.
// Test codes in the tests/nonsmoke/functional/roseTests/astTokenStreamTests directory turn on this 
// variable so that all regression tests can be processed to mix the unparsing of 
// the token stream with unparsing from the AST.
// ROSE_DLL_API bool ROSE_tokenUnparsingTestingMode = false;
extern bool ROSE_tokenUnparsingTestingMode;


// DQ (5/11/2021): Added support for selecting DOT colors, these are ALL of the 
// colors at: https://graphviz.org/doc/info/colors.html
string dot_color_map[680] = {
  "aliceblue", "antiquewhite", "antiquewhite1", "antiquewhite2", "antiquewhite3",
  "antiquewhite4", "aqua", "aquamarine", "aquamarine1", "aquamarine2",
  "aquamarine3", "aquamarine4", "azure", "azure1", "azure2",
  "azure3", "azure4", "beige", "bisque", "bisque1",
  "bisque2", "bisque3", "bisque4", "black", "blanchedalmond",
  "blue", "blue1", "blue2", "blue3", "blue4",
  "blueviolet", "brown", "brown1", "brown2", "brown3",
  "brown4", "burlywood", "burlywood1", "burlywood2", "burlywood3",
  "burlywood4", "cadetblue", "cadetblue1", "cadetblue2", "cadetblue3",
  "cadetblue4",         "chartreuse",   "chartreuse1",  "chartreuse2",  "chartreuse3",
  "chartreuse4",        "chocolate",    "chocolate1",   "chocolate2",   "chocolate3",
  "chocolate4",         "coral",        "coral1",       "coral2",       "coral3",
  "coral4",     "cornflowerblue",       "cornsilk",     "cornsilk1",    "cornsilk2",
  "cornsilk3",  "cornsilk4",    "crimson",      "cyan",         "cyan1",
  "cyan2",      "cyan3",        "cyan4",        "darkblue",     "darkcyan",
  "darkgoldenrod",      "darkgoldenrod1",       "darkgoldenrod2",       "darkgoldenrod3",       "darkgoldenrod4",
  "darkgray",   "darkgreen",    "darkgrey",     "darkkhaki",    "darkmagenta",
  "darkolivegreen",     "darkolivegreen1",      "darkolivegreen2",      "darkolivegreen3",      "darkolivegreen4",
  "darkorange",         "darkorange1",  "darkorange2",  "darkorange3",  "darkorange4",
  "darkorchid",         "darkorchid1",  "darkorchid2",  "darkorchid3",  "darkorchid4",
  "darkred",    "darksalmon",   "darkseagreen",         "darkseagreen1",        "darkseagreen2",
  "darkseagreen3",      "darkseagreen4",        "darkslateblue",        "darkslategray",        "darkslategray1",
  "darkslategray2",     "darkslategray3",       "darkslategray4",       "darkslategrey",        "darkturquoise",
  "darkviolet",         "deeppink",     "deeppink1",    "deeppink2",    "deeppink3",
  "deeppink4",  "deepskyblue",  "deepskyblue1",         "deepskyblue2",         "deepskyblue3",
  "deepskyblue4",       "dimgray",      "dimgrey",      "dodgerblue",   "dodgerblue1",
  "dodgerblue2",        "dodgerblue3",  "dodgerblue4",  "firebrick",    "firebrick1",
  "firebrick2",         "firebrick3",   "firebrick4",   "floralwhite",  "forestgreen",
  "fuchsia",    "gainsboro",    "ghostwhite",   "gold",         "gold1",
  "gold2",      "gold3",        "gold4",        "goldenrod",    "goldenrod1",
  "goldenrod2",         "goldenrod3",   "goldenrod4",   "gray",         "gray0",
  "gray1",      "gray10",       "gray100",      "gray11",       "gray12",
  "gray13",     "gray14",       "gray15",       "gray16",       "gray17",
  "gray18",     "gray19",       "gray2",        "gray20",       "gray21",
  "gray22",     "gray23",       "gray24",       "gray25",       "gray26",
  "gray27",     "gray28",       "gray29",       "gray3",        "gray30",
  "gray31",     "gray32",       "gray33",       "gray34",       "gray35",
  "gray36",     "gray37",       "gray38",       "gray39",       "gray4",
  "gray40",     "gray41",       "gray42",       "gray43",       "gray44",
  "gray45",     "gray46",       "gray47",       "gray48",       "gray49",
  "gray5",      "gray50",       "gray51",       "gray52",       "gray53",
  "gray54",     "gray55",       "gray56",       "gray57",       "gray58",
  "gray59",     "gray6",        "gray60",       "gray61",       "gray62",
  "gray63",     "gray64",       "gray65",       "gray66",       "gray67",
  "gray68",     "gray69",       "gray7",        "gray70",       "gray71",
  "gray72",     "gray73",       "gray74",       "gray75",       "gray76",
  "gray77",     "gray78",       "gray79",       "gray8",        "gray80",
  "gray81",     "gray82",       "gray83",       "gray84",       "gray85",
  "gray86",     "gray87",       "gray88",       "gray89",       "gray9",
  "gray90",     "gray91",       "gray92",       "gray93",       "gray94",
  "gray95",     "gray96",       "gray97",       "gray98",       "gray99",
  "green",      "green1",       "green2",       "green3",       "green4",
  "greenyellow",        "grey",         "grey0",        "grey1",        "grey10",
  "grey100",    "grey11",       "grey12",       "grey13",       "grey14",
  "grey15",     "grey16",       "grey17",       "grey18",       "grey19",
  "grey2",      "grey20",       "grey21",       "grey22",       "grey23",
  "grey24",     "grey25",       "grey26",       "grey27",       "grey28",
  "grey29",     "grey3",        "grey30",       "grey31",       "grey32",
  "grey33",     "grey34",       "grey35",       "grey36",       "grey37",
  "grey38",     "grey39",       "grey4",        "grey40",       "grey41",
  "grey42",     "grey43",       "grey44",       "grey45",       "grey46",
  "grey47",     "grey48",       "grey49",       "grey5",        "grey50",
  "grey51",     "grey52",       "grey53",       "grey54",       "grey55",
  "grey56",     "grey57",       "grey58",       "grey59",       "grey6",
  "grey60",     "grey61",       "grey62",       "grey63",       "grey64",
  "grey65",     "grey66",       "grey67",       "grey68",       "grey69",
  "grey7",      "grey70",       "grey71",       "grey72",       "grey73",
  "grey74",     "grey75",       "grey76",       "grey77",       "grey78",
  "grey79",     "grey8",        "grey80",       "grey81",       "grey82",
  "grey83",     "grey84",       "grey85",       "grey86",       "grey87",
  "grey88",     "grey89",       "grey9",        "grey90",       "grey91",
  "grey92",     "grey93",       "grey94",       "grey95",       "grey96",
  "grey97",     "grey98",       "grey99",       "honeydew",     "honeydew1",
  "honeydew2",  "honeydew3",    "honeydew4",    "hotpink",      "hotpink1",
  "hotpink2",   "hotpink3",     "hotpink4",     "indianred",    "indianred1",
  "indianred2",         "indianred3",   "indianred4",   "indigo",       "invis",
  "ivory",      "ivory1",       "ivory2",       "ivory3",       "ivory4",
  "khaki",      "khaki1",       "khaki2",       "khaki3",       "khaki4",
  "lavender",   "lavenderblush",        "lavenderblush1",       "lavenderblush2",       "lavenderblush3",
  "lavenderblush4",     "lawngreen",    "lemonchiffon",         "lemonchiffon1",        "lemonchiffon2",
  "lemonchiffon3",      "lemonchiffon4",        "lightblue",    "lightblue1",   "lightblue2",
  "lightblue3",         "lightblue4",   "lightcoral",   "lightcyan",    "lightcyan1",
  "lightcyan2",         "lightcyan3",   "lightcyan4",   "lightgoldenrod",       "lightgoldenrod1",
  "lightgoldenrod2",    "lightgoldenrod3",      "lightgoldenrod4",      "lightgoldenrodyellow",         "lightgray",
  "lightgreen",         "lightgrey",    "lightpink",    "lightpink1",   "lightpink2",
  "lightpink3",         "lightpink4",   "lightsalmon",  "lightsalmon1",         "lightsalmon2",
  "lightsalmon3",       "lightsalmon4",         "lightseagreen",        "lightskyblue",         "lightskyblue1",
  "lightskyblue2",      "lightskyblue3",        "lightskyblue4",        "lightslateblue",       "lightslategray",
  "lightslategrey",     "lightsteelblue",       "lightsteelblue1",      "lightsteelblue2",      "lightsteelblue3",
  "lightsteelblue4",    "lightyellow",  "lightyellow1",         "lightyellow2",         "lightyellow3",
  "lightyellow4",       "lime",         "limegreen",    "linen",        "magenta",
  "magenta1",   "magenta2",     "magenta3",     "magenta4",     "maroon",
  "maroon1",    "maroon2",      "maroon3",      "maroon4",      "mediumaquamarine",
  "mediumblue",         "mediumorchid",         "mediumorchid1",        "mediumorchid2",        "mediumorchid3",
  "mediumorchid4",      "mediumpurple",         "mediumpurple1",        "mediumpurple2",        "mediumpurple3",
  "mediumpurple4",      "mediumseagreen",       "mediumslateblue",      "mediumspringgreen",    "mediumturquoise",
  "mediumvioletred",    "midnightblue",         "mintcream",    "mistyrose",    "mistyrose1",
  "mistyrose2",         "mistyrose3",   "mistyrose4",   "moccasin",     "navajowhite",
  "navajowhite1",       "navajowhite2",         "navajowhite3",         "navajowhite4",         "navy",
  "navyblue",   "none",         "oldlace",      "olive",        "olivedrab",
  "olivedrab1",         "olivedrab2",   "olivedrab3",   "olivedrab4",   "orange",
  "orange1",    "orange2",      "orange3",      "orange4",      "orangered",
  "orangered1",         "orangered2",   "orangered3",   "orangered4",   "orchid",
  "orchid1",    "orchid2",      "orchid3",      "orchid4",      "palegoldenrod",
  "palegreen",  "palegreen1",   "palegreen2",   "palegreen3",   "palegreen4",
  "paleturquoise",      "paleturquoise1",       "paleturquoise2",       "paleturquoise3",       "paleturquoise4",
  "palevioletred",      "palevioletred1",       "palevioletred2",       "palevioletred3",       "palevioletred4",
  "papayawhip",         "peachpuff",    "peachpuff1",   "peachpuff2",   "peachpuff3",
  "peachpuff4",         "peru",         "pink",         "pink1",        "pink2",
  "pink3",      "pink4",        "plum",         "plum1",        "plum2",
  "plum3",      "plum4",        "powderblue",   "purple",       "purple1",
  "purple2",    "purple3",      "purple4",      "rebeccapurple",        "red",
  "red1",       "red2",         "red3",         "red4",         "rosybrown",
  "rosybrown1",         "rosybrown2",   "rosybrown3",   "rosybrown4",   "royalblue",
  "royalblue1",         "royalblue2",   "royalblue3",   "royalblue4",   "saddlebrown",
  "salmon",     "salmon1",      "salmon2",      "salmon3",      "salmon4",
  "sandybrown",         "seagreen",     "seagreen1",    "seagreen2",    "seagreen3",
  "seagreen4",  "seashell",     "seashell1",    "seashell2",    "seashell3",
  "seashell4",  "sienna",       "sienna1",      "sienna2",      "sienna3",
  "sienna4",    "silver",       "skyblue",      "skyblue1",     "skyblue2",
  "skyblue3",   "skyblue4",     "slateblue",    "slateblue1",   "slateblue2",
  "slateblue3",         "slateblue4",   "slategray",    "slategray1",   "slategray2",
  "slategray3",         "slategray4",   "slategrey",    "snow",         "snow1",
  "snow2",      "snow3",        "snow4",        "springgreen",  "springgreen1",
  "springgreen2",       "springgreen3",         "springgreen4",         "steelblue",    "steelblue1",
  "steelblue2",         "steelblue3",   "steelblue4",   "tan",  "tan1",
  "tan2",       "tan3",         "tan4",         "teal",         "thistle",
  "thistle1",   "thistle2",     "thistle3",     "thistle4",     "tomato",
  "tomato1",    "tomato2",      "tomato3",      "tomato4",      "transparent",
  "turquoise",  "turquoise1",   "turquoise2",   "turquoise3",   "turquoise4",
  "violet",     "violetred",    "violetred1",   "violetred2",   "violetred3",
  "violetred4",         "webgray",      "webgreen",     "webgrey",      "webmaroon",
  "webpurple",  "wheat",        "wheat1",       "wheat2",       "wheat3",
  "wheat4",     "white",        "whitesmoke",   "x11gray",      "x11green",
  "x11grey",    "x11maroon",    "x11purple",    "yellow",       "yellow1",
  "yellow2",    "yellow3",      "yellow4",      "yellowgreen"
};


// DQ (5/23/2021): Added more dark colors to the dark color map.
string dot_dark_color_map[] = { 
  "aqua",        "aquamarine4",    "darkorange2",      "azure4",       "bisque3", "blue", 
  "blueviolet",        "brown",      "cadetblue",  "chartreuse",        "coral3", "cornflowerblue", 
  "crimson",           "cyan4",       "darkblue",    "darkcyan", "darkgoldenrod", "darkgray",
  "darkgreen",   "darkmagenta", "darkolivegreen",  "darkorange",    "darkorchid", "darkred",
  "darkturquoise", "deeppink3",    "deepskyblue",  "dodgerblue",     "firebrick", "fuchsia",
  "gold",          "goldenrod",          "black",       "green",   "greenyellow", "grey",
  "hotpink",          "indigo",          "khaki",     "magenta",        "maroon", "midnightblue",
  "navajowhite",        "navy",         "orchid",        "peru",          "plum", "powderblue",
  "purple",              "red",      "royalblue", "saddlebrown",      "seagreen", "silver",
  "skyblue",       "slateblue",    "springgreen",   "steelblue",           "tan", "teal",
  "turquoise",     "violetred",          "wheat",      "wheat3",        "yellow", "yellowgreen"
};


string select_dot_dark_color( int & value )
   {
     string return_string;

  // Make sure that we don't access the array out of bounds.
     ROSE_ASSERT(value >= 0);

  // DQ (5/23/2021): Allow the input values to be unbounded, and we will wrap the color pallet around the input values.
  // ROSE_ASSERT(value <= 66);

     return_string = dot_dark_color_map[value % 66];

#if DEBUG_COLOR_SELECT
     printf ("Leaving select_dot_dark_color(): value = %d return_string = %s \n",value,return_string.c_str());
#endif

     return return_string;
   }


string select_dot_color( int & value )
   {
     string return_string;

  // Make sure that we don't access the array out of bounds.
     ROSE_ASSERT(value >= 0);
     ROSE_ASSERT(value <= 678);

     return_string = dot_color_map[value % 680];

#if DEBUG_COLOR_SELECT
     printf ("Leaving select_dot_color(): value = %d return_string = %s \n",value,return_string.c_str());
#endif

     return return_string;
   }


string select_unique_dot_color( int & value )
   {
  // This function returns a string (color) for use in DOT graphs.

#define DEBUG_COLOR_SELECT 0

  // Make sure that we don't access the array out of bounds.
     ROSE_ASSERT(value >= 0);
     ROSE_ASSERT(value <= 678);

     int initial_value = value % 679;
     string return_string;

     return_string = dot_color_map[value % 679];

#if DEBUG_COLOR_SELECT
     printf ("In select_unique_dot_color(): value = %d initial_value = %d return_string = %s \n",value,initial_value,return_string.c_str());
#endif

     if (isdigit(return_string[return_string.length()-1]) == true)
        {
#if DEBUG_COLOR_SELECT
          printf ("Found color string with trailing digit \n");
#endif
          string test = return_string;
          size_t last_char_pos = test.find_last_not_of("0123456789");
          string base = test.substr(0, last_char_pos + 1);
          return_string = base;

       // increment the initial_value to the next color that does not have a trailing digit.
          int test_value = initial_value;
          string test_string = dot_color_map[test_value];
          while (test_value < 679 && isdigit(test_string[test_string.length()-1]) == true)
            {
#if DEBUG_COLOR_SELECT
              printf ("TOP of loop: test_value = %d test_string = %s \n",test_value,test_string.c_str());
#endif
              test_value++;
              test_string = dot_color_map[test_value];

#if DEBUG_COLOR_SELECT
              printf ("BOTTOM of loop: test_value = %d test_string = %s \n",test_value,test_string.c_str());
#endif
            }

       // Update the value and return_string.
          value = test_value;
          return_string = test_string;
        }

#if DEBUG_COLOR_SELECT
     printf ("Leaving select_unique_dot_color(): value = %d return_string = %s \n",value,return_string.c_str());
#endif

     return return_string;
   }


FrontierDetectionForTokenStreamMapping_InheritedAttribute::
FrontierDetectionForTokenStreamMapping_InheritedAttribute()
   {
     sourceFile        = NULL;
     processChildNodes = false;

     isFrontier = false;

     unparseUsingTokenStream             = false;
     unparseFromTheAST                   = false;
  // containsNodesToBeUnparsedFromTheAST = false;

  // DQ (5/11/2021): Added to support header file unparsing.
     isInCurrentFile = true;
     node            = NULL;

  // DQ (5/23/2021): Added to support C++.
     isPartOfTemplateInstantiation = false;

  // DQ (5/12/2021): Added to support header file unparsing.
  // subtree_color_index = STARTING_SUBTREE_COLOR_INDEX;
  // subtree_color = select_unique_dot_color(subtree_color_index);
  // subtree_color = select_dot_dark_color(subtree_color_index);

  // isPartOfTypedefDeclaration   = false;
  // isPartOfConditionalStatement = false;
   }

// FrontierDetectionForTokenStreamMapping_InheritedAttribute::
// FrontierDetectionForTokenStreamMapping_InheritedAttribute( SgSourceFile* file, SgNode* n, int color_index )
FrontierDetectionForTokenStreamMapping_InheritedAttribute::
FrontierDetectionForTokenStreamMapping_InheritedAttribute( SgSourceFile* file, SgNode* n )
   {
  // DQ (5/11/2021): We need to get the sourceFile.
  // sourceFile        = NULL;
  // sourceFile        = isSgSourceFile(n);
     sourceFile        = file;

     ROSE_ASSERT(sourceFile != NULL);

     processChildNodes = false;

     isFrontier = false;

     unparseUsingTokenStream             = false;
     unparseFromTheAST                   = false;
  // containsNodesToBeUnparsedFromTheAST = false;

  // DQ (5/11/2021): Added to support header file unparsing.
     isInCurrentFile = true;
     node            = n;

  // DQ (5/23/2021): Added to support C++.
     isPartOfTemplateInstantiation = false;

  // DQ (5/12/2021): Added to support header file unparsing.
  // subtree_color_index = STARTING_SUBTREE_COLOR_INDEX;
  // subtree_color_index = color_index;
  // subtree_color = select_unique_dot_color(subtree_color_index);
  // subtree_color = select_dot_dark_color(subtree_color_index);

  // isPartOfTypedefDeclaration   = false;
  // isPartOfConditionalStatement = false;
   }

FrontierDetectionForTokenStreamMapping_InheritedAttribute::
FrontierDetectionForTokenStreamMapping_InheritedAttribute( SgSourceFile* file )
   {
  // DQ (5/11/2021): We need to get the sourceFile.
  // sourceFile        = NULL;
     sourceFile        = file;

     ROSE_ASSERT(sourceFile != NULL);

     processChildNodes = false;

     isFrontier = false;

     unparseUsingTokenStream             = false;
     unparseFromTheAST                   = false;
  // containsNodesToBeUnparsedFromTheAST = false;

  // DQ (5/11/2021): Added to support header file unparsing.
     isInCurrentFile = true;
     node            = file;

  // DQ (5/23/2021): Added to support C++.
     isPartOfTemplateInstantiation = false;

  // DQ (5/12/2021): Added to support header file unparsing.
  // subtree_color_index = STARTING_SUBTREE_COLOR_INDEX;
  // subtree_color = select_unique_dot_color(subtree_color_index);
  // subtree_color = select_dot_dark_color(subtree_color_index);

  // isPartOfTypedefDeclaration   = false;
  // isPartOfConditionalStatement = false;
   }

#if 0
// DQ (5/11/2021): I don't think this is used.
FrontierDetectionForTokenStreamMapping_InheritedAttribute::
FrontierDetectionForTokenStreamMapping_InheritedAttribute(SgSourceFile* input_sourceFile, int start, int end, bool processed)
   {
     sourceFile        = input_sourceFile;
     processChildNodes = processed;

     isFrontier = false;

     unparseUsingTokenStream             = false;
     unparseFromTheAST                   = false;
  // containsNodesToBeUnparsedFromTheAST = false;

  // DQ (5/11/2021): Added to support header file unparsing.
     isInCurrentFile = true;
     node            = input_sourceFile;

  // isPartOfTypedefDeclaration   = false;
  // isPartOfConditionalStatement = false;
   }
#endif

FrontierDetectionForTokenStreamMapping_InheritedAttribute::
FrontierDetectionForTokenStreamMapping_InheritedAttribute ( const FrontierDetectionForTokenStreamMapping_InheritedAttribute & X )
   {
     sourceFile        = X.sourceFile;
     processChildNodes = X.processChildNodes;

     isFrontier = X.isFrontier;

     unparseUsingTokenStream             = X.unparseUsingTokenStream;
     unparseFromTheAST                   = X.unparseFromTheAST;
  // containsNodesToBeUnparsedFromTheAST = X.containsNodesToBeUnparsedFromTheAST;

  // DQ (5/11/2021): Added to support header file unparsing.
     isInCurrentFile = X.isInCurrentFile;
     node            = X.node;

  // DQ (5/23/2021): Added to support C++.
     isPartOfTemplateInstantiation = X.isPartOfTemplateInstantiation;

  // DQ (5/12/2021): Added to support header file unparsing.
  // subtree_color_index = X.subtree_color_index;
  // subtree_color       = X.subtree_color;

  // isPartOfTypedefDeclaration   = X.isPartOfTypedefDeclaration;
  // isPartOfConditionalStatement = X.isPartOfConditionalStatement;
   }

FrontierDetectionForTokenStreamMapping_InheritedAttribute
FrontierDetectionForTokenStreamMapping_InheritedAttribute::operator= ( const FrontierDetectionForTokenStreamMapping_InheritedAttribute & X )
   {
     sourceFile        = X.sourceFile;
     processChildNodes = X.processChildNodes;

     isFrontier = X.isFrontier;

     unparseUsingTokenStream             = X.unparseUsingTokenStream;
     unparseFromTheAST                   = X.unparseFromTheAST;
  // containsNodesToBeUnparsedFromTheAST = X.containsNodesToBeUnparsedFromTheAST;

  // DQ (5/11/2021): Added to support header file unparsing.
     isInCurrentFile = X.isInCurrentFile;
     node            = X.node;

  // DQ (5/23/2021): Added to support C++.
     isPartOfTemplateInstantiation = X.isPartOfTemplateInstantiation;

  // DQ (5/12/2021): Added to support header file unparsing.
  // subtree_color_index = X.subtree_color_index;
  // subtree_color       = X.subtree_color;

  // isPartOfTypedefDeclaration   = X.isPartOfTypedefDeclaration;
  // isPartOfConditionalStatement = X.isPartOfConditionalStatement;

     return *this;
   }


FrontierDetectionForTokenStreamMapping_SynthesizedAttribute::
FrontierDetectionForTokenStreamMapping_SynthesizedAttribute()
   {
     node       = NULL;
     isFrontier = false;

  // DQ (5/14/2021): I think that we need the sourceFile information to support header file unparsing.
     sourceFile = NULL;

     unparseUsingTokenStream             = false;
     unparseFromTheAST                   = false;
     containsNodesToBeUnparsedFromTheAST = false;

     containsNodesToBeUnparsedFromTheTokenStream = false;
   }

// FrontierDetectionForTokenStreamMapping_SynthesizedAttribute::
// FrontierDetectionForTokenStreamMapping_SynthesizedAttribute(SgNode* n)
FrontierDetectionForTokenStreamMapping_SynthesizedAttribute::
  FrontierDetectionForTokenStreamMapping_SynthesizedAttribute(SgNode* n, SgSourceFile* file)
   {
     ROSE_ASSERT(n    != NULL);
     ROSE_ASSERT(file != NULL);

     node       = isSgStatement(n);
     isFrontier = false;

  // DQ (5/14/2021): I think that we need the sourceFile information to support header file unparsing.
     sourceFile = file;

     unparseUsingTokenStream             = false;
     unparseFromTheAST                   = false;
     containsNodesToBeUnparsedFromTheAST = false;

     containsNodesToBeUnparsedFromTheTokenStream = false;
   }

FrontierDetectionForTokenStreamMapping_SynthesizedAttribute::
FrontierDetectionForTokenStreamMapping_SynthesizedAttribute(const FrontierDetectionForTokenStreamMapping_SynthesizedAttribute & X)
   {
     node       = X.node;
     isFrontier = X.isFrontier;

  // DQ (5/14/2021): I think that we need the sourceFile information to support header file unparsing.
     sourceFile = X.sourceFile;

  // DQ (5/16/2021): Moved to the traversal class.
  // frontierNodes = X.frontierNodes;

     unparseUsingTokenStream             = X.unparseUsingTokenStream;
     unparseFromTheAST                   = X.unparseFromTheAST;
     containsNodesToBeUnparsedFromTheAST = X.containsNodesToBeUnparsedFromTheAST;

     containsNodesToBeUnparsedFromTheTokenStream = X.containsNodesToBeUnparsedFromTheTokenStream;
   }


FrontierDetectionForTokenStreamMapping_SynthesizedAttribute
FrontierDetectionForTokenStreamMapping_SynthesizedAttribute::operator=(const FrontierDetectionForTokenStreamMapping_SynthesizedAttribute & X)
   {
#if 0
     printf ("In FrontierDetectionForTokenStreamMapping_SynthesizedAttribute::operator=() \n");
#endif

     node       = X.node;
     isFrontier = X.isFrontier;

  // DQ (5/14/2021): I think that we need the sourceFile information to support header file unparsing.
     sourceFile = X.sourceFile;

#if 0
     printf ("In FrontierDetectionForTokenStreamMapping_SynthesizedAttribute::operator=(): calling map::operator=() \n");
#endif

  // DQ (5/16/2021): Moved to the traversal class.
  // frontierNodes = X.frontierNodes;

#if 0
     printf ("DONE: In FrontierDetectionForTokenStreamMapping_SynthesizedAttribute::operator=(): calling map::operator=() \n");
#endif

     unparseUsingTokenStream             = X.unparseUsingTokenStream;
     unparseFromTheAST                   = X.unparseFromTheAST;
     containsNodesToBeUnparsedFromTheAST = X.containsNodesToBeUnparsedFromTheAST;

     containsNodesToBeUnparsedFromTheTokenStream = X.containsNodesToBeUnparsedFromTheTokenStream;

#if 0
     printf ("Leaving FrontierDetectionForTokenStreamMapping_SynthesizedAttribute::operator=() \n");
#endif

     return *this;
   }


FrontierDetectionForTokenStreamMapping::
FrontierDetectionForTokenStreamMapping( SgSourceFile* sourceFile)
   {
  // This is the number of IR nodes in the current file and helpful in randomly 
  // marking IR nodes for testing to be either from the AST or from the token stream.
     numberOfNodes = numberOfNodesInSubtree(sourceFile);
   }


#if 0
std::vector<AstAttribute::AttributeNodeInfo>
FrontierDetectionForTokenStreamMapping::additionalNodeInfo()
   {
     vector<AstAttribute::AttributeNodeInfo> v;

     string vulnerabilityName    = " " + vulnerabilityPointer->get_name();
     string vulnerabilityColor   = vulnerabilityPointer->get_color();
     string vulnerabilityOptions = " arrowsize=7.0 style=\"setlinewidth(7)\" constraint=false fillcolor=" + vulnerabilityColor + ",style=filled ";

  // AstAttribute::AttributeNodeInfo vulnerabilityNode ( (SgNode*) vulnerabilityPointer, "SecurityVulnerabilityAttribute"," fillcolor=\"red\",style=filled ");
     AstAttribute::AttributeNodeInfo vulnerabilityNode ( (SgNode*) vulnerabilityPointer, vulnerabilityName, vulnerabilityOptions);
     v.push_back(vulnerabilityNode);

     return v;
   }
#endif


#if 0
int* target_physicial_file_id_ptr = NULL;
SgFunctionDeclaration* target_function_declaration_ptr = NULL;
#endif

int
generate_physical_file_id ( SgStatement* statement )
   {
     ROSE_ASSERT(statement != NULL);

     int physical_file_id = statement->get_file_info()->get_physical_file_id();

#if 0
  // DQ (5/20/2021): Debugging support.
     printf ("In generate_physical_file_id(): target_physicial_file_id_ptr = %p \n",target_physicial_file_id_ptr);
     if (target_physicial_file_id_ptr != NULL)
        {
          printf ("In generate_physical_file_id(): *target_physicial_file_id_ptr = %d \n",*target_physicial_file_id_ptr);
        }
#endif

     if (physical_file_id < 0)
        {
       // We need to translate any nodes that generate a negative physical_file_id 
       // to a non-negative value (usually by looking at the parent node).

          SgFunctionParameterList* functionParameterList = isSgFunctionParameterList(statement);
          if (functionParameterList != NULL)
             {
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(statement->get_parent());
               ROSE_ASSERT(functionDeclaration != NULL);

               int statement_physical_file_id = functionDeclaration->get_file_info()->get_physical_file_id();
#if 0
               printf ("In generate_physical_file_id(): case of SgFunctionParameterList: statement_physical_file_id = %d \n",statement_physical_file_id);
#endif
               physical_file_id = statement_physical_file_id;
             }

          SgCtorInitializerList* ctorInitializerList = isSgCtorInitializerList(statement);
          if (ctorInitializerList != NULL)
             {
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(statement->get_parent());
               ROSE_ASSERT(functionDeclaration != NULL);

               int statement_physical_file_id = functionDeclaration->get_file_info()->get_physical_file_id();
#if 0
               printf ("In generate_physical_file_id(): case of SgCtorInitializerList: statement_physical_file_id = %d \n",statement_physical_file_id);
#endif
               physical_file_id = statement_physical_file_id;
             }

       // DQ (5/22/2021): Added to support test2012_20.c test code.
          SgNullStatement* nullStatement = isSgNullStatement(statement);
          if (nullStatement != NULL)
             {
               SgLocatedNode* locatedNode = isSgLocatedNode(statement->get_parent());
               ROSE_ASSERT(locatedNode != NULL);

               int statement_physical_file_id = locatedNode->get_file_info()->get_physical_file_id();
#if 0
               printf ("In generate_physical_file_id(): case of SgNullStatement: statement_physical_file_id = %d \n",statement_physical_file_id);
#endif
               physical_file_id = statement_physical_file_id;
             }

       // DQ (5/22/2021): Added to fix issue in test2012_84.c
          SgBasicBlock* basicBlock = isSgBasicBlock(statement);
          if (basicBlock != NULL)
             {
               ROSE_ASSERT(basicBlock->get_parent() != NULL);
               SgLocatedNode* basicBlock_parent = isSgLocatedNode(basicBlock->get_parent());
               ROSE_ASSERT(basicBlock_parent != NULL);
               int statement_physical_file_id = basicBlock_parent->get_file_info()->get_physical_file_id();
#if 0
               printf ("In generate_physical_file_id(): physical_file_id < 0: case of SgBasicBlock: using parent: statement_physical_file_id = %d \n",statement_physical_file_id);
#endif
               physical_file_id = statement_physical_file_id;
             }

       // DQ (5/23/2021): Adding support for namespace definitions.
          SgNamespaceDefinitionStatement* namespaceDefinition = isSgNamespaceDefinitionStatement(statement);
          if (namespaceDefinition != NULL)
             {
               SgNamespaceDeclarationStatement* namespaceDeclaration = isSgNamespaceDeclarationStatement(namespaceDefinition->get_parent());
               ROSE_ASSERT(namespaceDeclaration != NULL);

               int statement_physical_file_id = namespaceDeclaration->get_file_info()->get_physical_file_id();
#if 0
               printf ("In generate_physical_file_id(): case of SgNamespaceDefinitionStatement: statement_physical_file_id = %d \n",statement_physical_file_id);
#endif
               physical_file_id = statement_physical_file_id;
             }

       // DQ (5/23/2021): Adding support for try statements.
          SgTryStmt* tryStatement = isSgTryStmt(statement);
          if (tryStatement != NULL)
             {
               SgLocatedNode* tryStatement_parent = isSgLocatedNode(tryStatement->get_parent());
               ROSE_ASSERT(tryStatement_parent != NULL);

               int statement_physical_file_id = tryStatement_parent->get_file_info()->get_physical_file_id();
#if 0
               printf ("In generate_physical_file_id(): case of SgTryStmt: using parent: tryStatement_parent = %p = %s statement_physical_file_id = %d \n",
                    tryStatement_parent,tryStatement_parent->class_name().c_str(),statement_physical_file_id);
#endif
               physical_file_id = statement_physical_file_id;
             }

       // DQ (5/23/2021): Adding support for try statements.
          SgCatchStatementSeq* catchStatement = isSgCatchStatementSeq(statement);
          if (catchStatement != NULL)
             {
               SgLocatedNode* catchStatement_parent = isSgLocatedNode(catchStatement->get_parent());
               ROSE_ASSERT(catchStatement_parent != NULL);

            // int statement_physical_file_id = tryStatement_parent->get_file_info()->get_physical_file_id();
            // int statement_physical_file_id = generate_physical_file_id(tryStatement_parent);
               int statement_physical_file_id = catchStatement_parent->get_file_info()->get_physical_file_id();
#if 0
               printf ("In generate_physical_file_id(): case of SgCatchStatementSeq: using parent: catchStatement_parent = %p = %s statement_physical_file_id = %d \n",
                    catchStatement_parent,catchStatement_parent->class_name().c_str(),statement_physical_file_id);
#endif
               physical_file_id = statement_physical_file_id;
             }

       // DQ (5/23/2021): Adding support for try statements.
          SgCatchOptionStmt* catchOptionStatement = isSgCatchOptionStmt(statement);
          if (catchOptionStatement != NULL)
             {
               SgStatement* catchOptionStatement_parent = isSgStatement(catchOptionStatement->get_parent());
               ROSE_ASSERT(catchOptionStatement_parent != NULL);

            // int statement_physical_file_id = tryStatement_parent->get_file_info()->get_physical_file_id();
            // int statement_physical_file_id = generate_physical_file_id(tryStatement_parent);
            // int statement_physical_file_id = catchOptionStatement_parent->get_file_info()->get_physical_file_id();
               int statement_physical_file_id = generate_physical_file_id(catchOptionStatement_parent);
#if 0
               printf ("In generate_physical_file_id(): case of SgCatchOptionStmt: using parent: catchOptionStatement_parent = %p = %s statement_physical_file_id = %d \n",
                    catchOptionStatement_parent,catchOptionStatement_parent->class_name().c_str(),statement_physical_file_id);
#endif
               physical_file_id = statement_physical_file_id;
             }


       // DQ (5/23/2021): Adding support for template instantiations. Not clear how to handle this case.
          if (SageInterface::isTemplateInstantiationNode(statement) == true)
             {
               SgStatement* statement_parent = isSgStatement(statement->get_parent());
               ROSE_ASSERT(statement_parent != NULL);

               int statement_physical_file_id = generate_physical_file_id(statement_parent);
#if 0
               printf ("In generate_physical_file_id(): case of isTemplateInstantiationNode: using parent: statement_parent = %p = %s statement_physical_file_id = %d \n",
                    statement_parent,statement_parent->class_name().c_str(),statement_physical_file_id);
#endif
               physical_file_id = statement_physical_file_id;
#if 0
               printf ("I think we should not be visiting template instantiations, and so we don't need to handle this case! \n");

               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }

          if (physical_file_id < 0)
             {
#if 0
               printf ("Error: physical_file_id < 0: physical_file_id = %d \n",physical_file_id);
               printf (" --- statement = %p = %s \n",statement,statement->class_name().c_str());
            // statement->get_file_info()->display("Error: physical_file_id < 0: debug");
               printf ("statement->get_parent() = %p = %s \n",statement->get_parent(),statement->get_parent()->class_name().c_str());
#endif
               SgLocatedNode* locatedNode = isSgLocatedNode(statement->get_parent());
               ROSE_ASSERT(locatedNode != NULL);
#if 0
               printf ("In generate_physical_file_id(): physical_file_id < 0: statement->get_parent(): locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
#endif
#if 0
               locatedNode->get_file_info()->display("Error: parent of node with physical_file_id < 0: debug");
#endif
            // DQ (5/22/2021): Added to fix issue in test2012_20.c
               SgGlobal* globalScope = isSgGlobal(locatedNode);
               if (globalScope != NULL)
                  {
                    SgSourceFile* sourceFile = SageInterface::getEnclosingSourceFile(statement);
                    ROSE_ASSERT(sourceFile != NULL);

                    int statement_physical_file_id = locatedNode->get_file_info()->get_physical_file_id();
#if 0
                    printf ("In generate_physical_file_id(): physical_file_id < 0: case of SgGlobal: using SgSourceFile: statement_physical_file_id = %d \n",statement_physical_file_id);
#endif
                    physical_file_id = statement_physical_file_id;
                  }
             }

          if (physical_file_id < 0)
             {
#if 0
               printf ("Backup case: traverse parent list: physical_file_id < 0: physical_file_id = %d \n",physical_file_id);
               printf (" --- statement = %p = %s \n",statement,statement->class_name().c_str());
#endif
               SgStatement* statement_parent = isSgStatement(statement->get_parent());
               ROSE_ASSERT(statement_parent != NULL);

               int statement_physical_file_id = generate_physical_file_id(statement_parent);

               physical_file_id = statement_physical_file_id;
             }

          if (physical_file_id < 0)
             {
#if 0
               printf ("Error: physical_file_id < 0: physical_file_id = %d \n",physical_file_id);
               printf (" --- statement = %p = %s \n",statement,statement->class_name().c_str());
#endif
               statement->get_file_info()->display("Error: physical_file_id < 0: debug");
#if 0
               printf ("statement->get_parent() = %p = %s \n",statement->get_parent(),statement->get_parent()->class_name().c_str());
#endif
               SgLocatedNode* locatedNode = isSgLocatedNode(statement->get_parent());
               ROSE_ASSERT(locatedNode != NULL);

               locatedNode->get_file_info()->display("Error: parent of node with physical_file_id < 0: debug");

               SgGlobal* globalScope = isSgGlobal(locatedNode);
               if (globalScope != NULL)
                  {
                    SgSourceFile* sourceFile = SageInterface::getEnclosingSourceFile(statement);
                    ROSE_ASSERT(sourceFile != NULL);
#if 0
                    printf ("sourceFile->getFileName() = %s \n",sourceFile->getFileName().c_str());
#endif
                  }
#if 0
            // DQ (5/20/2021): Testing the frontier computation.
               ROSE_ASSERT(target_function_declaration_ptr != NULL);
               printf ("target_function_declaration_ptr = %p = %s name = %s \n",target_function_declaration_ptr,
                    target_function_declaration_ptr->class_name().c_str(),target_function_declaration_ptr->get_name().str());
               int target_function_declaration_physical_file_id = target_function_declaration_ptr->get_file_info()->get_physical_file_id();
               printf ("target_function_declaration_physical_file_id = %d \n",target_function_declaration_physical_file_id);
#endif
               ROSE_ASSERT(false);
             }
        }

     return physical_file_id;
   }


bool isFromSameFile(int physical_file_id_1, SgStatement* statement)
   {
  // This is a supporting function for the isChildNodeFromSameFileAsCurrentNode() and isNodeFromCurrentFile() functions.

     bool return_value = false;

     ROSE_ASSERT(statement != NULL);

  // DQ (5/22/2021): Use a single function for generating this.
  // int physical_file_id_2  = statement->get_file_info()->get_physical_file_id();
     int physical_file_id_2  = generate_physical_file_id(statement);

#if 0
     printf ("In isFromSameFile(): physical_file_id_1 = %d physical_file_id_2 = %d \n",physical_file_id_1,physical_file_id_2);
#endif

     return_value = (physical_file_id_1 == physical_file_id_2);

#if 0
  // DQ (5/12/2021): Assume everything in rose_edg_required_macros_and_functions.h is in the source file (testing).
     string filenameWithPath = statement->get_file_info()->get_filenameString();
     string filenameWithOutPath = Rose::utility_stripPathFromFileName(filenameWithPath);
     if (filenameWithOutPath == "rose_edg_required_macros_and_functions.h")
        {
          return_value = true;
#if 0
          printf ("In isFromSameFile(): reset for IR node in rose_edg_required_macros_and_functions.h: sourceFile_physical_file_id = %d statement_physical_file_id = %d \n",
               physical_file_id_1,physical_file_id_2);
#endif
        }
#endif

#if 0
  // DQ (5/22/2021): Use a single function for generating this (now using generate_physical_file_id()).
     SgFunctionParameterList* functionParameterList = isSgFunctionParameterList(statement);
     if (functionParameterList != NULL)
        {
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(statement->get_parent());
          ROSE_ASSERT(functionDeclaration != NULL);

          int functionParameterList_physical_file_id = functionDeclaration->get_file_info()->get_physical_file_id();

          return_value = (physical_file_id_1 == functionParameterList_physical_file_id);
#if 0
          printf ("In isFromSameFile(): reset for SgFunctionParameterList: physical_file_id_1 = %d functionParameterList_physical_file_id = %d \n",
               physical_file_id_1,functionParameterList_physical_file_id);
#endif
        }

     SgCtorInitializerList* ctorInitializerList = isSgCtorInitializerList(statement);
     if (ctorInitializerList != NULL)
        {
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(statement->get_parent());
          ROSE_ASSERT(functionDeclaration != NULL);

          int ctorInitializerList_physical_file_id = functionDeclaration->get_file_info()->get_physical_file_id();

          return_value = (physical_file_id_1 == ctorInitializerList_physical_file_id);
#if 0
          printf ("In isFromSameFile(): reset for SgCtorInitializerList: physical_file_id_1 = %d ctorInitializerList_physical_file_id = %d \n",
               physical_file_id_1,ctorInitializerList_physical_file_id);
#endif
        }
#endif

     return return_value;
   }


bool
FrontierDetectionForTokenStreamMapping::isChildNodeFromSameFileAsCurrentNode (SgStatement* statement, SgStatement* child_statement)
   {
  // Refactored code.

  // DQ (5/10/2021): Add test for if this child node is from the same physical file.
     bool return_value = false;
  // SgStatement* child_statement = synthesizedAttributeList[i].node;

     if (statement != NULL && child_statement != NULL)
        {
       // int child_statement_file_id    = child_statement->get_file_info()->get_physical_file_id();
       // return_value = isFromSameFile(child_statement_file_id,statement);

       // DQ (5/23/2021): Using generate_physical_file_id() function instead.
       // int statement_file_id = statement->get_file_info()->get_physical_file_id();
          int statement_file_id = generate_physical_file_id(statement);

          return_value = isFromSameFile(statement_file_id,child_statement);
        }
       else
        {
       // DQ (5/10/2021): I think we can assert this.
          if (statement == NULL)
             {
#if 0
               printf ("In isChildNodeFromSameFileAsCurrentNode(): statement == NULL: \n");
#endif
             }
       // A SgSourceFile can contain more than jut the SgGlobal (package, import_list, class_list; for other languages).
       // ROSE_ASSERT(statement != NULL);
        }

#if 0
  // DQ (5/11/2021): Force the value to be true to test this bhavior (appears to build more complete frontierNode list, with more than one entry).
     return_value = true;
#endif

#if 0
     printf ("Leaving isChildNodeFromSameFileAsCurrentNode(): return_value = %s \n",return_value ? "true" : "false");
#endif

     return return_value;
   }


bool
FrontierDetectionForTokenStreamMapping_InheritedAttribute::isNodeFromCurrentFile ( SgStatement* statement )
   {
     ROSE_ASSERT(sourceFile != NULL);
     ROSE_ASSERT(statement  != NULL);

     bool return_value = false;

     int sourceFile_physical_file_id = sourceFile->get_file_info()->get_physical_file_id();

     return_value = isFromSameFile(sourceFile_physical_file_id,statement);

#if 0
     printf ("Leaving isNodeFromCurrentFile(): return_value = %s \n",return_value ? "true" : "false");
#endif

     return return_value;
   }


FrontierDetectionForTokenStreamMapping_InheritedAttribute
FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(SgNode* n, FrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute)
   {
  // By default, all inherited attributes are marked as:
  //    isFrontier                                  = false
  //    unparseUsingTokenStream                     = true
  //    unparseFromTheAST                           = false
  //    containsNodesToBeUnparsedFromTheAST         = false (this is always false and never set; can be removed)
  // When a node that is marked as isTransformation() == true is seen, then we set isFrontier = true

     static int random_counter = 0;

#define DEBUG_INHERIT 0

     SgStatement* statement = isSgStatement(n);

  // DQ (23/2021): Avoid the overhead of testing isTemplateInstantiationNode() for IR nodes that are not at least declarations.
  // bool isTemplateInstantiationNode = SageInterface::isTemplateInstantiationNode(n);
     bool isTemplateInstantiationNode = false;
  // if (statement != NULL && isSgDeclarationStatement(statement) != NULL)
     if (statement != NULL)
        {
          isTemplateInstantiationNode = SageInterface::isTemplateInstantiationNode(n);
        }

#if DEBUG_INHERIT || 0
     printf ("\n\nIIIIIIIIIIIIIIIIIIIIIIIIII \n");
     printf ("*** In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): random_counter = %d n = %p = %s \n",random_counter,n,n->class_name().c_str());
     printf (" --- name ============================================= %s \n",SageInterface::get_name(n).c_str());
     printf (" --- isTemplateInstantiationNode(n)                   = %s \n",isTemplateInstantiationNode ? "true" : "false");
     printf (" --- inheritedAttribute.isPartOfTemplateInstantiation = %s \n",inheritedAttribute.isPartOfTemplateInstantiation ? "true" : "false");
     printf ("IIIIIIIIIIIIIIIIIIIIIIIIII \n");
#endif

#if 0
  // DQ (5/20/2021): Testing the frontier computation.
     ROSE_ASSERT(target_function_declaration_ptr != NULL);
     printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): target_function_declaration_ptr = %p = %s name = %s \n",target_function_declaration_ptr,
          target_function_declaration_ptr->class_name().c_str(),target_function_declaration_ptr->get_name().str());
     int target_function_declaration_physical_file_id = target_function_declaration_ptr->get_file_info()->get_physical_file_id();
     printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): target_function_declaration_physical_file_id = %d \n",target_function_declaration_physical_file_id);
     if (statement != NULL)
        {
          printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): statement->get_file_info()->get_physical_file_id() = %d \n",statement->get_file_info()->get_physical_file_id());
        }
#endif

#if DEBUG_INHERIT || 0
     ROSE_ASSERT(inheritedAttribute.sourceFile != NULL);
     printf ("inheritedAttribute.sourceFile              = %p \n",inheritedAttribute.sourceFile);
     printf ("inheritedAttribute.unparseFromTheAST       = %s \n",inheritedAttribute.unparseFromTheAST ? "true" : "false");
     printf ("inheritedAttribute.unparseUsingTokenStream = %s \n",inheritedAttribute.unparseUsingTokenStream ? "true" : "false");
#endif

  // DQ (5/20/2021): Also skip the SgSourceFile and the SgGlobalScope.
  // Ignore IR nodes that are front-end specific (declarations of builtin functions, etc.).
  // if (n->get_file_info()->isFrontendSpecific() == false)
  // if (n->get_file_info()->isFrontendSpecific() == false && statement != NULL)
     if (n->get_file_info()->isFrontendSpecific() == false && statement != NULL && isSgSourceFile(n) == NULL && isSgGlobal(n) == NULL)
        {
       // Count the IR nodes traversed so that we can make a subset transformations.
          random_counter++;
        }

     ROSE_ASSERT(inheritedAttribute.sourceFile != NULL);

#if DEBUG_INHERIT || 0
     printf ("Building returnAttribute: inheritedAttribute.sourceFile = %s \n",inheritedAttribute.sourceFile->getFileName().c_str());
#endif

     FrontierDetectionForTokenStreamMapping_InheritedAttribute returnAttribute(inheritedAttribute.sourceFile,n);

  // DQ (5/23/2021): Ading support to skip template instantiations (and all of the code in them which 
  // is all marked as compiler generated so it is difficult to associate a physical file id).
  // if (isTemplateInstantiationNode == true)
  // if (inheritedAttribute.isPartOfTemplateInstantiation == true)
     if (isTemplateInstantiationNode == true || inheritedAttribute.isPartOfTemplateInstantiation == true)
        {
#if 0
          printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): Detected a template instantiation: setting returnAttribute \n");
#endif
       // returnAttribute.isPartOfTemplateInstantiation = isTemplateInstantiationNode;
          returnAttribute.isPartOfTemplateInstantiation = true;

          ROSE_ASSERT(returnAttribute.isPartOfTemplateInstantiation == true);
        }

#if 0
     if (isSgInitializedName(n) != NULL)
        {
          SgInitializedName* initializedName = isSgInitializedName(n);
          if (initializedName->get_name() == "v5_rename_me_renamed")
             {
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
             }
        }
#endif

  // if (statement != NULL && random_counter > 30 && random_counter < 40)
  // if (statement != NULL)
  // if (statement != NULL && inheritedAttribute.isPartOfTemplateInstantiation == false)
     if (statement != NULL && isTemplateInstantiationNode == false && inheritedAttribute.isPartOfTemplateInstantiation == false)
        {
#if DEBUG_INHERIT
          printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): random_counter = %d statement = %p = %s \n",random_counter,statement,statement->class_name().c_str());
          printf (" --- statement = %s \n",SageInterface::get_name(statement).c_str());
#endif
#if 0
       // DQ (5/20/2021): Testing the frontier computation.
          ROSE_ASSERT(target_function_declaration_ptr != NULL);
          printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): statement != NULL: target_function_declaration_ptr = %p = %s name = %s \n",target_function_declaration_ptr,
               target_function_declaration_ptr->class_name().c_str(),target_function_declaration_ptr->get_name().str());
          int target_function_declaration_physical_file_id = target_function_declaration_ptr->get_file_info()->get_physical_file_id();
          printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): statement != NULL: target_function_declaration_physical_file_id = %d \n",target_function_declaration_physical_file_id);
          printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): statement != NULL: statement->get_file_info()->get_physical_file_id() = %d \n",
               statement->get_file_info()->get_physical_file_id());
          if (isSgFunctionDeclaration(statement) != NULL)
             {
               printf ("target_function_declaration_ptr->get_definingDeclaration()         = %p \n",target_function_declaration_ptr->get_definingDeclaration());
               printf ("target_function_declaration_ptr->get_firstNondefiningDeclaration() = %p \n",target_function_declaration_ptr->get_firstNondefiningDeclaration());

               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(statement);
               printf ("functionDeclaration->get_definingDeclaration()         = %p \n",functionDeclaration->get_definingDeclaration());
               printf ("functionDeclaration->get_firstNondefiningDeclaration() = %p \n",functionDeclaration->get_firstNondefiningDeclaration());
             }
#endif
       // string filename = statement->get_file_info()->get_filenameString();
       // int physical_file_id = statement->get_file_info()->get_physical_file_id();
          int physical_file_id = generate_physical_file_id(statement);

          ROSE_ASSERT(physical_file_id >= 0);

       // string filename = Sg_File_Info::getFilenameFromID(physical_file_id);
          string color = select_dot_dark_color(physical_file_id);

#if DEBUG_INHERIT
          printf ("color = %s \n",color.c_str());
#endif
          string name    = "token_frontier";
       // string options = "color=\"blue\"";
          string options = "penwidth=5, color=\"blue\"";

          options.replace(options.find("blue"), sizeof("blue") - 1, color);

          bool nodeIsFromCurrentFile = inheritedAttribute.isNodeFromCurrentFile(statement);

#if DEBUG_INHERIT || 0
          printf ("nodeIsFromCurrentFile = %s \n",nodeIsFromCurrentFile ? "true" : "false");
#endif
#if 0
       // DQ (5/12/2021): Code to test use of new color selection functionality.
          printf ("Evaluate the DOT color string values: \n");
          int color_index = 0;
          while (color_index < 679)
             {
#if 0
               string color = select_dot_color(color_index);
#else
               string color = select_unique_dot_color(color_index);
#endif
               printf (" --- color_index = %3d color = %s \n",color_index,color.c_str());
               color_index++;
             }
#if 1
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
#endif

       // This will later just check if this is a statement marked as a transformation or that it has been modified (checking the SgNode->isModified member flag).
       // if (random_counter > 30 && random_counter < 40)
       // int numberOfnode = numberOfNodes;

       // Mark the middle 50% of IR nodes to come from the AST, instead of the token stream.
       // int lowerbound = numberOfNodes / 4;
       // int upperbound = 3 * (numberOfNodes / 4);
          int lowerbound = -1;
          int upperbound = -1;
#if 0
#if 0
       // Unparse wholely from the token stream.
          bool forceUnparseFromTokenStream = true;
          bool forceUnparseFromAST         = false; // (forceUnparseFromTokenStream == false);
#else
       // Unparse wholely from the AST.
          bool forceUnparseFromTokenStream = false;
          bool forceUnparseFromAST         = true;
#endif
#else
       // Unparse the middle of the AST from the AST and the outer 25% from the token stream.
          bool forceUnparseFromTokenStream = false;
          bool forceUnparseFromAST         = false;
#endif

#if DEBUG_INHERIT
          printf ("ROSE_tokenUnparsingTestingMode = %s \n",ROSE_tokenUnparsingTestingMode ? "true" : "false");
#endif

       // DQ (5/31/2021): The support for a testing mode is not moved to a seperate traversal (artificialFrontier.C).
       // DQ (12/1/2013): Added switch to control testing mode for token unparsing.
       // Test codes in the tests/nonsmoke/functional/roseTests/astTokenStreamTests directory turn on this 
       // variable so that all regression tests can be processed to mix the unparsing of 
       // the token stream with unparsing from the AST.
       // if (ROSE_tokenUnparsingTestingMode == true)
          if (ROSE_tokenUnparsingTestingMode == true && false)
             {
#if DEBUG_INHERIT || 1
               printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
               printf ("ROSE_tokenUnparsingTestingMode = %s \n",ROSE_tokenUnparsingTestingMode ? "true" : "false");
               printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
#endif
#if DEBUG_INHERIT || 1
               printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): forceUnparseFromTokenStream = %s \n",forceUnparseFromTokenStream ? "true" : "false");
               printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): forceUnparseFromAST         = %s \n",forceUnparseFromAST ? "true" : "false");
#endif
               if (forceUnparseFromTokenStream == true || forceUnparseFromAST == true)
                  {
                 // We need to set this to a sufficently high number so that the conditional below will always be false.
                    static int max_numberOfNodes = 10000000; // (int)SgNode::numberOfNodes();

                    if (forceUnparseFromAST == true)
                       {
                         ROSE_ASSERT(forceUnparseFromTokenStream == false);
                         lowerbound = -1;
                         upperbound = max_numberOfNodes;
                       }
                      else
                       {
                         ROSE_ASSERT(forceUnparseFromTokenStream == true);

                         lowerbound = max_numberOfNodes;
                         upperbound = -1;
#if DEBUG_INHERIT || 1
                         printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): max_numberOfNodes = %d \n",max_numberOfNodes);
#endif
                       }
                  }
                 else
                  {
                 // DQ (12/1/2013): This is the testing mode for all regression tests.
                    ROSE_ASSERT(forceUnparseFromAST == false);
                    ROSE_ASSERT(forceUnparseFromTokenStream == false);
#if DEBUG_INHERIT || 1
                    printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): numberOfNodes = %d \n",numberOfNodes);
#endif
                    lowerbound = numberOfNodes / 4;
                    upperbound = 3 * (numberOfNodes / 4);
                  }

#if DEBUG_INHERIT || 1
               printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): random_counter = %d lowerbound = %d upperbound = %d \n",random_counter,lowerbound,upperbound);
#endif
            // bool skipMarkingAsToBeUnparsedFromAST  = false;
            // if ( (random_counter > lowerbound && random_counter < upperbound) || forceFromTokenStream == true)
               if ( random_counter >= lowerbound && random_counter <= upperbound )
                  {
#if DEBUG_INHERIT || 1
                    printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): Mark this statement as a transformation: random_counter = %d statement = %p = %s \n",
                         random_counter,statement,statement->class_name().c_str());
#endif
                 // options = "color=\"red\"";
                 // options = "penwidth=5, color=\"red\"";

                    if (returnAttribute.unparseFromTheAST == false)
                       {
                      // Mark this as the frontier only if this is the first time (while decending into the AST) 
                      // that we will unparse this subtree from the AST.
                         returnAttribute.isFrontier = true;
                       }

                    returnAttribute.unparseFromTheAST       = true;
                    returnAttribute.unparseUsingTokenStream = false;
                  }
                 else
                  {
                    returnAttribute.unparseUsingTokenStream = true;
                  }

#if DEBUG_INHERIT || 1
               printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): returnAttribute.unparseFromTheAST       = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
               printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): returnAttribute.unparseUsingTokenStream = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
#endif
             }
            else
             {
            // DQ (12/1/2013): The default is to unparse using the token stream, unless transformations are detected.
            // We may at some point introduce a mechanism to disable transformations where macros have 
            // been expanded so that we can define a SAFE system.  This has not been done yet.

            // returnAttribute.unparseUsingTokenStream = true;
               if (statement->get_file_info()->isTransformation() == true)
                  {
#if DEBUG_INHERIT
                    printf ("Found an AST transformation: statement = %p = %s \n",statement,statement->class_name().c_str());
#endif
                 // DQ (5/15/2021): Changed from returnAttribute.unparseFromTheAST == false to inheritedAttribute.unparseFromTheAST == false.
                 // returnAttribute.isFrontier = true;
                 // if (returnAttribute.unparseFromTheAST == false)
                    if (inheritedAttribute.unparseFromTheAST == false)
                       {
                      // Mark this as the frontier only if this is the first time (while decending into the AST) 
                      // that we will unparse this subtree from the AST.
                         returnAttribute.isFrontier = true;
                       }

                    returnAttribute.unparseFromTheAST       = true;
                    returnAttribute.unparseUsingTokenStream = false;
                  }
                 else
                  {
#if DEBUG_INHERIT
                    printf ("This statement is NOT a transformation: statement = %p = %s (returnAttribute.unparseUsingTokenStream = true) \n",statement,statement->class_name().c_str());
#endif
                    returnAttribute.unparseUsingTokenStream = true;

                 // DQ (5/15/2021): Added assertion. Not clear if this is OK.
                    ROSE_ASSERT(returnAttribute.unparseFromTheAST == false);
                  }
             }

          ROSE_ASSERT(returnAttribute.sourceFile != NULL);

          if (nodeIsFromCurrentFile == false)
             {
            // DQ (5/23/2021): This can happen when the current node is listed as compiler generated (where the source position was not available in the frontend).

#if DEBUG_INHERIT
            // printf ("childNodeIsFromSameFileAsCurrentNode == true: setting returnAttribute.isInCurrentFile = false \n");
               printf ("nodeIsFromCurrentFile == false: setting returnAttribute.isInCurrentFile = false \n");
#endif
               returnAttribute.isInCurrentFile = false;

            // DQ (5/23/2021): We can use the value that was comuted in the outer scope of this branch.
            // DQ (5/12/2021): Now we can change the source file so that we can detect nested header files.
            // int nested_physical_file_id = statement->get_file_info()->get_physical_file_id();
            // int physical_file_id = generate_physical_file_id(statement);
               int nested_physical_file_id = physical_file_id;

               if (nested_physical_file_id < 0)
                  {
                    printf ("Error: nested_physical_file_id < 0: statement = %p = %s name = %s \n",statement,statement->class_name().c_str(),SageInterface::get_name(statement).c_str());
                    SgLocatedNode* locatedNode = isSgLocatedNode(statement->get_parent());
                    ROSE_ASSERT(locatedNode != NULL);
                    locatedNode->get_file_info()->display("ERROR: nested_physical_file_id < 0");
                  }
               ROSE_ASSERT(nested_physical_file_id >= 0);

               string filename = Sg_File_Info::getFilenameFromID(nested_physical_file_id);

#if DEBUG_INHERIT
               printf ("looking for header file: filename = %s \n",filename.c_str());
#endif
               SgSourceFile* include_source_file = NULL;
               if (EDG_ROSE_Translation::edg_include_file_map.find(filename) != EDG_ROSE_Translation::edg_include_file_map.end())
                  {
                    SgIncludeFile* includeFile = EDG_ROSE_Translation::edg_include_file_map[filename];
                    ROSE_ASSERT(includeFile != NULL);

                    include_source_file = includeFile->get_source_file();
                 // ROSE_ASSERT(include_source_file != NULL);

                    if (include_source_file != NULL)
                       {
                         returnAttribute.sourceFile = include_source_file;
                       }
                      else
                       {
                      // If the source file is unavailable, then don't change the returnAttribute.sourceFile.
#if DEBUG_INHERIT
                         printf ("header file unavailable: filename = %s \n",filename.c_str());
#endif
#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                       }
                  }
                 else
                  {
#if DEBUG_INHERIT
                    printf ("filename not found in EDG_ROSE_Translation::edg_include_file_map (so include_source_file == NULL) \n");
#endif
                  }

               ROSE_ASSERT(returnAttribute.sourceFile != NULL);
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }

       // DQ (11/30/2013): Allow us to ignore class defintions in typedefs.
       // Mark the whole subtree as being unparsed from the AST so that synthizized attributes can be more esily computed.
       // if (inheritedAttribute.unparseFromTheAST == true)
       // if (inheritedAttribute.unparseFromTheAST == true && skipMarkingAsToBeUnparsedFromAST == false)
          if (inheritedAttribute.unparseFromTheAST == true)
             {
#if DEBUG_INHERIT
               printf ("   --- Where inheritedAttribute.unparseFromTheAST == true: set returnAttribute.unparseFromTheAST == true and returnAttribute.unparseUsingTokenStream = false \n");
#endif
               returnAttribute.unparseFromTheAST       = true;
               returnAttribute.unparseUsingTokenStream = false;
             }

#if 0
          if (returnAttribute.isFrontier == true)
             {
            // attribute->set_unparseUsingTokenStream(true);
               returnAttribute.containsNodesToBeUnparsedFromTheAST = true;
             }
            else
             {
            // attribute->set_containsNodesToBeUnparsedFromTheAST(true);
             }
#endif
#if DEBUG_INHERIT || 0
          printf ("   --- returnAttribute.isFrontier                                  = %s \n",returnAttribute.isFrontier ? "true" : "false");
          printf ("   --- returnAttribute.unparseFromTheAST                           = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.unparseUsingTokenStream                     = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
       // printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.isInCurrentFile                             = %s \n",returnAttribute.isInCurrentFile ? "true" : "false");
#endif
        }
       else
        {
       // Default setting for all non-SgStatements.
       // returnAttribute.unparseUsingTokenStream = true;

          if (inheritedAttribute.unparseFromTheAST == true)
             {
#if DEBUG_INHERIT && 0
               printf ("   --- Where inheritedAttribute.unparseFromTheAST == true: set returnAttribute.unparseFromTheAST == true and returnAttribute.unparseUsingTokenStream = false \n");
#endif
               returnAttribute.unparseFromTheAST       = true;
               returnAttribute.unparseUsingTokenStream = false;
             }
            else
             {
            // Default setting for all non-SgStatements.
               returnAttribute.unparseUsingTokenStream = true;
             }

#if DEBUG_INHERIT || 0
          printf ("Non-SgStatement node: \n");
          printf ("   --- returnAttribute.isFrontier                                  = %s \n",returnAttribute.isFrontier ? "true" : "false");
          printf ("   --- returnAttribute.unparseFromTheAST                           = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.unparseUsingTokenStream                     = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
       // printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
#endif
        }

#if DEBUG_INHERIT
     printf ("IIIIIIIIIIIIIIIIIIIIIIIIII \n");
     printf ("*** Leaving FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): n = %p = %s \n",n,n->class_name().c_str());
     printf ("IIIIIIIIIIIIIIIIIIIIIIIIII \n");
#endif

     ROSE_ASSERT( (returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false) || 
                  (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true) );

     if (returnAttribute.isFrontier == true)
        {
          ROSE_ASSERT(returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true);
        }

  // DQ (5/14/2021): Can we assert this? No, any statement marked as isTransformation() == true, will be marked as isFrontier == true.
  // ROSE_ASSERT(returnAttribute.isFrontier == false);

  // DQ (5/14/2021): Now removed.
  // ROSE_ASSERT(returnAttribute.containsNodesToBeUnparsedFromTheAST == false);

  // DQ (5/12/2021): Added assertion.
     ROSE_ASSERT(returnAttribute.sourceFile != NULL);

     return returnAttribute;
   }


FrontierDetectionForTokenStreamMapping_SynthesizedAttribute 
FrontierDetectionForTokenStreamMapping::evaluateSynthesizedAttribute (SgNode* n, 
     FrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute, 
     SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
  // The goal of this function is to identify the node ranges in the frontier that are associated with 
  // tokens stream unparsing, and AST node unparsing.  There ranges are saved and concatinated as we
  // proceed in the evaluation of the synthesized attributes up the AST.

  // We want to generate a IR node range in each node which contains children so that we can concatinate the lists
  // across the whole AST and define the frontier in terms of IR nodes which will then be converted into 
  // token ranges to be unparsed and specific IR nodes to be unparsed from the AST directly.

  // DQ (5/15/2021): Unless a transformation is seen via the inherited attribute, inheritedAttribute.isFrontier == false.
  // Transfomations are always marked as inheritedAttribute.isFrontier == true.

     ROSE_ASSERT(n != NULL);

#define DEBUG_SYNTH 0

#if DEBUG_SYNTH || 0
     printf ("\n\nSSSSSSSSSSSSSSSSSSSSSSSSSS \n");
     printf ("### In FrontierDetectionForTokenStreamMapping::evaluateSynthesizedAttribute(): TOP n = %p = %s \n",n,n->class_name().c_str());
     printf ("SSSSSSSSSSSSSSSSSSSSSSSSSS \n");
#endif

  // DQ (5/14/2021): I think this is always true by definition (since we save the SgNode in the inheritedAttribute).
     ROSE_ASSERT(n == inheritedAttribute.node);

  // FrontierDetectionForTokenStreamMapping_SynthesizedAttribute returnAttribute(n);
     FrontierDetectionForTokenStreamMapping_SynthesizedAttribute returnAttribute(n,inheritedAttribute.sourceFile);

  // We need to handle SgStatement, plus the SgSourceFile because we need to copy synthesized results from the SgGlobal to the SgSourceFile.
     SgStatement*  statement  = isSgStatement(n);
     SgSourceFile* sourceFile = isSgSourceFile(n);

  // DQ (5/23/2021): We want to avoid processing template instantiations or any code inside of template instantiations.
  // if (statement != NULL || sourceFile != NULL)
     if ((statement != NULL || sourceFile != NULL) && inheritedAttribute.isPartOfTemplateInstantiation == false)
        {
#if DEBUG_SYNTH
          printf ("In FrontierDetectionForTokenStreamMapping::evaluateSynthesizedAttribute(): SgStatement or SgSourceFile = %p = %s \n",n,n->class_name().c_str());
          if (statement != NULL)
             {
               printf (" --- statement = %s \n",SageInterface::get_name(statement).c_str());
             }
            else
             {
               printf (" --- sourceFile = %s \n",SageInterface::get_name(sourceFile).c_str());
             }
#endif

#if 0
       // DQ (5/23/2021): Test the call to generate_physical_file_id() (shoudl not be called for template instantiations.
       // ROSE_ASSERT(statement != NULL && generate_physical_file_id(statement) >= 0);
          if (statement != NULL && generate_physical_file_id(statement) >= 0)
             {
            // testing...
             }
#endif
       // Mark these directly.
          returnAttribute.isFrontier              = inheritedAttribute.isFrontier;

       // This is a reasonable default setting.
          if (inheritedAttribute.unparseFromTheAST == true)
             {
            // DQ (5/16/2021): If this was marked to be unparsed from the AST within the inherited attribute, then nothing 
            // computed on the children should change that.
#if DEBUG_SYNTH
               printf ("For inheritedAttribute.unparseFromTheAST == true: set returnAttribute.containsNodesToBeUnparsedFromTheAST = true \n");
#endif
               returnAttribute.containsNodesToBeUnparsedFromTheAST = true;
             }

#if 0
       // DQ (5/16/2021): If we set this to be true at the start then we can't tell if the current node should be unparsed from the AST or not.
       // This is a reasonable default setting.
          if (inheritedAttribute.unparseUsingTokenStream == true)
             {
#if DEBUG_SYNTH
               printf ("For inheritedAttribute.unparseUsingTokenStream == true: set returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = true \n");
#endif
               returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = true;
             }
#endif

#if DEBUG_SYNTH
          printf ("After using info from the inheritedAttribute: \n");
          printf ("   --- returnAttribute.isFrontier                                  = %s \n",returnAttribute.isFrontier ? "true" : "false");
          printf ("   --- returnAttribute.unparseFromTheAST                           = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.unparseUsingTokenStream                     = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
          printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif

       // DQ (5/15/2021): This is now set at the end of the function based on values of 
       // containsNodesToBeUnparsedFromTheAST and containsNodesToBeUnparsedFromTheTokenStream
       // ROSE_ASSERT(returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false);
       // ROSE_ASSERT(returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true);
       // ROSE_ASSERT( (returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false) || 
       //              (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true) );
       // ROSE_ASSERT(returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == false);
       // ROSE_ASSERT(returnAttribute.unparseUsingTokenStream == false);
          ROSE_ASSERT(returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == false);

       // DQ (5/15/2021): Try to assert this for at least test22.
       // ROSE_ASSERT(returnAttribute.unparseUsingTokenStream == true);
       // ROSE_ASSERT(returnAttribute.containsNodesToBeUnparsedFromTheTokenStream == true);

       // DQ (5/15/2021): The trivial case of unparsing the whole file from the AST will not be addressed 
       // if we insist that returnAttribute.containsNodesToBeUnparsedFromTheAST == true
       // Older comment: If this contains IR nodes to be unparsed from the AST and from the token stream 
       // then we have to report the list explicitly, else the action of unparsing from either the AST or 
       // the token stream can be reported using the single node (and not entered into the frontier list 
       // but progated usign the synthesized attribute.
       // if (returnAttribute.containsNodesToBeUnparsedFromTheAST == true)
       // if (returnAttribute.containsNodesToBeUnparsedFromTheAST == true && returnAttribute.containsNodesToBeUnparsedFromTheTokenStream == true)
       // if (returnAttribute.containsNodesToBeUnparsedFromTheTokenStream == true || true)
          if (returnAttribute.containsNodesToBeUnparsedFromTheAST == false)
             {
            // DQ (5/15/2021): This is now set at the end of the function.
            // returnAttribute.unparseFromTheAST       = false;

            // DQ (5/15/2021): This is now set at the end of the function.
            // DQ (5/15/2021): If containsNodesToBeUnparsedFromTheTokenStream then also mark returnAttribute.unparseUsingTokenStream = true
            // returnAttribute.unparseUsingTokenStream = false;
            // returnAttribute.unparseUsingTokenStream = true;

#if DEBUG_SYNTH
               printf ("Saving the list of relevant frontier nodes! \n");
#endif
            // DQ (12/1/2013): This implements a restriction no conditional statements so they will not be unparsed using a mixture of tokens and AST unparsing.
               SgIfStmt* ifStatement = isSgIfStmt(statement);

            // bool specialCaseNode = (ifStatement != NULL);
               bool specialCaseNode = false;
          
            // if (ifStatement != NULL)
            // if (specialCaseNode == true)
               if (ifStatement != NULL)
                  {
                 // There are special cases where we don't (at least presently) want to mix the two types on unparsing.
                 // This is how those special cases are handled.
#if DEBUG_SYNTH
                    printf ("*** processing SgIfStmt: \n");
                    printf ("Synthesized attribute evaluation is part of a conditional statment (containing a conditional expression/statement): statment = %p = %s \n",
                         statement,statement->class_name().c_str());
#endif
#if DEBUG_SYNTH
                    printf ("Iterate over the children (again): \n");
                    for (size_t i = 0; i < synthesizedAttributeList.size(); i++)
                       {
                         printf ("   --- synthesizedAttributeList[i=%" PRIuPTR "].node = %p = %s isFrontier = %s unparseUsingTokenStream = %s unparseFromTheAST = %s containsNodesToBeUnparsedFromTheAST = %s containsNodesToBeUnparsedFromTheTokenStream = %s \n",
                              i,synthesizedAttributeList[i].node,
                              synthesizedAttributeList[i].node != NULL                                ? synthesizedAttributeList[i].node->class_name().c_str() : "null",
                              synthesizedAttributeList[i].isFrontier                                  ? "true " : "false",
                              synthesizedAttributeList[i].unparseUsingTokenStream                     ? "true " : "false",
                              synthesizedAttributeList[i].unparseFromTheAST                           ? "true " : "false",
                              synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST         ? "true " : "false",
                              synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheTokenStream ? "true " : "false");

                      // DQ (5/16/2021): Moved to the traversal class.
                      // printf ("   ---   --- synthesizedAttributeList[i=%" PRIuPTR "].frontierNodes.size() = %" PRIuPTR " \n",i,synthesizedAttributeList[i].frontierNodes.size());
                       }
#endif
                    if (synthesizedAttributeList[0].containsNodesToBeUnparsedFromTheAST != synthesizedAttributeList[1].containsNodesToBeUnparsedFromTheAST)
                       {
#if DEBUG_SYNTH
                         printf ("This node has children that mix the two different types of unparsing! \n");
#endif
                         specialCaseNode = true;
                       }
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ABORT();
#endif
                  }

               SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(statement);
               if (typedefDeclaration != NULL)
                  {
#if DEBUG_SYNTH
                    printf ("*** processing SgTypedefDeclaration: \n");
                    for (size_t i = 0; i < synthesizedAttributeList.size(); i++)
                       {
                         printf ("   --- synthesizedAttributeList[i=%" PRIuPTR "].node = %p = %s isFrontier = %s unparseUsingTokenStream = %s unparseFromTheAST = %s containsNodesToBeUnparsedFromTheAST = %s containsNodesToBeUnparsedFromTheTokenStream = %s \n",
                              i,synthesizedAttributeList[i].node,
                              synthesizedAttributeList[i].node != NULL                                ? synthesizedAttributeList[i].node->class_name().c_str() : "null",
                              synthesizedAttributeList[i].isFrontier                                  ? "true " : "false",
                              synthesizedAttributeList[i].unparseUsingTokenStream                     ? "true " : "false",
                              synthesizedAttributeList[i].unparseFromTheAST                           ? "true " : "false",
                              synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST         ? "true " : "false",
                              synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheTokenStream ? "true " : "false");

                      // DQ (5/16/2021): Moved to the traversal class.
                      // printf ("   ---   --- synthesizedAttributeList[i=%" PRIuPTR "].frontierNodes.size() = %" PRIuPTR " \n",i,synthesizedAttributeList[i].frontierNodes.size());
                       }
#endif
                    if (synthesizedAttributeList[0].containsNodesToBeUnparsedFromTheAST == true && synthesizedAttributeList[0].containsNodesToBeUnparsedFromTheTokenStream == true)
                       {
#if DEBUG_SYNTH
                         printf ("This SgTypedefDeclaration node has children that mix the two different types of unparsing! \n");
#endif
                         specialCaseNode = true;
                       }
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ABORT();
#endif
                  }

            // DQ (12/1/2013): This handles the details of the SgForStatement (which has 3 relevant children (excluding the body).
               SgForStatement* forStatement = isSgForStatement(statement);
               if (forStatement != NULL)
                  {
                 // There are special cases where we don't (at least presently) want to mix the two types on unparsing.
                 // This is how those special cases are handled.
#if DEBUG_SYNTH
                    printf ("*** processing SgForStatement: \n");
                    printf ("Synthesized attribute evaluation is part of a SgForStatement (containing a conditional expression/statement): statment = %p = %s \n",statement,statement->class_name().c_str());
#endif
#if DEBUG_SYNTH
                    for (size_t i = 0; i < synthesizedAttributeList.size(); i++)
                       {
                         printf ("   --- synthesizedAttributeList[i=%" PRIuPTR "].node = %p = %s isFrontier = %s unparseUsingTokenStream = %s unparseFromTheAST = %s containsNodesToBeUnparsedFromTheAST = %s containsNodesToBeUnparsedFromTheTokenStream = %s \n",
                              i,synthesizedAttributeList[i].node,
                              synthesizedAttributeList[i].node != NULL                                ? synthesizedAttributeList[i].node->class_name().c_str() : "null",
                              synthesizedAttributeList[i].isFrontier                                  ? "true " : "false",
                              synthesizedAttributeList[i].unparseUsingTokenStream                     ? "true " : "false",
                              synthesizedAttributeList[i].unparseFromTheAST                           ? "true " : "false",
                              synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST         ? "true " : "false",
                              synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheTokenStream ? "true " : "false");

                      // DQ (5/16/2021): Moved to the traversal class.
                      // printf ("   ---   --- synthesizedAttributeList[i=%" PRIuPTR "].frontierNodes.size() = %" PRIuPTR " \n",i,synthesizedAttributeList[i].frontierNodes.size());
                       }
#endif
                 // I don't know why unparseUsingTokenStream are different, but the containsNodesToBeUnparsedFromTheAST are the same.
                 // if (synthesizedAttributeList[0].containsNodesToBeUnparsedFromTheAST != synthesizedAttributeList[1].containsNodesToBeUnparsedFromTheAST)
                    if ( (synthesizedAttributeList[0].unparseUsingTokenStream != synthesizedAttributeList[1].unparseUsingTokenStream) ||
                         (synthesizedAttributeList[0].unparseUsingTokenStream != synthesizedAttributeList[2].unparseUsingTokenStream) )
                       {
#if DEBUG_SYNTH
                         printf ("This node (SgForStatement) has children that mix the two different types of unparsing! \n");
#endif
                         specialCaseNode = true;
                       }
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ABORT();
#endif
                  }

#if DEBUG_SYNTH
            // DQ (5/16/2021): Moved to the traversal class.
            // printf ("Fixup returnAttribute.frontierNodes list: frontierNodes.size() = %zu \n",returnAttribute.frontierNodes.size());
               printf ("specialCaseNode = %s \n",specialCaseNode ? "true" : "false");
#endif
               if (specialCaseNode == true)
                  {
                 // Mark the current node to be a frontier, instead of the child nodes.  Mark as to be unparse from the AST (no choice since subtrees must be unparsed from the AST).
#if DEBUG_SYNTH
                    printf ("*** processing special case: \n");
                    printf ("Handling synthesized attribute as a special case: statement = %p = %s \n",statement,statement->class_name().c_str());
#endif
                    returnAttribute.isFrontier   = true;

                    bool unparseUsingTokenStream = false;
                    bool unparseFromTheAST       = true;

                    FrontierNode* frontierNode = new FrontierNode(statement,unparseUsingTokenStream,unparseFromTheAST);
                    ROSE_ASSERT(frontierNode != NULL);

                 // DQ (5/16/2021): Moved to the traversal class.
                 // returnAttribute.frontierNodes.push_back(frontierNode);
                 // frontierNodes.push_back(frontierNode);
                    addFrontierNode (statement, frontierNode);
                  }
                 else
                  {
                 // This is the non-special case.
                    ROSE_ASSERT(specialCaseNode == false);
#if DEBUG_SYNTH
                    printf ("*** processing non-special case: \n");
                    printf ("(part 2): Iterate over the children (size = %zu): \n",synthesizedAttributeList.size());
#endif
                    for (size_t i = 0; i < synthesizedAttributeList.size(); i++)
                       {
                         SgStatement* child_synthesized_attribute_statement = synthesizedAttributeList[i].node;
#if DEBUG_SYNTH || 0
                         printf ("\nCHILD -- CHILD -- CHILD -- CHILD -- CHILD -- CHILD -- CHILD -- CHILD -- CHILD -- CHILD \n");
                         printf ("TOP OF ITERATION OVER CHILDREN \n");
                         printf ("   --- child_synthesized_attribute_statement = %p = %s name = %s \n",child_synthesized_attribute_statement,
                              child_synthesized_attribute_statement != NULL ? child_synthesized_attribute_statement->class_name().c_str() : "null",
                              child_synthesized_attribute_statement != NULL ? SageInterface::get_name(child_synthesized_attribute_statement).c_str() : "null");
                         printf ("   ---   --- synthesizedAttributeList[i=%" PRIuPTR "].isFrontier                                  = %s \n",i,synthesizedAttributeList[i].isFrontier ? "true" : "false");
                         printf ("   ---   --- synthesizedAttributeList[i=%" PRIuPTR "].unparseUsingTokenStream                     = %s \n",
                              i,synthesizedAttributeList[i].unparseUsingTokenStream ? "true" : "false");
                         printf ("   ---   --- synthesizedAttributeList[i=%" PRIuPTR "].unparseFromTheAST                           = %s \n",
                              i,synthesizedAttributeList[i].unparseFromTheAST ? "true" : "false");
                         printf ("   ---   --- synthesizedAttributeList[i=%" PRIuPTR "].containsNodesToBeUnparsedFromTheAST         = %s \n",
                              i,synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST ? "true" : "false");
                         printf ("   ---   --- synthesizedAttributeList[i=%" PRIuPTR "].containsNodesToBeUnparsedFromTheTokenStream = %s \n",
                              i,synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif
                      // DQ (5/23/2021): Moved to after (child_synthesized_attribute_statement != NULL && inheritedAttribute.isPartOfTemplateInstantiation == false) predicate.
                      // bool childNodeIsFromSameFileAsCurrentNode = isChildNodeFromSameFileAsCurrentNode (statement, child_synthesized_attribute_statement);

#if DEBUG_SYNTH
                      // DQ (5/16/2021): Moved to the traversal class.
                      // printf ("* building the returnAttribute.frontierNodes list \n");
                         printf ("* building the frontierNodes list \n");
                      // printf ("childNodeIsFromSameFileAsCurrentNode = %s \n",childNodeIsFromSameFileAsCurrentNode ? "true" : "false");
#endif
#if DEBUG_SYNTH || 0
                         printf ("child_synthesized_attribute_statement            = %p \n",child_synthesized_attribute_statement);
                         printf ("inheritedAttribute.isPartOfTemplateInstantiation = %s \n",inheritedAttribute.isPartOfTemplateInstantiation ? "true" : "false");
#endif
                      // if (child_synthesized_attribute_statement != NULL)
                      // if (child_synthesized_attribute_statement != NULL && childNodeIsFromSameFileAsCurrentNode == true)
                      // if (child_synthesized_attribute_statement != NULL)
                         if (child_synthesized_attribute_statement != NULL && inheritedAttribute.isPartOfTemplateInstantiation == false)
                            {
#if DEBUG_SYNTH
                              printf ("Before building FrontierNode (child_synthesized_attribute_statement != NULL): \n");
                              printf (" --- returnAttribute.unparseFromTheAST                           = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
                              printf (" --- returnAttribute.unparseUsingTokenStream                     = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
                              printf (" --- returnAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
                              printf (" --- returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif

#if 0
                           // Original code.
                              FrontierNode* frontierNode = new FrontierNode(child_synthesized_attribute_statement,synthesizedAttributeList[i].unparseUsingTokenStream,synthesizedAttributeList[i].unparseFromTheAST);
                              ROSE_ASSERT(frontierNode != NULL);
#else
                           // DQ (5/23/2021): Testing the statement and child_synthesized_attribute_statement directly.
                           // ROSE_ASSERT(statement == NULL || (statement != NULL && generate_physical_file_id(statement) >= 0));
                           // ROSE_ASSERT(child_synthesized_attribute_statement != NULL && generate_physical_file_id(child_synthesized_attribute_statement) >= 0);
#if 1
                           // DQ (5/23/2021): Relocated to after (child_synthesized_attribute_statement != NULL && inheritedAttribute.isPartOfTemplateInstantiation == false) predicate.
                              bool childNodeIsFromSameFileAsCurrentNode = isChildNodeFromSameFileAsCurrentNode (statement, child_synthesized_attribute_statement);
#else
                              bool childNodeIsFromSameFileAsCurrentNode = true;
                              if (statement != NULL)
                                 {
                                   childNodeIsFromSameFileAsCurrentNode = isChildNodeFromSameFileAsCurrentNode (statement, child_synthesized_attribute_statement);
                                 }
#endif
                           // DQ (5/18/2021): New version of code to address how a node from a header file can  
                           // not effect the decision to unparse from the token stream or AST in the parent file.
                              FrontierNode* frontierNode = NULL;
                              if (childNodeIsFromSameFileAsCurrentNode == false)
                                 {
#if 0
                                   printf ("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC \n");
                                   printf ("childNodeIsFromSameFileAsCurrentNode == false \n");
                                   printf ("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC \n");
#endif
#if 0
                                // DQ (6/6/2021): This is where the error is in test_161.cpp in the code segregation.
                                // A child node that is not from the same file as the current node (parent) does not 
                                // contribute to the decision process for if the parent should be unparsed from the 
                                // AST or the token stream.

                                // returnAttribute.containsNodesToBeUnparsedFromTheAST         = false;
                                // returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = true;
                                   returnAttribute.containsNodesToBeUnparsedFromTheAST         = returnAttribute.containsNodesToBeUnparsedFromTheAST || false;
                                   returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = returnAttribute.containsNodesToBeUnparsedFromTheTokenStream && true;
#endif
                                // Can we set these? Yes, the child synthesizedAttributeList array can be modified.  This is useful in
                                // embeddeding the header file information into the analysis for where to unparse from the token stream.
                                   synthesizedAttributeList[i].unparseFromTheAST       = false;
                                   synthesizedAttributeList[i].unparseUsingTokenStream = true;

                                // bool local_unparseFromTheAST       = false;
                                // bool local_unparseUsingTokenStream = true;
                                // bool local_unparseFromTheAST       = returnAttribute.containsNodesToBeUnparsedFromTheAST;
                                // bool local_unparseUsingTokenStream = returnAttribute.containsNodesToBeUnparsedFromTheTokenStream;
                                   bool local_unparseFromTheAST       = false;
                                   bool local_unparseUsingTokenStream = true;

                                   frontierNode = new FrontierNode(child_synthesized_attribute_statement,local_unparseUsingTokenStream,local_unparseFromTheAST);
                                   ROSE_ASSERT(frontierNode != NULL);
#if 0
                                   printf ("frontierNode->display() = %s \n",frontierNode->display().c_str());
#endif
#if 0
                                   printf ("Exiting as a test! \n");
                                   ROSE_ASSERT(false);
#endif
                                 }
                                else
                                 {
                                // This is the original code.
                                   frontierNode = new FrontierNode(child_synthesized_attribute_statement,synthesizedAttributeList[i].unparseUsingTokenStream,synthesizedAttributeList[i].unparseFromTheAST);
                                   ROSE_ASSERT(frontierNode != NULL);
                                 }

                              ROSE_ASSERT(frontierNode != NULL);
#endif

#if DEBUG_SYNTH
                              printf ("synthesizedAttributeList[i].unparseUsingTokenStream = %s \n",synthesizedAttributeList[i].unparseUsingTokenStream ? "true" : "false");
#endif
                              if (synthesizedAttributeList[i].unparseUsingTokenStream == true)
                                 {
#if DEBUG_SYNTH
                                   printf ("NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN \n");
                                   printf ("synthesizedAttributeList[i].unparseUsingTokenStream == true \n");
                                   printf ("NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN \n");
#endif
                                // DQ (5/16/2021): Moved to the traversal class.
                                // returnAttribute.frontierNodes.push_back(frontierNode);
                                // frontierNodes.push_back(frontierNode);
                                   addFrontierNode (synthesizedAttributeList[i].node, frontierNode);

                                // DQ (5/15/2021): Added uniform setting of unparseUsingTokenStream.
                                // returnAttribute.unparseUsingTokenStream                     = true;
                                   returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = true;
#if 0
                                   printf ("Exiting as a test! \n");
                                   ROSE_ASSERT(false);
#endif
                                 }
                                else
                                 {
#if DEBUG_SYNTH
                                   printf ("synthesizedAttributeList[i].unparseFromTheAST = %s \n",synthesizedAttributeList[i].unparseFromTheAST? "true" : "false");
#endif
                                   if (synthesizedAttributeList[i].unparseFromTheAST == true)
                                      {
#if DEBUG_SYNTH
                                        printf ("NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN \n");
                                        printf ("synthesizedAttributeList[i].unparseFromTheAST == true \n");
                                        printf ("NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN \n");
#endif
                                     // DQ (5/16/2021): Moved to the traversal class.
                                     // returnAttribute.frontierNodes.push_back(frontierNode);
                                     // frontierNodes.push_back(frontierNode);
                                        addFrontierNode (synthesizedAttributeList[i].node, frontierNode);

                                     // DQ (5/15/2021): This is computed at the end of the function.
                                     // DQ (5/15/2021): If we have just one child that is requiring unparsing from
                                     // the AST, then we can't unpare the current node from the token stream.
                                     // returnAttribute.unparseUsingTokenStream             = false;

                                     // DQ (5/15/2021): Added uniform setting of unparseFromTheAST.
                                     // returnAttribute.unparseFromTheAST                   = true;
                                        returnAttribute.containsNodesToBeUnparsedFromTheAST = true;
                                      }
                                     else
                                      {
#if DEBUG_SYNTH
                                        printf ("NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN \n");
                                        printf ("Copy all of the child frontier nodes \n");
                                        printf ("NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN \n");
#endif
#if DEBUG_SYNTH
                                     // DQ (5/16/2021): Moved to the traversal class.
                                     // printf ("Current node = %p = %s not added to frontier node list (add any lists from synthesizedAttributeList[i=%" PRIuPTR "].frontierNodes.size() = %" PRIuPTR ") \n",
                                     //      n,n->class_name().c_str(),i,synthesizedAttributeList[i].frontierNodes.size());
                                        printf ("Current node = %p = %s not added to frontier node list (add any lists from frontierNodes.size() = %" PRIuPTR ") \n",
                                             n,n->class_name().c_str(),frontierNodes.size());
#endif
                                     // We don't record nodes that are just containing subtrees to be unparsed from the AST.
                                     // if (synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST == false)
                                           {
#if DEBUG_SYNTH
                                          // DQ (5/16/2021): Moved to the traversal class.
                                          // printf ("Copy the synthesized_attribute child node's frontierNodes into returnAttribute frontierNodes: size = %zu \n",
                                          //      synthesizedAttributeList[i].frontierNodes.size());
                                             printf ("Copy the synthesized_attribute child node's frontierNodes into returnAttribute frontierNodes: size = %zu \n",frontierNodes.size());
#endif
                                          // Lack of AST usage in the subtree implies that there are nodes unparsed from the token stream.
                                             if (synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST == false)
                                                {
#if DEBUG_SYNTH
                                                  printf (" --- Set returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = true \n");
#endif
                                               // returnAttribute.unparseUsingTokenStream = true;
                                                  returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = true;
                                                }
#if 0
                                             printf ("Exiting as a test! \n");
                                             ROSE_ASSERT(false);
#endif
                                           }

                                     // DQ (5/16/2021): Moved to the traversal class.
                                     // DQ (5/15/2021): Even if we don't have a list of IR nodes to add then at least add one for the current node.
                                     // returnAttribute.frontierNodes.push_back(frontierNode);
                                      }
                                 }

#if DEBUG_SYNTH || 0
                           // DQ (5/15/2021): Output the current frontierNode
                              printf ("frontierNode = %s \n",frontierNode->display().c_str());
#endif
                            }
                           else
                            {
#if DEBUG_SYNTH
                              printf ("WARNING: synthesized_attribute_statement == NULL \n");
#endif
                            }

#if DEBUG_SYNTH
                         printf (" --- At base of loop over synthesized attribute list elements i = %" PRIuPTR " \n",i);
                         printf (" --- --- returnAttribute.node                                        = %p = %s \n",
                              returnAttribute.node,returnAttribute.node != NULL ? returnAttribute.node->class_name().c_str() : "null");
                         printf (" --- --- returnAttribute.isFrontier                                  = %s \n",returnAttribute.isFrontier ? "true" : "false");
                         printf (" --- --- returnAttribute.unparseFromTheAST                           = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
                         printf (" --- --- returnAttribute.unparseUsingTokenStream                     = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
                         printf (" --- --- returnAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
                         printf (" --- --- returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
                         printf ("BOTTOM OF ITERATION OVER CHILDREN \n\n");
#endif

                       }

#if DEBUG_SYNTH
                    printf ("DONE: (part 2) Iterate over the children: \n");
#endif
                  }

            // DQ (5/15/2021): For debugging test22, we should have frontierNode->unparseUsingTokenStream == true
            // ROSE_ASSERT(frontierNode->unparseFromTheAST == true);
            // ROSE_ASSERT(returnAttribute.unparseUsingTokenStream == true);
             }
            else
             {
            // DQ (5/15/2021): This characterizes this false branch.
            // ROSE_ASSERT (returnAttribute.containsNodesToBeUnparsedFromTheAST == false || returnAttribute.containsNodesToBeUnparsedFromTheTokenStream == false);
            // ROSE_ASSERT (returnAttribute.containsNodesToBeUnparsedFromTheTokenStream == false);
               ROSE_ASSERT (returnAttribute.containsNodesToBeUnparsedFromTheAST == true);
             }

#if DEBUG_SYNTH
       // DQ (5/16/2021): Moved to the traversal class.
       // printf ("* after processing of returnAttribute.frontierNodes(): size = %zu \n",returnAttribute.frontierNodes.size());
          printf ("* after processing of frontierNodes(): size                      = %zu \n",frontierNodes.size());
          printf (" --- returnAttribute.isFrontier                                  = %s \n",returnAttribute.isFrontier ? "true" : "false");
          printf (" --- returnAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
          printf (" --- returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif

       // DQ (5/15/2021): This can be set from the inherited attribute.
       // ROSE_ASSERT(returnAttribute.isFrontier == false);

       // DQ (5/15/2021): Each should be false before this point.
          ROSE_ASSERT(returnAttribute.unparseFromTheAST == false && returnAttribute.unparseUsingTokenStream == false);

       // DQ (5/15/2021): Here is where we mark the values returnAttribute.unparseFromTheAST and returnAttribute.unparseUsingTokenStream.
       // To support both unparsing from the token stream and from the AST, we need to mark returnAttribute.unparseFromTheAST = true.
          if (returnAttribute.containsNodesToBeUnparsedFromTheAST == true  && returnAttribute.containsNodesToBeUnparsedFromTheTokenStream == true)
             {
               returnAttribute.isFrontier        = true;
               returnAttribute.unparseFromTheAST = true;
             }
            else
             {
               if (returnAttribute.containsNodesToBeUnparsedFromTheAST == false  && returnAttribute.containsNodesToBeUnparsedFromTheTokenStream == true)
                  {
                    returnAttribute.unparseUsingTokenStream = true;
                  }
                 else
                  {
                 // if (returnAttribute.containsNodesToBeUnparsedFromTheTokenStream == false)
                    if (returnAttribute.containsNodesToBeUnparsedFromTheAST == true  && returnAttribute.containsNodesToBeUnparsedFromTheTokenStream == false)
                       {
                      // returnAttribute.unparseUsingTokenStream = true;
                         returnAttribute.unparseFromTheAST = true;
                       }
                      else
                       {
                         if (returnAttribute.containsNodesToBeUnparsedFromTheAST == false  && returnAttribute.containsNodesToBeUnparsedFromTheTokenStream == false)
                            {
                           // DQ (5/16/2021): Just because returnAttribute.containsNodesToBeUnparsedFromTheTokenStream == false does
                           // not imply that this node should be unpared from the AST (e.g. the case of an empty SgBasicBlock).

                           // This is the default when 
                           // returnAttribute.unparseFromTheAST = true;
                              returnAttribute.unparseUsingTokenStream = true;
                            }
                           else
                            {
                              printf ("I don't think that this case can happen! \n");
#if 1
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif
                            }
                       }
                  }
             }

#if 0
       // DQ (5/15/2021): Also need to set the frontier node's isFrontier flag.
          if ( (returnAttribute.containsNodesToBeUnparsedFromTheAST != returnAttribute.containsNodesToBeUnparsedFromTheTokenStream) && 
               (returnAttribute.containsNodesToBeUnparsedFromTheAST == true || returnAttribute.containsNodesToBeUnparsedFromTheTokenStream == true) )
             {
               returnAttribute.isFrontier = true;
             }
#endif


       // DQ (5/17/2021): Moved this code to the point after where the returnAttribute.unparseFromTheAST or returnAttribute.unparseUsingTokenStream is set.

       // SgStatement* statement = synthesizedAttributeList[i].node;
       // ROSE_ASSERT(statement != NULL);
          if (statement != NULL && returnAttribute.containsNodesToBeUnparsedFromTheAST == true)
             {
#if DEBUG_SYNTH
               printf ("Before building FrontierNode (statement != NULL && returnAttribute.containsNodesToBeUnparsedFromTheAST == true): \n");
               printf (" --- returnAttribute.unparseFromTheAST                           = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
               printf (" --- returnAttribute.unparseUsingTokenStream                     = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
               printf (" --- returnAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
               printf (" --- returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif
               FrontierNode* frontierNode = new FrontierNode(statement,returnAttribute.unparseUsingTokenStream,returnAttribute.unparseFromTheAST);

            // DQ (5/16/2021): Moved to the traversal class.
            // returnAttribute.frontierNodes.push_back(frontierNode);
            // frontierNodes.push_back(frontierNode);
               addFrontierNode (statement, frontierNode);
             }
            else
             {
            // This case happens for test_CplusplusMacro_C.c, which has nothing but CPP directives.
             }

       // DQ (5/15/2021): NOTE: We might want to just add all of the frontier nodes to a set 
       // rather than trying to pass them up through the synthesized attributes.

       // DQ (5/15/2021): Try to assert this for at least test22.
       // ROSE_ASSERT(returnAttribute.unparseUsingTokenStream == true);
       // ROSE_ASSERT(returnAttribute.containsNodesToBeUnparsedFromTheTokenStream == true);

#if DEBUG_SYNTH && 0
          printf ("* Frontier nodes (n = %p = %s): ",n,n->class_name().c_str());

          printf ("Calling outputFrontierNodes() \n");
          outputFrontierNodes();

          printf ("* Frontier End \n");
#endif
#if DEBUG_SYNTH || 0
          printf ("   --- returnAttribute.isFrontier                                  = %s \n",returnAttribute.isFrontier ? "true" : "false");
          printf ("   --- returnAttribute.unparseFromTheAST                           = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.unparseUsingTokenStream                     = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
          printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif
          ROSE_ASSERT( (returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false) || 
                       (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true)  || 
                       (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == false) );

#if 0
       // DQ (5/20/2021): This fails the test of any empty files.
          if (isSgGlobal(n) != NULL)
             {
            // DQ (5/16/2021): Moved to the traversal class.
            // DQ (5/15/2021): There should be at least one FrontierNode even if it is only the source file (or maybe the global scope).
            // ROSE_ASSERT(returnAttribute.frontierNodes.size() > 0);
               ROSE_ASSERT(frontierNodes.size() > 0);
             }

          if (sourceFile != NULL)
             {
            // DQ (5/16/2021): Moved to the traversal class.
            // DQ (5/15/2021): There should be at least one FrontierNode even if it is only the source file (or maybe the global scope).
            // ROSE_ASSERT(returnAttribute.frontierNodes.size() > 0);
               ROSE_ASSERT(frontierNodes.size() > 0);
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ABORT();
#endif
             }
#endif

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ABORT();
#endif
        }
       else
        {
       // DQ (5/15/2021): This characterizes this false branch.
       // if (statement != NULL || sourceFile != NULL)
       // ROSE_ASSERT (statement == NULL && sourceFile == NULL);
       // if ((statement != NULL || sourceFile != NULL) && inheritedAttribute.isPartOfTemplateInstantiation == false)
          ROSE_ASSERT ((statement == NULL && sourceFile == NULL) || inheritedAttribute.isPartOfTemplateInstantiation == true);

       // Default settings.
       // returnAttribute.unparseUsingTokenStream = true;

       // DQ (5/10/2021): Added initialization of the node data member (we can't do this because n is not a SgStatement).
       // returnAttribute.node                                        = n;

          returnAttribute.isFrontier                                  = false;
          returnAttribute.unparseUsingTokenStream                     = false;
          returnAttribute.unparseFromTheAST                           = false;
          returnAttribute.containsNodesToBeUnparsedFromTheAST         = false;
          returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = false;

          returnAttribute.isFrontier                                  = inheritedAttribute.isFrontier;
          returnAttribute.unparseUsingTokenStream                     = inheritedAttribute.unparseUsingTokenStream;
          returnAttribute.unparseFromTheAST                           = inheritedAttribute.unparseFromTheAST;

       // DQ (5/14/2021): This data member is now removed from the inherited attribute, becasue it was never set and was always false.
       // So setting it to false here makes the most sense.
       // returnAttribute.containsNodesToBeUnparsedFromTheAST         = inheritedAttribute.containsNodesToBeUnparsedFromTheAST;
          returnAttribute.containsNodesToBeUnparsedFromTheAST         = false;

#if DEBUG_SYNTH
          printf ("Case of non-SgStatement and non-SgSourceFile: \n");
          printf ("   --- returnAttribute.isFrontier                                  = %s \n",returnAttribute.isFrontier ? "true" : "false");
          printf ("   --- returnAttribute.unparseFromTheAST                           = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.unparseUsingTokenStream                     = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
          printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif
        }

#if DEBUG_SYNTH
     printf ("SSSSSSSSSSSSSSSSSSSSSSSSSS \n");
     printf ("### Leaving FrontierDetectionForTokenStreamMapping::evaluateSynthesizedAttribute(): BOTTOM n = %p = %s \n",n,n->class_name().c_str());
     printf ("SSSSSSSSSSSSSSSSSSSSSSSSSS \n");
#endif

  // This can't be applied to non-statements.
  // ROSE_ASSERT( (returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false) || 
  //              (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true) );
     ROSE_ASSERT( (returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false) || 
                  (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true)  || 
                  (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == false) );

#if 0
  // DQ (5/20/2021): This fails the test of any empty files.
     if (isSgGlobal(n) != NULL)
        {
       // DQ (5/16/2021): Moved to the traversal class.
       // DQ (5/15/2021): There should be at least one FrontierNode even if it is only the source file (or maybe the global scope).
       // ROSE_ASSERT(returnAttribute.frontierNodes.size() > 0);
          ROSE_ASSERT(frontierNodes.size() > 0);
        }

     if (sourceFile != NULL)
        {
       // DQ (5/16/2021): Moved to the traversal class.
       // DQ (5/15/2021): There should be at least one FrontierNode even if it is only the source file (or maybe the global scope).
       // ROSE_ASSERT(returnAttribute.frontierNodes.size() > 0);
          ROSE_ASSERT(frontierNodes.size() > 0);
        }
#endif

     return returnAttribute;
   }


int
FrontierDetectionForTokenStreamMapping::numberOfNodesInSubtree(SgSourceFile* sourceFile)
   {
     int value = 0;

     class CountTraversal : public SgSimpleProcessing
        {
          public:
              int count;
              CountTraversal() : count(0) {}

           // We only want to count statements since we only test the token/AST unparsing at the statement level.
           // void visit ( SgNode* n ) { count++; }
              void visit ( SgNode* n ) { if (isSgStatement(n) != NULL) count++; }
        };

     CountTraversal counter;
  // SgNode* thisNode = const_cast<SgNode*>(this);
     counter.traverseWithinFile(sourceFile,preorder);
     value = counter.count;

     return value;
   }


#if 0
void
frontierDetectionForTokenStreamMapping ( SgSourceFile* sourceFile )
#else
// DQ (5/10/2021): Activate this code.
void
frontierDetectionForTokenStreamMapping ( SgSourceFile* sourceFile, bool traverseHeaderFiles)
#endif
   {
  // This frontier detection happens before we associate token subsequences to the AST (in a seperate map).

     ROSE_ASSERT(sourceFile != NULL);

  // FrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute;
     FrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute(sourceFile);

     FrontierDetectionForTokenStreamMapping fdTraversal(sourceFile);

#if 0
     printf ("In frontierDetectionForTokenStreamMapping(): calling traverse() sourceFile = %p \n",sourceFile);
#endif

#if 0
     std::set<SgStatement*> transformedStatementSet = SageInterface::collectTransformedStatements(sourceFile);
     printf ("In frontierDetectionForTokenStreamMapping(): transformedStatementSet.size() = %zu \n",transformedStatementSet.size());
#endif
#if 0
  // Debugging (this set in the constructor).
     printf ("numberOfNodes = %d \n",fdTraversal.numberOfNodes);
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ABORT();
#endif

#if 0
  // fdTraversal.traverse(sourceFile,inheritedAttribute);
  // FrontierDetectionForTokenStreamMapping_SynthesizedAttribute topAttribute = fdTraversal.traverse(sourceFile,inheritedAttribute);
  // FrontierDetectionForTokenStreamMapping_SynthesizedAttribute topAttribute = fdTraversal.traverseInputFiles(sourceFile,inheritedAttribute);
     FrontierDetectionForTokenStreamMapping_SynthesizedAttribute topAttribute = fdTraversal.traverseWithinFile(sourceFile,inheritedAttribute);
#else
  // DQ (5/10/2021): Activate this code.
     FrontierDetectionForTokenStreamMapping_SynthesizedAttribute topAttribute;
     if (traverseHeaderFiles == false)
       {
         topAttribute = fdTraversal.traverseWithinFile(sourceFile,inheritedAttribute);
       }
      else
       {
         topAttribute = fdTraversal.traverse(sourceFile,inheritedAttribute);
       }
#endif

#if 0
  // DQ (5/9/2021): I think this is the next problem to resolve in the morning.
     printf ("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF \n");
     printf ("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF \n");
     printf ("An issue here is that we are not using a traversal over the whole AST \n");
     printf ("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF \n");
     printf ("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF \n");
#endif

  // DQ (5/15/2012): The SgSourceFile is not a SgStatement, so we can't assert this.
  // ROSE_ASSERT(topAttribute.node != NULL);

#if 0
     printf ("   --- topAttribute.node                          = %p = %s \n",topAttribute.node,(topAttribute.node != NULL) ? topAttribute.node->class_name().c_str() : "null");
     printf ("   --- topAttribute.isFrontier                                  = %s \n",topAttribute.isFrontier ? "true" : "false");
     printf ("   --- topAttribute.unparseUsingTokenStream                     = %s \n",topAttribute.unparseUsingTokenStream ? "true" : "false");
     printf ("   --- topAttribute.unparseFromTheAST                           = %s \n",topAttribute.unparseFromTheAST ? "true" : "false");
     printf ("   --- topAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",topAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
     printf ("   --- topAttribute.containsNodesToBeUnparsedFromTheTokenStream = %s \n",topAttribute.containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif

  // DQ (5/16/2021): Moved to the traversal class.
  // DQ (5/15/2021): There should be at least one FrontierNode even if it is only the source file (or maybe the global scope).
  // ROSE_ASSERT(topAttribute.frontierNodes.size() > 0);
     ROSE_ASSERT(fdTraversal.frontierNodes.size() > 0);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ABORT();
#endif

  // map<SgStatement*,FrontierNode*> token_unparse_frontier_map;

#if 0
  // DQ (5/16/2021): Moved to the traversal class.
  // printf ("Final Frontier (topAttribute.frontierNodes.size() = %" PRIuPTR ") (n = %p = %s): ",topAttribute.frontierNodes.size(),sourceFile,sourceFile->class_name().c_str());
     printf ("Final Frontier (topAttribute.frontierNodes.size() = %" PRIuPTR ") (n = %p = %s): ",fdTraversal.frontierNodes.size(),sourceFile,sourceFile->class_name().c_str());
#endif

  // DQ (11/29/2013): Used to mark statements that are redundantly mapped to a single token sequence.
  // int lastTokenIndex = 0;

  // DQ (5/17/2021): Using newer form of frontierNodes container.
     std::map<int,std::map<SgStatement*,FrontierNode*>* >::iterator i = fdTraversal.frontierNodes.begin();
     while (i != fdTraversal.frontierNodes.end())
        {
          map<SgStatement*,FrontierNode*> token_unparse_frontier_map;

          int physical_file_id = i->first;
          std::map<SgStatement*,FrontierNode*>* frontierMap = i->second;

       // std::map<std::string, SgIncludeFile*> edg_include_file_map;

       // Find the associated file
          SgSourceFile* associatedSourceFile = NULL;

          string filename = Sg_File_Info::getFilenameFromID(physical_file_id);
#if 0
          printf ("filename = %s \n",filename.c_str());
#endif
#if 1
          if (EDG_ROSE_Translation::edg_include_file_map.find(filename) != EDG_ROSE_Translation::edg_include_file_map.end())
             {
               SgIncludeFile* includeFile = EDG_ROSE_Translation::edg_include_file_map[filename];

               ROSE_ASSERT(includeFile != NULL);

               associatedSourceFile = includeFile->get_source_file();
#if 0
               if (associatedSourceFile != NULL)
                  {
                    printf ("Found an associatedSourceFile = %s \n",associatedSourceFile->getFileName().c_str());
                  }
#endif
            // Note that rose_edg_required_macros_and_functions.h will not have a valid source file.
            // ROSE_ASSERT(associatedSourceFile != NULL);
             }
            else
             {
               associatedSourceFile = sourceFile;
               ROSE_ASSERT(associatedSourceFile != NULL);
             }
#endif

          if (associatedSourceFile != NULL)
             {
            // Find the internal map
               std::map<SgStatement*,FrontierNode*>::iterator j = frontierMap->begin();
               while (j != frontierMap->end())
                  {
                 // Find the internal map
                    SgStatement* statement     = j->first;
                    FrontierNode* frontierNode = j->second;

                    ROSE_ASSERT(statement    != NULL);
                    ROSE_ASSERT(frontierNode != NULL);
#if 0
                    printf ("physical_file_id = %d statement = %p = %30s frontierNode = %p display = %s \n",physical_file_id,
                         statement,statement->class_name().c_str(),frontierNode,frontierNode->display().c_str());
#endif
                 // Setup the map of SgStatement pointers to FrontierNode pointers.
                    token_unparse_frontier_map[statement] = frontierNode;
#if 1
                    int physical_file_id = generate_physical_file_id(statement);
                    ROSE_ASSERT(physical_file_id >= 0);

                    string name  = "token_frontier";
                    string color = select_dot_dark_color(physical_file_id);
#if 0
                    printf ("color = %s \n",color.c_str());
#endif
                 // string name    = "token_frontier";
                 // string options = "color=\"blue\"";
                 // string options = "penwidth=5, color=\"blue\"";

                    string options;
                    if (frontierNode->unparseUsingTokenStream == true)
                       {
                      // options = "fillcolor=\"greenyellow\",style=filled";
                         options = "color=\"xxx\", penwidth=5, fillcolor=\"greenyellow\",style=filled";
                       }
                      else
                       {
                      // DQ (5/16/2021): If this is not in the source file, then frontierNode->unparseFromTheAST == false.
                      // ROSE_ASSERT(frontierNode->unparseFromTheAST == true);
                         if (frontierNode->unparseFromTheAST == true)
                            {
                              options = "color=\"xxx\", penwidth=5, fillcolor=\"skyblue\",style=filled";
                            }
                       }
#if 0
                    printf ("before replace: options = %s \n",options.c_str());
#endif
                    if (frontierNode->unparseUsingTokenStream == true || frontierNode->unparseFromTheAST == true)
                       {
                         ROSE_ASSERT(options.find("xxx") != string::npos);
#if 1
                         if (frontierNode->node->isTransformation() == true)
                            {
                              options.replace(options.find("xxx"), sizeof("xxx") - 1, color);

                              if (frontierNode->unparseUsingTokenStream == true)
                                 {
                                   options.replace(options.find("greenyellow"), sizeof("greenyellow") - 1, "yellow");
                                 }
                                else
                                 {
                                   if (frontierNode->unparseFromTheAST == true)
                                      {
                                        options.replace(options.find("skyblue"), sizeof("skyblue") - 1, "yellow");
                                      }
                                     else
                                      {
#if 1
                                        printf ("Exiting as a test! \n");
                                        ROSE_ASSERT(false);
#endif
                                      }
                                 }
                            }
                           else
                            {
                              options.replace(options.find("xxx"), sizeof("xxx") - 1, color);
                            }
#else
                         options.replace(options.find("xxx"), sizeof("xxx") - 1, color);
#endif
#if 0
                         printf ("after replace: options = %s \n",options.c_str());
#endif
                      // DQ (5/17/2021): This is now computed from the FrontierNode objects.
                         FrontierDetectionForTokenStreamMappingAttribute* attribute = new FrontierDetectionForTokenStreamMappingAttribute ( statement, name, options);
#if 0
                         printf ("Adding attribute: test 5: skyblue or greenyellow \n");
#endif
                         statement->setAttribute(name,attribute);
                       }
                      else
                       {
                         ROSE_ASSERT(frontierNode->unparseUsingTokenStream == false && frontierNode->unparseFromTheAST == false);
                         printf ("Skipping the generation of the FrontierDetectionForTokenStreamMappingAttribute \n");
                       }
#endif
                    j++;
                  }

               associatedSourceFile->set_token_unparse_frontier(token_unparse_frontier_map);
#if 0
               printf ("associatedSourceFile->get_token_unparse_frontier().size() = %zu \n",associatedSourceFile->get_token_unparse_frontier().size());
#endif
             }

          i++;
        }

#if 0
     printf (" Frontier End \n");
#endif

  // sourceFile->set_token_unparse_frontier(token_unparse_frontier_map);

#if 0
  // DQ (5/16/2021): This fails for the new organization of frontierNodes.
  // DQ (5/15/2021): Testing the generated token_unparse_frontier map.
     std::map<SgStatement*,FrontierNode*> & frontier_nodes = sourceFile->get_token_unparse_frontier();

     SgGlobal* globalScope = sourceFile->get_globalScope();
     ROSE_ASSERT(globalScope != NULL);
     std::map<SgStatement*,FrontierNode*>::iterator i = frontier_nodes.find(globalScope);
     bool isFrontierNode = (i != frontier_nodes.end());
     FrontierNode* associatedFrontierNode = (isFrontierNode == true) ? i->second : NULL;
     ROSE_ASSERT(associatedFrontierNode != NULL);
#endif

#if 0
  // DQ (5/10/2021): Original code.
  // Output an optional graph of the AST (just the tree, so we can identify the frontier)
     SgFileList* fileList = isSgFileList(sourceFile->get_parent());

#if 0
     printf ("fileList                 = %p \n",fileList);
     printf ("sourceFile->get_parent() = %p \n",sourceFile->get_parent());
     if (sourceFile->get_parent() != NULL)
        {
          printf ("sourceFile->get_parent() = %p = %s \n",sourceFile->get_parent(),sourceFile->get_parent()->class_name().c_str());
        }
#endif

  // DQ (9/10/2018): Alternatives are either the SgFileList (for the source file) or the SgHeaderFileBody (for header files)
  // ROSE_ASSERT(fileList != NULL);
     SgProject* project = fileList != NULL ? isSgProject(fileList->get_parent()) : NULL;
#else
  // DQ (5/10/2021): This is a more uniform way to get the SgProject from any node in the AST.
     ROSE_ASSERT(sourceFile != NULL);
     SgProject* project = SageInterface::getEnclosingNode<SgProject>(sourceFile);
     ROSE_ASSERT(project != NULL);
#endif

     if (project != NULL)
        {
       // ROSE_ASSERT(project != NULL);

#if 0
          printf ("In frontierDetectionForTokenStreamMapping(): Generate the graph of the AST with the frontier defined \n");
          if ( SgProject::get_verbose() >= 2 )
             {
               generateDOT ( *project, "_token_unparsing_frontier" );
             }
#endif
        }

#if 0
     printf ("In frontierDetectionForTokenStreamMapping(): DONE: Generate the graph of the AST with the frontier defined \n");
#endif

  // Now traverse the AST and record the linked list of nodes to be unparsed as tokens and from the AST.
  // So that we can query next and last statements and determine if they were unparsed from the token 
  // stream or the AST.  Not clear if the edges of token-stream/AST unparsing should be unparsed from the 
  // token stream leading trailing token information or from the AST using the attached CPP info.

#if 0
     printf ("In frontierDetectionForTokenStreamMapping(): Calling previousAndNextNodeTraversal() \n");
#endif

#if 0
     string name_next     = "frontier_next";
     string name_previous = "frontier_previous";
  // string options = "color=\"red\"";
  // string options = "penwidth=5, color=\"red\"";
     string options = "weight=0, penwidth=5, color=\"red\"";

  // DQ (5/16/2021): Moved to the traversal class.
  // std::map<SgNode*,PreviousAndNextNodeData*> previousAndNextNodeMap = previousAndNextNodeTraversal(sourceFile);
  // std::map<SgNode*,PreviousAndNextNodeData*> previousAndNextNodeMap = computePreviousAndNextNodes(sourceFile->get_globalScope(),topAttribute.frontierNodes);
     std::map<SgNode*,PreviousAndNextNodeData*> previousAndNextNodeMap = computePreviousAndNextNodes(sourceFile->get_globalScope(),fdTraversal.frontierNodes);

     sourceFile->set_token_unparse_frontier_adjacency(previousAndNextNodeMap);
#endif

  // sourceFile->set_token_unparse_frontier(topAttribute.frontierNodes);

#if 0
     printf ("Output previousAndNextNodeMap for frontier: \n");
#endif

#if 0
  // DQ (5/10/2021): Disabled the red edges between frontier nodes (testing).
     for (size_t j = 0; j < topAttribute.frontierNodes.size(); j++)
        {
          ROSE_ASSERT(topAttribute.frontierNodes[j]->node != NULL);
          FrontierNode* frontierNode = topAttribute.frontierNodes[j];
          ROSE_ASSERT(frontierNode != NULL);
          SgStatement* frontier_statement = frontierNode->node;
          ROSE_ASSERT(frontier_statement != NULL);
#if 0
          printf ("frontier_statement = %p = %s \n",frontier_statement,frontier_statement->class_name().c_str());
#endif
          std::map<SgNode*,PreviousAndNextNodeData*>::iterator i = previousAndNextNodeMap.find(frontier_statement);
       // ROSE_ASSERT(i != previousAndNextNodeMap.end());
          if (i != previousAndNextNodeMap.end())
             {
#if 0
               printf ("   --- it for i->first = %p = %s i->second = %p i->second->previous = %p = %s i->second->next = %p = %s \n",
                    i->first,i->first->class_name().c_str(),i->second,
                    i->second->previous,i->second->previous != NULL ? i->second->previous->class_name().c_str() : "null",
                    i->second->next,i->second->next != NULL ? i->second->next->class_name().c_str() : "null");
#endif
               SgStatement* statement = isSgStatement(i->first);
               if (statement != NULL)
                  {
                    PreviousAndNextAttribute* edgeAttribute_next = new PreviousAndNextAttribute(i->first, i->second->next, name_next, options);
                    statement->setAttribute(name_next,edgeAttribute_next);
                    PreviousAndNextAttribute* edgeAttribute_previous = new PreviousAndNextAttribute(i->first, i->second->previous, name_previous, options);
                    statement->setAttribute(name_previous,edgeAttribute_previous);
                  }
             }
            else
             {
               printf ("WARNING: --- could not find associated IR node in previousAndNextNodeMap \n");
             }
        }
#endif

#if 0
     printf ("In frontierDetectionForTokenStreamMapping(): Generate the graph of the AST with the frontier AND edges defined \n");
     if (project != NULL)
        {
          if ( SgProject::get_verbose() >= 2 )
             {
               generateDOT(*project,"_token_unparsing_frontier_with_next_previous_edges");
             }
        }
#endif

#if 0
     printf ("In frontierDetectionForTokenStreamMapping(): DONE: Generate the graph of the AST with the frontier AND edges defined \n");
#endif

#if 0
     printf ("In frontierDetectionForTokenStreamMapping(): DONE: Calling previousAndNextNodeTraversal() \n");
#endif

#if 0
  // DQ (11/20/2013): Test using support for multiple files for Java testing.
  // Output an optional graph of the AST (just the tree, when active)
  // generateDOT ( *project );
  // SgProject* project = isSgProject(sourceFile->get_project());
  // SgProject* project = sourceFile->get_project();
     ROSE_ASSERT(project != NULL);

     printf ("In frontierDetectionForTokenStreamMapping(): Output dot file for project \n");

     generateDOTforMultipleFile(*project,"in_frontierDetectionForTokenStreamMapping");
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ABORT();
#endif
   }




FrontierDetectionForTokenStreamMappingAttribute::
FrontierDetectionForTokenStreamMappingAttribute(SgNode* n, std::string name, std::string options)
// DQ (3/25/2017): Remove to avoid Clang warning about unused private variable.
// : node(n), 
   : name(name), options(options)
   {
#if 0
     printf ("In FrontierDetectionForTokenStreamMappingAttribute constructor! name = %s options = %s \n",name.c_str(),options.c_str());
#endif
  // ROSE_ASSERT(false);
   }

FrontierDetectionForTokenStreamMappingAttribute::
FrontierDetectionForTokenStreamMappingAttribute(const FrontierDetectionForTokenStreamMappingAttribute & X)
   {
     printf ("In FrontierDetectionForTokenStreamMappingAttribute copy constructor! \n");
     ROSE_ABORT();
   }

string
FrontierDetectionForTokenStreamMappingAttribute::additionalNodeOptions()
   {
#if 0
     printf ("Error: FrontierDetectionForTokenStreamMappingAttribute::additionalNodeOptions(): not implemented! \n");
#endif
  // ROSE_ASSERT(false);

  // return "red";
  // return "color=\"red\"";
     return options;
   }


vector<AstAttribute::AttributeEdgeInfo> 
FrontierDetectionForTokenStreamMappingAttribute::additionalEdgeInfo()
   {
#if 0
     printf ("Error: FrontierDetectionForTokenStreamMappingAttribute::additionalEdgeInfo(): not implemented! \n");
#endif
  // ROSE_ASSERT(false);

     return vector<AstAttribute::AttributeEdgeInfo>();
   }


vector<AstAttribute::AttributeNodeInfo> 
FrontierDetectionForTokenStreamMappingAttribute::additionalNodeInfo()
   {
#if 0
     printf ("Error: FrontierDetectionForTokenStreamMappingAttribute::additionalNodeInfo(): not implemented! \n");
#endif
  // ROSE_ASSERT(false);

     return vector<AstAttribute::AttributeNodeInfo>();
   }

AstAttribute*
FrontierDetectionForTokenStreamMappingAttribute::copy() const
   {
  // Support for the copying of AST and associated attributes on each IR node (required for attributes 
  // derived from AstAttribute, else just the base class AstAttribute will be copied).

#if 0
     printf ("Error: FrontierDetectionForTokenStreamMappingAttribute::copy(): not implemented! \n");
     ROSE_ABORT();

     return new FrontierDetectionForTokenStreamMappingAttribute(*this);
#else
     printf ("Warning: FrontierDetectionForTokenStreamMappingAttribute::copy(): not implemented! \n");
     return NULL;
#endif
   }

// DQ (11/14/2017): This addition is not portable, should not be specified outside of the class definition, 
// and fails for C++11 mode on the GNU 4.8.5 compiler and the llvm (some version that Craig used).
// DQ (6/11/2017): Added virtual function now required to eliminate warning at runtime.
AstAttribute::OwnershipPolicy
FrontierDetectionForTokenStreamMappingAttribute::getOwnershipPolicy() const // override
   {
     return CONTAINER_OWNERSHIP;
   }


#if 0
void
FrontierDetectionForTokenStreamMappingAttribute::set_unparseUsingTokenStream( bool flag )
   {
     unparseUsingTokenStream = flag;
   }

bool
FrontierDetectionForTokenStreamMappingAttribute::get_unparseUsingTokenStream()
   {
     return unparseUsingTokenStream;
   }


void
FrontierDetectionForTokenStreamMappingAttribute::set_unparseFromTheAST( bool flag )
   {
     unparseFromTheAST = flag;
   }

bool
FrontierDetectionForTokenStreamMappingAttribute::get_unparseFromTheAST()
   {
     return unparseFromTheAST;
   }

void
FrontierDetectionForTokenStreamMappingAttribute::set_containsNodesToBeUnparsedFromTheAST( bool flag )
   {
     containsNodesToBeUnparsedFromTheAST = flag;
   }

bool
FrontierDetectionForTokenStreamMappingAttribute::get_containsNodesToBeUnparsedFromTheAST()
   {
     return containsNodesToBeUnparsedFromTheAST;
   }
#endif


FrontierNode::FrontierNode(SgStatement* node, bool unparseUsingTokenStream, bool unparseFromTheAST)
   : node(node), 
     unparseUsingTokenStream(unparseUsingTokenStream), 
     unparseFromTheAST(unparseFromTheAST),
     redundant_token_subsequence(false)
   {
  // Enforce specific constraints.
     ROSE_ASSERT(node != NULL);
  // ROSE_ASSERT( (unparseUsingTokenStream == true && unparseFromTheAST == false) || (unparseUsingTokenStream == false && unparseFromTheAST == true) );
     ROSE_ASSERT( (unparseUsingTokenStream == true && unparseFromTheAST == false) || (unparseUsingTokenStream == false && unparseFromTheAST == true) || (unparseUsingTokenStream == false && unparseFromTheAST == false));
     ROSE_ASSERT(redundant_token_subsequence == false);

  // DQ (5/16/2021): Either one of the other of these should be true (we must unparse from either the AST or the token stream).
     ROSE_ASSERT((unparseUsingTokenStream == true || unparseFromTheAST == true));
   }


void
FrontierDetectionForTokenStreamMapping::addFrontierNode (SgStatement* statement, FrontierNode* frontierNode )
   {
  // std::map<int,std::vector<FrontierNode*> > frontierNodes;
  // std::map<int,std::map<SgStatement*,FrontierNode*> > frontierNodes;
  // std::map<int,std::map<SgStatement*,FrontierNode*>* > frontierNodes;

  // DQ (5/17/2021): We need to use generate_physical_file_id so that we can address the special cases such 
  // as SgFunctionParameterList, SgCtorInitializerList, etc.
  // int physical_file_id = statement->get_file_info()->get_physical_file_id();
     int physical_file_id = generate_physical_file_id(statement);

#if 0
     printf ("statement        = %p = %s \n",statement,statement->class_name().c_str());
     printf ("frontierNode     = %p \n",frontierNode);
     printf ("physical_file_id = %d \n",physical_file_id);
#endif

     ROSE_ASSERT(physical_file_id >= 0);

     ROSE_ASSERT(frontierNode->node == statement);

     if (frontierNodes.find(physical_file_id) == frontierNodes.end())
        {
          frontierNodes[physical_file_id] = new map<SgStatement*,FrontierNode*>();
        }

     ROSE_ASSERT(frontierNodes.find(physical_file_id) != frontierNodes.end());

     if (frontierNodes[physical_file_id]->find(statement) == frontierNodes[physical_file_id]->end())
        {
          std::map<SgStatement*,FrontierNode*>* frontierMap = frontierNodes[physical_file_id];

       // frontierNodes[physical_file_id]->insert<std::pair<SgStatement*,FrontierNode*> >(statement,frontierNode);
          frontierMap->insert(std::pair<SgStatement*,FrontierNode*>(statement,frontierNode));
        }

     ROSE_ASSERT(frontierNodes[physical_file_id]->find(statement) != frontierNodes[physical_file_id]->end());
   }


FrontierNode*
FrontierDetectionForTokenStreamMapping::getFrontierNode (SgStatement* statement )
   {
  // std::map<int,std::vector<FrontierNode*> > frontierNodes;
  // std::map<int,std::map<SgStatement*,FrontierNode*> > frontierNodes;
  // std::map<int,std::map<SgStatement*,FrontierNode*>* > frontierNodes;

     ROSE_ASSERT(statement != NULL);

  // DQ (5/23/2021): Using generate_physical_file_id() function instead.
  // int physical_file_id = statement->get_file_info()->get_physical_file_id();
     int physical_file_id = generate_physical_file_id(statement);

#if 0
     printf ("statement        = %p = %s \n",statement,statement->class_name().c_str());
     printf ("physical_file_id = %d \n",physical_file_id);
#endif

  // Make sure that the std::map<SgStatement*,FrontierNode*> is available in the frontierNodes.
     ROSE_ASSERT(frontierNodes.find(physical_file_id) != frontierNodes.end());

  // Make sure that the std::map<SgStatement*,FrontierNode*> has a valid entry for statement.
     ROSE_ASSERT(frontierNodes[physical_file_id]->find(statement) != frontierNodes[physical_file_id]->end());

     std::map<SgStatement*,FrontierNode*>* frontierMap = frontierNodes[physical_file_id];

     ROSE_ASSERT(frontierMap != NULL);

  // FrontierNode* frontierNode = frontierNodes[physical_file_id]->second->[statement];
     FrontierNode* frontierNode = frontierMap->operator[](statement);

#if 0
     printf ("frontierNode = %p \n",frontierNode);
#endif

     return frontierNode;
   }


void
FrontierDetectionForTokenStreamMapping::outputFrontierNodes()
   {
  // std::map<int,std::map<SgStatement*,FrontierNode*>* > frontierNodes;
     std::map<int,std::map<SgStatement*,FrontierNode*>* >::iterator i = frontierNodes.begin();
     while (i != frontierNodes.end())
        {
       // Find the internal map
          int physical_file_id = i->first;
          std::map<SgStatement*,FrontierNode*>* frontierMap = i->second;
          std::map<SgStatement*,FrontierNode*>::iterator j = frontierMap->begin();
          while (j != frontierMap->end())
             {
            // Find the internal map
               SgStatement* statement     = j->first;
               FrontierNode* frontierNode = j->second;

               ROSE_ASSERT(statement    != NULL);
               ROSE_ASSERT(frontierNode != NULL);

               printf ("physical_file_id = %d statement = %p = %25s frontierNode = %p display = %s \n",physical_file_id,
                    statement,statement->class_name().c_str(),frontierNode,frontierNode->display().c_str());

               j++;
             }

          i++;
        }
   }


std::string 
FrontierNode::display()
   {
     string s;

     s += string("node=") + StringUtility::numberToString(node) + string(",") + node->class_name() + string(":");
     s += string("(TS=")  + (unparseUsingTokenStream == true ? "true" : "false"); 
     s += string(",AST=") + (unparseFromTheAST == true ? "true" : "false") + ")";
     s += string(" ");

     return s;
   }

FrontierNode::FrontierNode(const FrontierNode & X)
   {
     operator=(X);
   }

FrontierNode FrontierNode::operator=(const FrontierNode & X)
   {
     node                        = X.node; 
     unparseUsingTokenStream     = X.unparseUsingTokenStream; 
     unparseFromTheAST           = X.unparseFromTheAST;
     redundant_token_subsequence = X.redundant_token_subsequence;

     return *this;
   }
