These are notes to support the testing of the AST File I/O


Static data members in the AST have to be handled carefully
so that they can be merged. These include:
   1) static SgFunctionTypeTable* SgNode::p_globalFunctionTypeTable
   2) static std::map<int, std::string> Sg_File_Info::p_fileidtoname_map
   3) static std::map<std::string, int> Sg_File_Info::p_nametofileid_map

