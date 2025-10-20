#ifndef SIMPLE_ARG_PARSER_SPEC_VALUE_TRAITS_HPP
#define SIMPLE_ARG_PARSER_SPEC_VALUE_TRAITS_HPP

// This file contains ValueTraits<T> specializations for T substituted as SimpleArgParser::SwitchState, bool, std::string,
// and std::chrono::system_clock::time_point types.
//
// Include this file into you code, only if you need these specializations. Alternatively, you may write your own
// spetializations for these types. Also, you may redefine some of the default specializations in this file by
// predefining appropriate macros:
//
// SIMPLE_ARG_PARSER_DISABLE_VALUE_TRAITS_SWITCHSTATE_SPEC - to disable default ValueTraits<SwitchState> specialization;
// SIMPLE_ARG_PARSER_DISABLE_VALUE_TRAITS_BOOL_SPEC - to disable default ValueTraits<bool> specialization;
// SIMPLE_ARG_PARSER_DISABLE_VALUE_TRAITS_STD_STRING_SPEC - to disable default ValueTraits<std::string> specialization;
// SIMPLE_ARG_PARSER_DISABLE_VALUE_TRAITS_STD__TIME_POINT_SPEC - to disable default
//                                                               ValueTraits<std::chrono::system_clock::time_point>
//                                                               specialization.
//                                                               (NOTE double undescore between 'STD' and 'TIME' inside
//                                                               the macro name, please, it's intended!)
// NOTE: add a comment '// IWYU pragma: keep' after the #include statement to suppress the CLang warning, which is
//       redundantly paranoic for this case.
//       Resulting line should be like this:
//       #include "simple_arg_parser_spec_value_traits.hpp" // IWYU pragma: keep

#include <iomanip>
#include <chrono>
#include "simple_arg_parser_compiler_fine_tunes.hpp"
#include "simple_arg_parser_iostream_handlers.hpp"
#include "simple_arg_parser_switch_state.hpp"

namespace SimpleArgParser
{
#ifndef SIMPLE_ARG_PARSER_DISABLE_VALUE_TRAITS_SWITCHSTATE_SPEC
    template <>
    struct ValueTraits<SwitchState>
    // For SwitchState option value specialized output only supported ('cause input has no sense).
    {
        std::optional<std::string> output(std::ostream& os, SwitchState state) const
        {
            os << (state == SwitchState::Specified ? "SwitchState::Specified" : "SwitchState::Omitted");

            return std::nullopt;
        }
    };
#endif

#ifndef SIMPLE_ARG_PARSER_DISABLE_VALUE_TRAITS_BOOL_SPEC
    template <>
    struct ValueTraits<bool>: public TypeIndependentValueTraits
    // For bool option value traits use std::boolalpha representation for output
    // and input.
    {
        std::optional<std::string> output(std::ostream &os, bool value) const {
            os << std::boolalpha << value << std::noboolalpha;

            return std::nullopt;
        }

        std::optional<std::string> input(std::istream &is, bool& value) const {
            is >> std::boolalpha >> value >> std::noboolalpha;

            if (is.fail()) {
                is.clear();

                return "Failed to read std::boolalpha value representation from "
                       "std::istream!";
            }

            return std::nullopt;
        }
    };
#endif

#ifndef SIMPLE_ARG_PARSER_DISABLE_VALUE_TRAITS_STD_STRING_SPEC
    template <>
    struct ValueTraits<std::string>: public TypeIndependentValueTraits
    // For std::string option value traits contain a list of string literal quote marks.
    {
        std::optional<std::string> output(std::ostream& os, const std::string& value) const
        {
            os << std::quoted(value, quote_marks[0]);

            return std::nullopt; // Always successful
        }

        std::optional<std::string> input(std::istream& is, std::string& s) const
        {
            char next_char = is.peek();

            if (quote_marks.find(next_char) != std::string::npos)
                is >> std::quoted(s, next_char);
            else
                is >> s;

            return std::nullopt; // Always successful
        }

        std::string quote_marks{"'\""};
    };
#endif

#ifndef SIMPLE_ARG_PARSER_DISABLE_VALUE_TRAITS_STD__TIME_POINT_SPEC
    template <>
    struct ValueTraits<std::chrono::system_clock::time_point>: public TypeIndependentValueTraits
    // For std::chrono::system_clock::time_point option value traits contain ...
    {
        std::optional<std::string> output(std::ostream& os, const std::chrono::system_clock::time_point& tp) const
        {
            os << runtime_format(output_formatter, tp);

            return std::nullopt; // Always successful
        }

        std::optional<std::string> input(std::istream& is, std::chrono::system_clock::time_point& tp) const
        {
            is >> std::chrono::parse(parsing_formatter, tp);

            return std::nullopt; // Always successful
        }

        std::string parsing_formatter{"%F %T"}; // ... format string for parsing a value of the type
        std::string output_formatter{"'{0:%F} {0:%T}'"}; // ... format string for outputting a value of the type.
    };
#endif
}

#endif // SIMPLE_ARG_PARSER_SPEC_VALUE_TRAITS_HPP
