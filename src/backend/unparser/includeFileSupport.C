
#include "sage3basic.h"

#include "includeFileSupport.h"

using namespace std;


IncludeFileSupport::InheritedAttribute::InheritedAttribute ()
   {
     path_prefix = "";
   }

IncludeFileSupport::InheritedAttribute::InheritedAttribute ( const IncludeFileSupport::InheritedAttribute & X)
   {
     path_prefix = X.path_prefix;
   }


IncludeFileSupport::SynthesizedAttribute::SynthesizedAttribute ()
   {
     path_prefix = "";
     include_file = NULL;
   }

IncludeFileSupport::SynthesizedAttribute::SynthesizedAttribute (SgIncludeFile* input_include_file)
   {
     ASSERT_not_null(input_include_file);

     path_prefix = "";
     include_file = input_include_file;
   }

IncludeFileSupport::SynthesizedAttribute::SynthesizedAttribute ( const IncludeFileSupport::SynthesizedAttribute & X)
   {
     path_prefix  = X.path_prefix;
     include_file = X.include_file;
   }


IncludeFileSupport::InheritedAttribute 
IncludeFileSupport::PrefixTraversal::evaluateInheritedAttribute ( SgNode* node, IncludeFileSupport::InheritedAttribute inheritedAttribute )
   {
#if 0
     printf ("In evaluateInheritedAttribute(): node = %p = %s \n",node,node->class_name().c_str());
#endif
  // IncludeFileSupport::InheritedAttribute ih;
     IncludeFileSupport::InheritedAttribute ih(inheritedAttribute);

     SgIncludeFile* includeFile = isSgIncludeFile(node);
     if (includeFile != NULL)
        {
#if 0
          printf ("include file: filename                = %s \n",includeFile->get_filename().str());
          printf ("   --- directory_prefix               = %s \n",includeFile->get_directory_prefix().str());
          printf ("   --- ih.path_prefix                 = %s \n",ih.path_prefix.c_str());
          printf ("   --- isSystemInclude                = %s \n",includeFile->get_isSystemInclude() ? "true" : "false");
          printf ("   --- isPreinclude                   = %s \n",includeFile->get_isPreinclude() ? "true" : "false");
       // printf ("   --- isHeaderFile                   = %s \n",includeFile->get_isHeaderFile() ? "true" : "false");
          printf ("   --- name_used_in_include_directive = %s \n",includeFile->get_name_used_in_include_directive().str());
#endif

       // ROSE_ASSERT(includeFile->get_directory_prefix().is_null() == false);
          if (includeFile->get_directory_prefix() != ".")
             {
            // ih.path_prefix += includeFile->get_directory_prefix();
               ih.path_prefix = inheritedAttribute.path_prefix + includeFile->get_directory_prefix();
#if 0
               printf ("   --- Modified: ih.path_prefix = %s \n",ih.path_prefix.c_str());
#endif
             }
        }

     return ih;
   }


IncludeFileSupport::SynthesizedAttribute 
IncludeFileSupport::PrefixTraversal::evaluateSynthesizedAttribute ( SgNode* node, IncludeFileSupport::InheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
#if 0
     printf ("In evaluateSynthesizedAttribute(): node = %p = %s \n",node,node->class_name().c_str());
#endif

     SgIncludeFile* includeFile = isSgIncludeFile(node);
     ASSERT_not_null(includeFile);

     SynthesizedAttribute syn_attribute (includeFile);

     if (includeFile != NULL)
        {
#if 0
          printf ("include file: filename = %s \n",includeFile->get_filename().str());
#endif
       // DQ (11/17/2018): the default setting for the rose_edg_required_macros_and_functions.h is true, since it is not inlcuded in the children.
          bool unparseAllChildren = true;

       // Loop over the children.
          for (size_t i = 0; i < synthesizedAttributeList.size(); i++)
             {
               SgIncludeFile* child_include_file = synthesizedAttributeList[i].include_file;
               ASSERT_not_null(child_include_file);
#if 0
               printf ("synthesizedAttributeList[%zu].include_file: filename = %s \n",i,child_include_file->get_filename().str());
#endif
            // if (child_include_file->get_will_be_unparsed() == false)
               if (child_include_file->get_will_be_unparsed() == false && child_include_file->get_isPreinclude() == false)
                  {
                 // DQ (11/17/2018): If one of the children will not be unparsed, then we need to provide a path to the 
                 // original header file so that it can be found at compile time.
                    unparseAllChildren = false;
                  }
             }
#if 0
          printf ("unparseAllChildren = %s \n",unparseAllChildren ? "true" : "false");
#endif
          if (unparseAllChildren == false)
             {
               for (size_t i = 0; i < synthesizedAttributeList.size(); i++)
                  {
                    SgIncludeFile* child_include_file = synthesizedAttributeList[i].include_file;
                    if (child_include_file->get_will_be_unparsed() == false && child_include_file->get_isPreinclude() == false)
                       {
#if 0
                         printf ("   --- setting requires_explict_path_for_unparsed_headers == true \n");
#endif
                      // child_include_file->set_requires_explict_path_for_unparsed_headers(true);
                         if (child_include_file->get_isSystemInclude() == false)
                            {
                              child_include_file->set_requires_explict_path_for_unparsed_headers(true);
                            }
                           else
                            {
#if 0
                              printf ("Ignore paths to system included files: synthesizedAttributeList[%zu].include_file: filename = %s \n",i,child_include_file->get_filename().str());
#endif
                            }
                       }
                  }
             }

       // syn_attribute.path_prefix = "set_in_evaluateSynthesizedAttribute";
          syn_attribute.path_prefix = inheritedAttribute.path_prefix;

          for (size_t i = 0; i < synthesizedAttributeList.size(); i++)
             {
               SgIncludeFile* child_include_file = synthesizedAttributeList[i].include_file;
               ASSERT_not_null(child_include_file);
#if 0
               printf ("(2nd pass) synthesizedAttributeList[%zu].include_file: filename = %s \n",i,child_include_file->get_filename().str());
               printf ("   --- child_include_file->get_requires_explict_path_for_unparsed_headers() = %s \n",child_include_file->get_requires_explict_path_for_unparsed_headers() ? "true" : "false");
#endif
               if (child_include_file->get_requires_explict_path_for_unparsed_headers() == true)
                  {
                 // Add a path to the added_include_path_set.
                 // string path = includeFile->get_filename() + "___" + inheritedAttribute.path_prefix;
                 // string path = XXX::xxx(child_include_file->get_filename());
                    string path = Rose::getPathFromFileName(child_include_file->get_filename());

                 // if (syn_attribute.added_include_path_set.find(path) != syn_attribute.added_include_path_set.end())
                    if (added_include_path_set.find(path) == added_include_path_set.end())
                       {
#if 0
                         printf ("Adding path to added_include_path_set: path = %s \n",path.c_str());
#endif
                      // syn_attribute.added_include_path_set.insert(path);

                         added_include_path_set.insert(path);
#if 0
                         if (child_include_file->get_isSystemInclude() == false)
                            {
                           // child_include_file->set_requires_explict_path_for_unparsed_headers(true);
                              added_include_path_set.insert(path);
                            }
                           else
                            {
                              printf ("Ignore paths to system included files: synthesizedAttributeList[%zu].include_file: filename = %s \n",i,child_include_file->get_filename().str());
                            }
#endif
#if 0
                         printf ("   --- added_include_path_set.size() = %zu \n",added_include_path_set.size());
#endif
                       }
                  }
             }

#if 0
       // printf ("added_include_path_set.size() = %zu \n",syn_attribute.added_include_path_set.size());
          printf ("added_include_path_set.size() = %zu \n",added_include_path_set.size());
#endif

       // for (size_t i = 0; i < syn_attribute.added_include_path_set.size(); i++)
       // std::set<std::string>::iterator i = syn_attribute.added_include_path_set.begin();
          std::set<std::string>::iterator i = added_include_path_set.begin();
          int count = 0;
       // while (i != syn_attribute.added_include_path_set.end())
          while (i != added_include_path_set.end())
             {
#if 0
               printf ("   --- added_include_path_set[%d] = %s \n",count,i->c_str());
#endif
               i++;
               count++;
             }

        }

     return syn_attribute;
   }


// void IncludeFileSupport::headerFilePrefix ( SgIncludeFile* includeFile )
std::set<std::string> 
IncludeFileSupport::headerFilePrefix ( SgIncludeFile* includeFile )
   {
#if 0
     printf ("In IncludeFileSupport::headerFilePrefix(): includeFile filename = %s \n",includeFile->get_filename().str());
#endif

  // class PrefixTraversal : public AstTopDownBottomUpProcessing<SageInterface::InheritedAttribute,SageInterface::SythesizedAttribute>

     InheritedAttribute ih;

  // Now buid the traveral object and call the traversal (preorder) on the function definition.
     IncludeFileSupport::PrefixTraversal traversal;

  // IncludeFileSupport::SynthesizedAttribute si = traversal.traverse(includeFile, inheritedAttribute);
#if 1
     traversal.traverse(includeFile, ih);

     std::set<std::string> added_include_path_set = traversal.added_include_path_set;

#else
     SynthesizedAttribute = traversal.traverse(includeFile, ih);
#endif

  // printf ("si.path_prefix = %s \n",si.path_prefix.c_str());

     return added_include_path_set;
   }


