#ifndef _OPTION_DECLARATION_H
#define _OPTION_DECLARATION_H

 /*! \if documentDevelopmentVersionUsingDoxygen
          \brief This class is part of the specification of transformation options.

          \ingroup transformationSupportLib

          This class is part of a general mechanism to permit the user's application 
          to easily specify options which compile to useless code (no side-effects) 
          but which can be seen by preprocessors built with ROSE.  

          Declarations of options can appear with an option name string and either a number, or additional
          string, or ...
     \endif
 */
class ROSE_DLL_API OptionDeclaration
   {

     public:

       //! This is the name of the option which is used at compile-time to associate the value with the option
          std::string optionName;

      /*! @{
          \name values associated with the option string
          \brief These are the possible values that are associated with the option names in the optionName
       */
          int    valueInteger;
          bool   valueBool;
          double valuedouble;
          std::string valueString;
      /*! @} */

      /*! @{
          \name Constructors to associate strings with values.
          \brief Constructors designed to trigger auto-promotion of constructor arguments

          This mechanism depends upon autopromotion to generate a OptionDeclaration object
          from a string and a value.
       */

          OptionDeclaration( std::string s, int value );
          OptionDeclaration( std::string s, double value );
          OptionDeclaration( std::string s, std::string value );
      /*! @} */

      //! get the input value.
          std::string getValue();
   };

// endif for  _OPTION_DECLARATION_H
#endif
