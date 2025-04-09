#include "sage3basic.h"
#include "tokenStreamMapping.h"
#include "previousAndNextNode.h"

using namespace std;
using namespace Rose;

// DQ (12/1/2013): Added switch to control testing mode for token unparsing.
// Test codes in the tests/nonsmoke/functional/roseTests/astTokenStreamTests directory turn on this 
// variable so that all regression tests can be processed to mix the unparsing of 
// the token stream with unparsing from the AST.
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
     ASSERT_require(value >= 0);
     ASSERT_require(value <= 678);

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
     sourceFile        = nullptr;
     processChildNodes = false;
     isFrontier = false;

     unparseUsingTokenStream = false;
     unparseFromTheAST       = false;

     isInCurrentFile = true;
     node            = nullptr;

     isPartOfTemplateInstantiation = false;
   }

FrontierDetectionForTokenStreamMapping_InheritedAttribute::
FrontierDetectionForTokenStreamMapping_InheritedAttribute(SgSourceFile* file, SgNode* n)
   {
     sourceFile = file;
     ASSERT_not_null(sourceFile);
     processChildNodes = false;
     isFrontier = false;

     unparseUsingTokenStream = false;
     unparseFromTheAST       = false;

     isInCurrentFile = true;
     node            = n;

     isPartOfTemplateInstantiation = false;
   }

FrontierDetectionForTokenStreamMapping_InheritedAttribute::
FrontierDetectionForTokenStreamMapping_InheritedAttribute(SgSourceFile* file)
   {
     sourceFile = file;
     ASSERT_not_null(sourceFile);

     processChildNodes = false;
     isFrontier = false;
     unparseUsingTokenStream = false;
     unparseFromTheAST = false;

     isInCurrentFile = true;
     node            = file;
     isPartOfTemplateInstantiation = false;
   }

FrontierDetectionForTokenStreamMapping_InheritedAttribute::
FrontierDetectionForTokenStreamMapping_InheritedAttribute(const FrontierDetectionForTokenStreamMapping_InheritedAttribute &X)
   {
     sourceFile        = X.sourceFile;
     processChildNodes = X.processChildNodes;

     isFrontier = X.isFrontier;

     unparseUsingTokenStream             = X.unparseUsingTokenStream;
     unparseFromTheAST                   = X.unparseFromTheAST;

     isInCurrentFile = X.isInCurrentFile;
     node            = X.node;

     isPartOfTemplateInstantiation = X.isPartOfTemplateInstantiation;
   }

FrontierDetectionForTokenStreamMapping_InheritedAttribute
FrontierDetectionForTokenStreamMapping_InheritedAttribute::operator= (const FrontierDetectionForTokenStreamMapping_InheritedAttribute &X)
   {
     sourceFile        = X.sourceFile;
     processChildNodes = X.processChildNodes;

     isFrontier = X.isFrontier;

     unparseUsingTokenStream             = X.unparseUsingTokenStream;
     unparseFromTheAST                   = X.unparseFromTheAST;

     isInCurrentFile = X.isInCurrentFile;
     node            = X.node;

     isPartOfTemplateInstantiation = X.isPartOfTemplateInstantiation;

     return *this;
   }


FrontierDetectionForTokenStreamMapping_SynthesizedAttribute::
FrontierDetectionForTokenStreamMapping_SynthesizedAttribute()
   {
     node       = nullptr;
     isFrontier = false;
     sourceFile = nullptr;

     unparseUsingTokenStream             = false;
     unparseFromTheAST                   = false;
     containsNodesToBeUnparsedFromTheAST = false;

     containsNodesToBeUnparsedFromTheTokenStream = false;
   }

FrontierDetectionForTokenStreamMapping_SynthesizedAttribute::
  FrontierDetectionForTokenStreamMapping_SynthesizedAttribute(SgNode* n, SgSourceFile* file)
   {
     ASSERT_not_null(n);
     ASSERT_not_null(file);

     node       = isSgStatement(n);
     isFrontier = false;
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
     sourceFile = X.sourceFile;

     unparseUsingTokenStream             = X.unparseUsingTokenStream;
     unparseFromTheAST                   = X.unparseFromTheAST;
     containsNodesToBeUnparsedFromTheAST = X.containsNodesToBeUnparsedFromTheAST;

     containsNodesToBeUnparsedFromTheTokenStream = X.containsNodesToBeUnparsedFromTheTokenStream;
   }

FrontierDetectionForTokenStreamMapping_SynthesizedAttribute
FrontierDetectionForTokenStreamMapping_SynthesizedAttribute::operator=(const FrontierDetectionForTokenStreamMapping_SynthesizedAttribute &X)
   {
     node       = X.node;
     isFrontier = X.isFrontier;
     sourceFile = X.sourceFile;

     unparseUsingTokenStream             = X.unparseUsingTokenStream;
     unparseFromTheAST                   = X.unparseFromTheAST;
     containsNodesToBeUnparsedFromTheAST = X.containsNodesToBeUnparsedFromTheAST;

     containsNodesToBeUnparsedFromTheTokenStream = X.containsNodesToBeUnparsedFromTheTokenStream;

     return *this;
   }


FrontierDetectionForTokenStreamMapping::
FrontierDetectionForTokenStreamMapping( SgSourceFile* sourceFile)
   {
  // This is the number of IR nodes in the current file and helpful in randomly 
  // marking IR nodes for testing to be either from the AST or from the token stream.
     numberOfNodes = numberOfNodesInSubtree(sourceFile);
   }

int
generate_physical_file_id(SgStatement* statement)
   {
     ASSERT_not_null(statement);

     int physical_file_id = statement->get_file_info()->get_physical_file_id();
     if (physical_file_id < 0)
        {
       // We need to translate any nodes that generate a negative physical_file_id 
       // to a non-negative value (usually by looking at the parent node).

          SgFunctionParameterList* functionParameterList = isSgFunctionParameterList(statement);
          if (functionParameterList != nullptr)
             {
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(statement->get_parent());
               ASSERT_not_null(functionDeclaration);

               int statement_physical_file_id = functionDeclaration->get_file_info()->get_physical_file_id();
               physical_file_id = statement_physical_file_id;
             }

          SgCtorInitializerList* ctorInitializerList = isSgCtorInitializerList(statement);
          if (ctorInitializerList != nullptr)
             {
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(statement->get_parent());
               ASSERT_not_null(functionDeclaration);

               int statement_physical_file_id = functionDeclaration->get_file_info()->get_physical_file_id();
               physical_file_id = statement_physical_file_id;
             }

       // DQ (5/22/2021): Added to support test2012_20.c test code.
          SgNullStatement* nullStatement = isSgNullStatement(statement);
          if (nullStatement != nullptr)
             {
               SgLocatedNode* locatedNode = isSgLocatedNode(statement->get_parent());
               ASSERT_not_null(locatedNode);

               int statement_physical_file_id = locatedNode->get_file_info()->get_physical_file_id();
               physical_file_id = statement_physical_file_id;
             }

       // DQ (5/22/2021): Added to fix issue in test2012_84.c
          SgBasicBlock* basicBlock = isSgBasicBlock(statement);
          if (basicBlock != nullptr)
             {
               ASSERT_not_null(basicBlock->get_parent());
               SgLocatedNode* basicBlock_parent = isSgLocatedNode(basicBlock->get_parent());
               ASSERT_not_null(basicBlock_parent);
               int statement_physical_file_id = basicBlock_parent->get_file_info()->get_physical_file_id();
               physical_file_id = statement_physical_file_id;
             }

          SgNamespaceDefinitionStatement* namespaceDefinition = isSgNamespaceDefinitionStatement(statement);
          if (namespaceDefinition != nullptr)
             {
               SgNamespaceDeclarationStatement* namespaceDeclaration = isSgNamespaceDeclarationStatement(namespaceDefinition->get_parent());
               ASSERT_not_null(namespaceDeclaration);

               int statement_physical_file_id = namespaceDeclaration->get_file_info()->get_physical_file_id();
               physical_file_id = statement_physical_file_id;
             }

          SgTryStmt* tryStatement = isSgTryStmt(statement);
          if (tryStatement != nullptr)
             {
               SgLocatedNode* tryStatement_parent = isSgLocatedNode(tryStatement->get_parent());
               ASSERT_not_null(tryStatement_parent);

               int statement_physical_file_id = tryStatement_parent->get_file_info()->get_physical_file_id();
               physical_file_id = statement_physical_file_id;
             }

          SgCatchStatementSeq* catchStatement = isSgCatchStatementSeq(statement);
          if (catchStatement != nullptr)
             {
               SgLocatedNode* catchStatement_parent = isSgLocatedNode(catchStatement->get_parent());
               ASSERT_not_null(catchStatement_parent);

               int statement_physical_file_id = catchStatement_parent->get_file_info()->get_physical_file_id();
               physical_file_id = statement_physical_file_id;
             }

          SgCatchOptionStmt* catchOptionStatement = isSgCatchOptionStmt(statement);
          if (catchOptionStatement != nullptr)
             {
               SgStatement* catchOptionStatement_parent = isSgStatement(catchOptionStatement->get_parent());
               ASSERT_not_null(catchOptionStatement_parent);

               int statement_physical_file_id = generate_physical_file_id(catchOptionStatement_parent);
               physical_file_id = statement_physical_file_id;
             }

          if (SageInterface::isTemplateInstantiationNode(statement) == true)
             {
               SgStatement* statement_parent = isSgStatement(statement->get_parent());
               ASSERT_not_null(statement_parent);

               int statement_physical_file_id = generate_physical_file_id(statement_parent);
               physical_file_id = statement_physical_file_id;
             }

          if (physical_file_id < 0)
             {
               SgLocatedNode* locatedNode = isSgLocatedNode(statement->get_parent());
               ASSERT_not_null(locatedNode);

            // DQ (5/22/2021): Added to fix issue in test2012_20.c
               SgGlobal* globalScope = isSgGlobal(locatedNode);
               if (globalScope != nullptr)
                  {
                    SgSourceFile* sourceFile = SageInterface::getEnclosingSourceFile(statement);
                    ASSERT_not_null(sourceFile);

                    int statement_physical_file_id = locatedNode->get_file_info()->get_physical_file_id();
                    physical_file_id = statement_physical_file_id;
                  }
             }

          if (physical_file_id < 0)
             {
               SgStatement* statement_parent = isSgStatement(statement->get_parent());
               ASSERT_not_null(statement_parent);

               int statement_physical_file_id = generate_physical_file_id(statement_parent);

               physical_file_id = statement_physical_file_id;
             }

          if (physical_file_id < 0)
             {
               statement->get_file_info()->display("Error: physical_file_id < 0: debug");

               SgLocatedNode* locatedNode = isSgLocatedNode(statement->get_parent());
               ASSERT_not_null(locatedNode);

               locatedNode->get_file_info()->display("Error: parent of node with physical_file_id < 0: debug");

               SgGlobal* globalScope = isSgGlobal(locatedNode);
               if (globalScope != nullptr)
                  {
                    SgSourceFile* sourceFile = SageInterface::getEnclosingSourceFile(statement);
                    ASSERT_not_null(sourceFile);
                  }
               ASSERT_require(false);
             }
        }

     return physical_file_id;
   }


bool isFromSameFile(int physical_file_id_1, SgStatement* statement)
   {
  // This is a supporting function for the isChildNodeFromSameFileAsCurrentNode() and isNodeFromCurrentFile() functions.
     ASSERT_not_null(statement);

     bool return_value = false;
     int physical_file_id_2  = generate_physical_file_id(statement);

     return_value = (physical_file_id_1 == physical_file_id_2);

     return return_value;
   }


bool
FrontierDetectionForTokenStreamMapping::isChildNodeFromSameFileAsCurrentNode (SgStatement* statement, SgStatement* child_statement)
   {
  // DQ (5/10/2021): Add test for if this child node is from the same physical file.
     bool return_value = false;

     if (statement != nullptr && child_statement != nullptr)
        {
          int statement_file_id = generate_physical_file_id(statement);
          return_value = isFromSameFile(statement_file_id,child_statement);
        }

     return return_value;
   }


bool
FrontierDetectionForTokenStreamMapping_InheritedAttribute::isNodeFromCurrentFile(SgStatement* statement)
   {
     ASSERT_not_null(statement);
     ASSERT_not_null(sourceFile);

     bool return_value = false;
     int sourceFile_physical_file_id = sourceFile->get_file_info()->get_physical_file_id();

     return_value = isFromSameFile(sourceFile_physical_file_id,statement);

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

     bool isTemplateInstantiationNode = false;
     if (statement != nullptr)
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

#if DEBUG_INHERIT || 0
     ASSERT_not_null(inheritedAttribute.sourceFile);
     printf ("inheritedAttribute.sourceFile              = %p \n",inheritedAttribute.sourceFile);
     printf ("inheritedAttribute.unparseFromTheAST       = %s \n",inheritedAttribute.unparseFromTheAST ? "true" : "false");
     printf ("inheritedAttribute.unparseUsingTokenStream = %s \n",inheritedAttribute.unparseUsingTokenStream ? "true" : "false");
#endif

     if (n->get_file_info()->isFrontendSpecific() == false && statement != nullptr && isSgSourceFile(n) == nullptr && isSgGlobal(n) == nullptr)
        {
       // Count the IR nodes traversed so that we can make a subset transformations.
          random_counter++;
        }

     ASSERT_not_null(inheritedAttribute.sourceFile);

#if DEBUG_INHERIT || 0
     printf ("Building returnAttribute: inheritedAttribute.sourceFile = %s \n",inheritedAttribute.sourceFile->getFileName().c_str());
#endif

     FrontierDetectionForTokenStreamMapping_InheritedAttribute returnAttribute(inheritedAttribute.sourceFile,n);

     if (isTemplateInstantiationNode == true || inheritedAttribute.isPartOfTemplateInstantiation == true)
        {
          returnAttribute.isPartOfTemplateInstantiation = true;
          ASSERT_require(returnAttribute.isPartOfTemplateInstantiation == true);
        }

     if (statement != nullptr && isTemplateInstantiationNode == false && inheritedAttribute.isPartOfTemplateInstantiation == false)
        {
#if DEBUG_INHERIT
          printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): random_counter = %d statement = %p = %s \n",random_counter,statement,statement->class_name().c_str());
          printf (" --- statement = %s \n",SageInterface::get_name(statement).c_str());
#endif

          int physical_file_id = generate_physical_file_id(statement);
          ASSERT_require(physical_file_id >= 0);

          string color = select_dot_dark_color(physical_file_id);

#if DEBUG_INHERIT
          printf ("color = %s \n",color.c_str());
#endif
          string name    = "token_frontier";
          string options = "penwidth=5, color=\"blue\"";

          options.replace(options.find("blue"), sizeof("blue") - 1, color);

          bool nodeIsFromCurrentFile = inheritedAttribute.isNodeFromCurrentFile(statement);

#if DEBUG_INHERIT || 0
          printf ("nodeIsFromCurrentFile = %s \n",nodeIsFromCurrentFile ? "true" : "false");
#endif

       // This will later just check if this is a statement marked as a transformation or that it has been modified (checking the SgNode->isModified member flag).
       // Mark the middle 50% of IR nodes to come from the AST, instead of the token stream.
          int lowerbound = -1;
          int upperbound = -1;

       // Unparse the middle of the AST from the AST and the outer 25% from the token stream.
          bool forceUnparseFromTokenStream = false;
          bool forceUnparseFromAST         = false;

#if DEBUG_INHERIT
          printf ("ROSE_tokenUnparsingTestingMode = %s \n",ROSE_tokenUnparsingTestingMode ? "true" : "false");
#endif

       // DQ (5/31/2021): The support for a testing mode is not moved to a seperate traversal (artificialFrontier.C).
       // DQ (12/1/2013): Added switch to control testing mode for token unparsing.
       // Test codes in the tests/nonsmoke/functional/roseTests/astTokenStreamTests directory turn on this 
       // variable so that all regression tests can be processed to mix the unparsing of 
       // the token stream with unparsing from the AST.
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
                         ASSERT_require(forceUnparseFromTokenStream == false);
                         lowerbound = -1;
                         upperbound = max_numberOfNodes;
                       }
                      else
                       {
                         ASSERT_require(forceUnparseFromTokenStream == true);

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
                    ASSERT_require(forceUnparseFromAST == false);
                    ASSERT_require(forceUnparseFromTokenStream == false);
#if DEBUG_INHERIT || 1
                    printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): numberOfNodes = %d \n",numberOfNodes);
#endif
                    lowerbound = numberOfNodes / 4;
                    upperbound = 3 * (numberOfNodes / 4);
                  }

#if DEBUG_INHERIT || 1
               printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): random_counter = %d lowerbound = %d upperbound = %d \n",random_counter,lowerbound,upperbound);
#endif
               if ( random_counter >= lowerbound && random_counter <= upperbound )
                  {
#if DEBUG_INHERIT || 1
                    printf ("In FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): Mark this statement as a transformation: random_counter = %d statement = %p = %s \n",
                         random_counter,statement,statement->class_name().c_str());
#endif
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

               if (statement->get_file_info()->isTransformation() == true)
                  {
#if DEBUG_INHERIT
                    printf ("Found an AST transformation: statement = %p = %s \n",statement,statement->class_name().c_str());
#endif
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
                    ASSERT_require(returnAttribute.unparseFromTheAST == false);
                  }
             }

          ASSERT_not_null(returnAttribute.sourceFile);

          if (nodeIsFromCurrentFile == false)
             {
            // DQ (5/23/2021): This can happen when the current node is listed as compiler generated (where the source position was not available in the frontend).

#if DEBUG_INHERIT
               printf ("nodeIsFromCurrentFile == false: setting returnAttribute.isInCurrentFile = false \n");
#endif
               returnAttribute.isInCurrentFile = false;
               int nested_physical_file_id = physical_file_id;

               if (nested_physical_file_id < 0)
                  {
                    printf ("Error: nested_physical_file_id < 0: statement = %p = %s name = %s \n",statement,statement->class_name().c_str(),SageInterface::get_name(statement).c_str());
                    SgLocatedNode* locatedNode = isSgLocatedNode(statement->get_parent());
                    ASSERT_not_null(locatedNode);
                    locatedNode->get_file_info()->display("ERROR: nested_physical_file_id < 0");
                  }
               ASSERT_require(nested_physical_file_id >= 0);

               string filename = Sg_File_Info::getFilenameFromID(nested_physical_file_id);

#if DEBUG_INHERIT
               printf ("looking for header file: filename = %s \n",filename.c_str());
#endif
               SgSourceFile* include_source_file = nullptr;
               if (EDG_ROSE_Translation::edg_include_file_map.find(filename) != EDG_ROSE_Translation::edg_include_file_map.end())
                  {
                    SgIncludeFile* includeFile = EDG_ROSE_Translation::edg_include_file_map[filename];
                    ASSERT_not_null(includeFile);

                    include_source_file = includeFile->get_source_file();

                    if (include_source_file != nullptr)
                       {
                         returnAttribute.sourceFile = include_source_file;
                       }
                      else
                       {
                      // If the source file is unavailable, then don't change the returnAttribute.sourceFile.
#if DEBUG_INHERIT
                         printf ("header file unavailable: filename = %s \n",filename.c_str());
#endif
                       }
                  }
                 else
                  {
#if DEBUG_INHERIT
                    printf ("filename not found in EDG_ROSE_Translation::edg_include_file_map (so include_source_file == NULL) \n");
#endif
                  }

               ASSERT_not_null(returnAttribute.sourceFile);
             }

       // DQ (11/30/2013): Allow us to ignore class defintions in typedefs.
       // Mark the whole subtree as being unparsed from the AST so that synthizized attributes can be more esily computed.
          if (inheritedAttribute.unparseFromTheAST == true)
             {
#if DEBUG_INHERIT
               printf ("   --- Where inheritedAttribute.unparseFromTheAST == true: set returnAttribute.unparseFromTheAST == true and returnAttribute.unparseUsingTokenStream = false \n");
#endif
               returnAttribute.unparseFromTheAST       = true;
               returnAttribute.unparseUsingTokenStream = false;
             }

#if DEBUG_INHERIT || 0
          printf ("   --- returnAttribute.isFrontier                                  = %s \n",returnAttribute.isFrontier ? "true" : "false");
          printf ("   --- returnAttribute.unparseFromTheAST                           = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
          printf ("   --- returnAttribute.unparseUsingTokenStream                     = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
          printf ("   --- returnAttribute.isInCurrentFile                             = %s \n",returnAttribute.isInCurrentFile ? "true" : "false");
#endif
        }
       else
        {

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
#endif
        }

#if DEBUG_INHERIT
     printf ("IIIIIIIIIIIIIIIIIIIIIIIIII \n");
     printf ("*** Leaving FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(): n = %p = %s \n",n,n->class_name().c_str());
     printf ("IIIIIIIIIIIIIIIIIIIIIIIIII \n");
#endif

     ASSERT_require( (returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false) ||
                     (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true) );

     if (returnAttribute.isFrontier == true)
        {
          ASSERT_require(returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true);
        }

     ASSERT_not_null(returnAttribute.sourceFile);

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

     ASSERT_not_null(n);

#define DEBUG_SYNTH 0

#if DEBUG_SYNTH || 0
     printf ("\n\nSSSSSSSSSSSSSSSSSSSSSSSSSS \n");
     printf ("### In FrontierDetectionForTokenStreamMapping::evaluateSynthesizedAttribute(): TOP n = %p = %s \n",n,n->class_name().c_str());
     printf ("SSSSSSSSSSSSSSSSSSSSSSSSSS \n");
#endif

     ASSERT_require(n == inheritedAttribute.node);
     FrontierDetectionForTokenStreamMapping_SynthesizedAttribute returnAttribute(n,inheritedAttribute.sourceFile);

  // We need to handle SgStatement, plus the SgSourceFile because we need to copy synthesized results from the SgGlobal to the SgSourceFile.
     SgStatement*  statement  = isSgStatement(n);
     SgSourceFile* sourceFile = isSgSourceFile(n);

     if ((statement != nullptr || sourceFile != nullptr) && inheritedAttribute.isPartOfTemplateInstantiation == false)
        {
#if DEBUG_SYNTH
          printf ("In FrontierDetectionForTokenStreamMapping::evaluateSynthesizedAttribute(): SgStatement or SgSourceFile = %p = %s \n",n,n->class_name().c_str());
          if (statement != nullptr)
             {
               printf (" --- statement = %s \n",SageInterface::get_name(statement).c_str());
             }
            else
             {
               printf (" --- sourceFile = %s \n",SageInterface::get_name(sourceFile).c_str());
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

          ASSERT_require(returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == false);

          if (returnAttribute.containsNodesToBeUnparsedFromTheAST == false)
             {
               SgIfStmt* ifStatement = isSgIfStmt(statement);
               bool specialCaseNode = false;
          
               if (ifStatement != nullptr)
                  {
                 // There are special cases where we don't (at least presently) want to mix the two types on unparsing.
                 // This is how those special cases are handled.
                    if (synthesizedAttributeList[0].containsNodesToBeUnparsedFromTheAST != synthesizedAttributeList[1].containsNodesToBeUnparsedFromTheAST)
                       {
                         specialCaseNode = true;
                       }
                  }

               SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(statement);
               if (typedefDeclaration != nullptr)
                  {
                    if (synthesizedAttributeList[0].containsNodesToBeUnparsedFromTheAST == true && synthesizedAttributeList[0].containsNodesToBeUnparsedFromTheTokenStream == true)
                       {
                         specialCaseNode = true;
                       }
                  }

            // DQ (12/1/2013): This handles the details of the SgForStatement (which has 3 relevant children (excluding the body).
               SgForStatement* forStatement = isSgForStatement(statement);
               if (forStatement != nullptr)
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
                              synthesizedAttributeList[i].node != nullptr                                ? synthesizedAttributeList[i].node->class_name().c_str() : "null",
                              synthesizedAttributeList[i].isFrontier                                  ? "true " : "false",
                              synthesizedAttributeList[i].unparseUsingTokenStream                     ? "true " : "false",
                              synthesizedAttributeList[i].unparseFromTheAST                           ? "true " : "false",
                              synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST         ? "true " : "false",
                              synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheTokenStream ? "true " : "false");
                       }
#endif
                 // I don't know why unparseUsingTokenStream are different, but the containsNodesToBeUnparsedFromTheAST are the same.
                    if ( (synthesizedAttributeList[0].unparseUsingTokenStream != synthesizedAttributeList[1].unparseUsingTokenStream) ||
                         (synthesizedAttributeList[0].unparseUsingTokenStream != synthesizedAttributeList[2].unparseUsingTokenStream) )
                       {
#if DEBUG_SYNTH
                         printf ("This node (SgForStatement) has children that mix the two different types of unparsing! \n");
#endif
                         specialCaseNode = true;
                       }
                  }

#if DEBUG_SYNTH
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
                    ASSERT_not_null(frontierNode);

                    addFrontierNode (statement, frontierNode);
                  }
                 else
                  {
                 // This is the non-special case.
                    ASSERT_require(specialCaseNode == false);
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

#if DEBUG_SYNTH || 0
                         printf ("child_synthesized_attribute_statement            = %p \n",child_synthesized_attribute_statement);
                         printf ("inheritedAttribute.isPartOfTemplateInstantiation = %s \n",inheritedAttribute.isPartOfTemplateInstantiation ? "true" : "false");
#endif
                         if (child_synthesized_attribute_statement != nullptr && inheritedAttribute.isPartOfTemplateInstantiation == false)
                            {
#if DEBUG_SYNTH
                              printf ("Before building FrontierNode (child_synthesized_attribute_statement != NULL): \n");
                              printf (" --- returnAttribute.unparseFromTheAST                           = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
                              printf (" --- returnAttribute.unparseUsingTokenStream                     = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
                              printf (" --- returnAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
                              printf (" --- returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif

                              bool childNodeIsFromSameFileAsCurrentNode = isChildNodeFromSameFileAsCurrentNode (statement, child_synthesized_attribute_statement);
                           // DQ (5/18/2021): New version of code to address how a node from a header file can  
                           // not effect the decision to unparse from the token stream or AST in the parent file.
                              FrontierNode* frontierNode = nullptr;
                              if (childNodeIsFromSameFileAsCurrentNode == false)
                                 {
                                // Can we set these? Yes, the child synthesizedAttributeList array can be modified.  This is useful in
                                // embeddeding the header file information into the analysis for where to unparse from the token stream.
                                   synthesizedAttributeList[i].unparseFromTheAST       = false;
                                   synthesizedAttributeList[i].unparseUsingTokenStream = true;

                                   bool local_unparseFromTheAST       = false;
                                   bool local_unparseUsingTokenStream = true;

                                   frontierNode = new FrontierNode(child_synthesized_attribute_statement,local_unparseUsingTokenStream,local_unparseFromTheAST);
                                   ASSERT_not_null(frontierNode);
                                 }
                                else
                                 {
                                   frontierNode = new FrontierNode(child_synthesized_attribute_statement,synthesizedAttributeList[i].unparseUsingTokenStream,synthesizedAttributeList[i].unparseFromTheAST);
                                   ASSERT_not_null(frontierNode);
                                 }

                              ASSERT_not_null(frontierNode);

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
                                   addFrontierNode (synthesizedAttributeList[i].node, frontierNode);

                                   returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = true;
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
                                        addFrontierNode (synthesizedAttributeList[i].node, frontierNode);
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
                                        printf ("Current node = %p = %s not added to frontier node list (add any lists from frontierNodes.size() = %" PRIuPTR ") \n",
                                             n,n->class_name().c_str(),frontierNodes.size());
#endif
                                     // Lack of AST usage in the subtree implies that there are nodes unparsed from the token stream.
                                        if (synthesizedAttributeList[i].containsNodesToBeUnparsedFromTheAST == false)
                                         {
                                           returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = true;
                                         }
                                      }
                                 }
#if DEBUG_SYNTH || 0
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
                              returnAttribute.node,returnAttribute.node != nullptr ? returnAttribute.node->class_name().c_str() : "null");
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

             }
            else
             {
               ASSERT_require(returnAttribute.containsNodesToBeUnparsedFromTheAST == true);
             }

#if DEBUG_SYNTH
          printf ("* after processing of frontierNodes(): size                      = %zu \n",frontierNodes.size());
          printf (" --- returnAttribute.isFrontier                                  = %s \n",returnAttribute.isFrontier ? "true" : "false");
          printf (" --- returnAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
          printf (" --- returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif

       // DQ (5/15/2021): Each should be false before this point.
          ASSERT_require(returnAttribute.unparseFromTheAST == false && returnAttribute.unparseUsingTokenStream == false);

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
                    if (returnAttribute.containsNodesToBeUnparsedFromTheAST == true  && returnAttribute.containsNodesToBeUnparsedFromTheTokenStream == false)
                       {
                         returnAttribute.unparseFromTheAST = true;
                       }
                      else
                       {
                         if (returnAttribute.containsNodesToBeUnparsedFromTheAST == false  && returnAttribute.containsNodesToBeUnparsedFromTheTokenStream == false)
                            {
                           // DQ (5/16/2021): Just because returnAttribute.containsNodesToBeUnparsedFromTheTokenStream == false does
                           // not imply that this node should be unpared from the AST (e.g. the case of an empty SgBasicBlock).
                              returnAttribute.unparseUsingTokenStream = true;
                            }
                           else
                            {
                              printf ("I don't think that this case can happen! \n");
                              printf ("Exiting as a test! \n");
                              ASSERT_require(false);
                            }
                       }
                  }
             }

          if (statement != nullptr && returnAttribute.containsNodesToBeUnparsedFromTheAST == true)
             {
#if DEBUG_SYNTH
               printf ("Before building FrontierNode (statement != NULL && returnAttribute.containsNodesToBeUnparsedFromTheAST == true): \n");
               printf (" --- returnAttribute.unparseFromTheAST                           = %s \n",returnAttribute.unparseFromTheAST ? "true" : "false");
               printf (" --- returnAttribute.unparseUsingTokenStream                     = %s \n",returnAttribute.unparseUsingTokenStream ? "true" : "false");
               printf (" --- returnAttribute.containsNodesToBeUnparsedFromTheAST         = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheAST ? "true" : "false");
               printf (" --- returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = %s \n",returnAttribute.containsNodesToBeUnparsedFromTheTokenStream ? "true" : "false");
#endif
               FrontierNode* frontierNode = new FrontierNode(statement,returnAttribute.unparseUsingTokenStream,returnAttribute.unparseFromTheAST);

               addFrontierNode (statement, frontierNode);
             }
            else
             {
            // This case happens for test_CplusplusMacro_C.c, which has nothing but CPP directives.
             }

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
          ASSERT_require( (returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false) ||
                          (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true)  ||
                          (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == false) );
        }
       else
        {
       // DQ (5/15/2021): This characterizes this false branch.
          ASSERT_require((statement == nullptr && sourceFile == nullptr) || inheritedAttribute.isPartOfTemplateInstantiation == true);

          returnAttribute.isFrontier                                  = false;
          returnAttribute.unparseUsingTokenStream                     = false;
          returnAttribute.unparseFromTheAST                           = false;
          returnAttribute.containsNodesToBeUnparsedFromTheAST         = false;
          returnAttribute.containsNodesToBeUnparsedFromTheTokenStream = false;

          returnAttribute.isFrontier                                  = inheritedAttribute.isFrontier;
          returnAttribute.unparseUsingTokenStream                     = inheritedAttribute.unparseUsingTokenStream;
          returnAttribute.unparseFromTheAST                           = inheritedAttribute.unparseFromTheAST;
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

     ASSERT_require( (returnAttribute.unparseUsingTokenStream == true  && returnAttribute.unparseFromTheAST == false) ||
                     (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == true)  ||
                     (returnAttribute.unparseUsingTokenStream == false && returnAttribute.unparseFromTheAST == false) );

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
              void visit ( SgNode* n ) { if (isSgStatement(n) != nullptr) count++; }
        };

     CountTraversal counter;
     counter.traverseWithinFile(sourceFile,preorder);
     value = counter.count;

     return value;
   }

void
frontierDetectionForTokenStreamMapping(SgSourceFile* sourceFile, bool traverseHeaderFiles)
   {
  // This frontier detection happens before we associate token subsequences to the AST (in a seperate map).
     ASSERT_not_null(sourceFile);

     FrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute(sourceFile);
     FrontierDetectionForTokenStreamMapping fdTraversal(sourceFile);

     FrontierDetectionForTokenStreamMapping_SynthesizedAttribute topAttribute;
     if (traverseHeaderFiles == false)
       {
         topAttribute = fdTraversal.traverseWithinFile(sourceFile,inheritedAttribute);
       }
      else
       {
         topAttribute = fdTraversal.traverse(sourceFile,inheritedAttribute);
       }
     ASSERT_require(fdTraversal.frontierNodes.size() > 0);

     std::map<int,std::map<SgStatement*,FrontierNode*>* >::iterator i = fdTraversal.frontierNodes.begin();
     while (i != fdTraversal.frontierNodes.end())
        {
          map<SgStatement*,FrontierNode*> token_unparse_frontier_map;

          int physical_file_id = i->first;
          std::map<SgStatement*,FrontierNode*>* frontierMap = i->second;

       // Find the associated file
          SgSourceFile* associatedSourceFile = nullptr;
          string filename = Sg_File_Info::getFilenameFromID(physical_file_id);

          if (EDG_ROSE_Translation::edg_include_file_map.find(filename) != EDG_ROSE_Translation::edg_include_file_map.end())
             {
               SgIncludeFile* includeFile = EDG_ROSE_Translation::edg_include_file_map[filename];
               ASSERT_not_null(includeFile);

               associatedSourceFile = includeFile->get_source_file();
             }
            else
             {
               associatedSourceFile = sourceFile;
               ASSERT_not_null(associatedSourceFile);
             }

          if (associatedSourceFile != nullptr)
             {
            // Find the internal map
               std::map<SgStatement*,FrontierNode*>::iterator j = frontierMap->begin();
               while (j != frontierMap->end())
                  {
                 // Find the internal map
                    SgStatement* statement     = j->first;
                    FrontierNode* frontierNode = j->second;

                    ASSERT_not_null(statement);
                    ASSERT_not_null(frontierNode);

                 // Setup the map of SgStatement pointers to FrontierNode pointers.
                    token_unparse_frontier_map[statement] = frontierNode;
                    int physical_file_id = generate_physical_file_id(statement);
                    ASSERT_require(physical_file_id >= 0);

                    string name  = "token_frontier";
                    string color = select_dot_dark_color(physical_file_id);

                    string options;
                    if (frontierNode->unparseUsingTokenStream == true)
                       {
                         options = "color=\"xxx\", penwidth=5, fillcolor=\"greenyellow\",style=filled";
                       }
                      else
                       {
                      // DQ (5/16/2021): If this is not in the source file, then frontierNode->unparseFromTheAST == false.
                         if (frontierNode->unparseFromTheAST == true)
                            {
                              options = "color=\"xxx\", penwidth=5, fillcolor=\"skyblue\",style=filled";
                            }
                       }

                    if (frontierNode->unparseUsingTokenStream == true || frontierNode->unparseFromTheAST == true)
                       {
                         ASSERT_require(options.find("xxx") != string::npos);

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
                                        printf ("Exiting as a test! \n");
                                        ASSERT_require(false);
                                      }
                                 }
                            }
                           else
                            {
                              options.replace(options.find("xxx"), sizeof("xxx") - 1, color);
                            }

                         FrontierDetectionForTokenStreamMappingAttribute* attribute = new FrontierDetectionForTokenStreamMappingAttribute ( statement, name, options);
                         statement->setAttribute(name,attribute);
                       }
                      else
                       {
                         ASSERT_require(frontierNode->unparseUsingTokenStream == false && frontierNode->unparseFromTheAST == false);
                         printf ("Skipping the generation of the FrontierDetectionForTokenStreamMappingAttribute \n");
                       }
                    j++;
                  }

               associatedSourceFile->set_token_unparse_frontier(token_unparse_frontier_map);
             }

          i++;
        }

     ASSERT_not_null(sourceFile);
     SgProject* project = SageInterface::getEnclosingNode<SgProject>(sourceFile);
     ASSERT_not_null(project);

  // Now traverse the AST and record the linked list of nodes to be unparsed as tokens and from the AST.
  // So that we can query next and last statements and determine if they were unparsed from the token 
  // stream or the AST.  Not clear if the edges of token-stream/AST unparsing should be unparsed from the 
  // token stream leading trailing token information or from the AST using the attached CPP info.
   }

FrontierDetectionForTokenStreamMappingAttribute::
FrontierDetectionForTokenStreamMappingAttribute(SgNode* /*n*/, std::string name, std::string options)
   : name(name), options(options)
   {
   }

FrontierDetectionForTokenStreamMappingAttribute::
FrontierDetectionForTokenStreamMappingAttribute(const FrontierDetectionForTokenStreamMappingAttribute &)
   {
     printf ("In FrontierDetectionForTokenStreamMappingAttribute copy constructor! \n");
     ROSE_ABORT();
   }

string
FrontierDetectionForTokenStreamMappingAttribute::additionalNodeOptions()
   {
     return options;
   }


vector<AstAttribute::AttributeEdgeInfo> 
FrontierDetectionForTokenStreamMappingAttribute::additionalEdgeInfo()
   {
     return vector<AstAttribute::AttributeEdgeInfo>();
   }


vector<AstAttribute::AttributeNodeInfo> 
FrontierDetectionForTokenStreamMappingAttribute::additionalNodeInfo()
   {
     return vector<AstAttribute::AttributeNodeInfo>();
   }

AstAttribute*
FrontierDetectionForTokenStreamMappingAttribute::copy() const
   {
  // Support for the copying of AST and associated attributes on each IR node (required for attributes 
  // derived from AstAttribute, else just the base class AstAttribute will be copied).
     printf ("Warning: FrontierDetectionForTokenStreamMappingAttribute::copy(): not implemented! \n");
     return nullptr;
   }

// DQ (11/14/2017): This addition is not portable, should not be specified outside of the class definition, 
// and fails for C++11 mode on the GNU 4.8.5 compiler and the llvm (some version that Craig used).
AstAttribute::OwnershipPolicy
FrontierDetectionForTokenStreamMappingAttribute::getOwnershipPolicy() const // override
   {
     return CONTAINER_OWNERSHIP;
   }

FrontierNode::FrontierNode(SgStatement* node, bool unparseUsingTokenStream, bool unparseFromTheAST)
   : node(node), 
     unparseUsingTokenStream(unparseUsingTokenStream), 
     unparseFromTheAST(unparseFromTheAST),
     redundant_token_subsequence(false)
   {
  // Enforce specific constraints.
     ASSERT_not_null(node);
     ASSERT_require( (unparseUsingTokenStream == true && unparseFromTheAST == false) || (unparseUsingTokenStream == false && unparseFromTheAST == true) || (unparseUsingTokenStream == false && unparseFromTheAST == false));
     ASSERT_require(redundant_token_subsequence == false);

  // DQ (5/16/2021): Either one of the other of these should be true (we must unparse from either the AST or the token stream).
     ASSERT_require((unparseUsingTokenStream == true || unparseFromTheAST == true));
   }

void
FrontierDetectionForTokenStreamMapping::addFrontierNode (SgStatement* statement, FrontierNode* frontierNode )
   {
     int physical_file_id = generate_physical_file_id(statement);
     ASSERT_require(physical_file_id >= 0);
     ASSERT_require(frontierNode->node == statement);

     if (frontierNodes.find(physical_file_id) == frontierNodes.end())
        {
          frontierNodes[physical_file_id] = new map<SgStatement*,FrontierNode*>();
        }

     ASSERT_require(frontierNodes.find(physical_file_id) != frontierNodes.end());

     if (frontierNodes[physical_file_id]->find(statement) == frontierNodes[physical_file_id]->end())
        {
          std::map<SgStatement*,FrontierNode*>* frontierMap = frontierNodes[physical_file_id];
          frontierMap->insert(std::pair<SgStatement*,FrontierNode*>(statement,frontierNode));
        }

     ASSERT_require(frontierNodes[physical_file_id]->find(statement) != frontierNodes[physical_file_id]->end());
   }

FrontierNode*
FrontierDetectionForTokenStreamMapping::getFrontierNode(SgStatement* statement)
   {
     ASSERT_not_null(statement);
     int physical_file_id = generate_physical_file_id(statement);

  // Make sure that the std::map<SgStatement*,FrontierNode*> is available in the frontierNodes.
     ASSERT_require(frontierNodes.find(physical_file_id) != frontierNodes.end());

  // Make sure that the std::map<SgStatement*,FrontierNode*> has a valid entry for statement.
     ASSERT_require(frontierNodes[physical_file_id]->find(statement) != frontierNodes[physical_file_id]->end());

     std::map<SgStatement*,FrontierNode*>* frontierMap = frontierNodes[physical_file_id];
     ASSERT_not_null(frontierMap);

     FrontierNode* frontierNode = frontierMap->operator[](statement);
     return frontierNode;
   }

void
FrontierDetectionForTokenStreamMapping::outputFrontierNodes()
   {
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

               ASSERT_not_null(statement);
               ASSERT_not_null(frontierNode);

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
