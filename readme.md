# Simple Arguments Parser

This library allows to define command line options easily and easily parse their values according to their
types. You may utilize following features:

- define options with values of "scalar" data types, such as **int**, **double**, **std::string**,
**std::chrono::system_clock::time_point** (the latter two types (and any similar ones) are treated as "scalar"
in library context, because they're used as a single value or an item value of "vectored" option);
- define options with values of user-defined types (with some additional customization of stream I/O and library-provided
value traits template; see the *sap_user_type_sample/main.cpp* for details);
- define options contaning a vector of any "scalar" or user-defined type ("vectored" option in the library context);
- define option as owninig the value or keeping a pointer to it (an address of a variable containing the value)
to avoid extra copying;
- customize parsing (inputting) and/or outputting option value to enrich it with user-defined behaviour.
  
  
## Usage instructions

1. Add to your project *simple_arg_parser.so* library and path to it's header files (*hpp/*).
2. Include to your code folowing header files:
   + *simple_arg_parser.hpp* -- the main header file containing **SimpleArgParser::Parser**
   class and indirectly including all necessary helper classes and templates;
   + *simple_arg_parser_spec_value_traits.hpp* -- header file containing library-provided
   specializations of **SimpleArgParser::ValueTraits\<T\>** template for following T's:
     * **SimpleArgParser::SwitchState** (library-specific type representing "switch option value" 
     that is option behaving like a switch: taking value **SimpleArgParser::SwitchState::Specified**
     when its key's specified in command line or **SimpleArgParser::SwitchState::Omitted** otherwise);
     * **bool**;
     * **std::string**;
     * **std::chrono::system_clock::time_point**.
   
>  \*\*\*  ***Hint:*** 
>  > *you may disable any of these specializations defining following appropriate macros*
>  > *before including the header:*
>   
>  > **SIMPLE_ARG_PARSER_DISABLE_VALUE_TRAITS_SWITCH_STATE_SPEC**
>
>  > **SIMPLE_ARG_PARSER_DISABLE_VALUE_TRAITS_BOOL_SPEC**
>
>  > **SIMPLE_ARG_PARSER_DISABLE_VALUE_TRAITS_STD_STRING_SPEC**
>
>  > **SIMPLE_ARG_PARSER_DISABLE_VALUE_TRAITS_STD__TIME_POINT_SPEC**
>  
>  \*\*\*
      
   + *simple_arg_parser_auxiliaries.hpp* -- header file containing library auxiliaries, which
   currently are **lno()** and **qd()** std::ostream manipulataros. The first one outputs
   current source line number, the second outputs a
   value in quotes (see the *sap_quick_start_sample/main.cpp* to understand the usage).
3. Write customization code for your own data types and/or special option value parsing (input)
and output logic (***if needed!***).

The easiest way to start is to look through privided samples:
- *sap_quick_start_sample/main.cpp* -- for using options with integral, floating point or any simple
standard library types, including 'vectored' options with items of that types;
- *sap_user_type_sample/main.cpp* -- for custom code for your own data types and parsing logic.
These samples are commented thoroughly, so, you can catch the idea quickly.

## C++ standard compatibility considerations

The library requires C++20 or higher standard compiler support.

*Actually, it uses even one of C++26's feature: **std::runtime_format()**
functuion, but it's hided inside library's **SimpleArgParser::runtime_format()** analogue, which implementation substituted
depending on compiler support. In case of C++26 it calls **std::runtime_format()**, otherwise implementation via **std::vformat()**
is used. See details in *simple_arg_parser_compiler_fine_tunes.hpp*, if you wondered.*

>  \*\*\*   **Caution:** 
>  >  current library version project and samples of library usage have been tested with GCC compiler only, any other compilers may 
>  >  be used to build them but successful result "out-of-the-box" is not garanteed!
>  
>  \*\*\*

## If you'd like to provide feedback to the library author

Although the library author does not have the capacity to support the code on a commercial basis, and therefore the library 
is provided 'as is', the author is willing to consider any feedback or suggestions for potential development and/or improvement 
of the library. Above all, the author is particularly interested in receiving information about identified bugs. Additionally, 
while the author cannot guarantee a mandatory response, he's open to provide some explanations regarding the usage of the library 
and the design features of its code.

For feedback to the library author and questions regarding its usage, please use the public Telegram channel: ***t.me/arkanarian_support***.

