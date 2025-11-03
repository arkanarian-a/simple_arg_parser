// Copyright 2025 arkanarian-a

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is furnished
// to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
// FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef SIMPLE_ARG_PARSER_HPP
#define SIMPLE_ARG_PARSER_HPP

#include <unordered_map>
#include <type_traits>
#include "simple_arg_parser_option.hpp"

using namespace std::literals::string_view_literals;


namespace SimpleArgParser
{
    enum class ParsingPolicy: unsigned int
    {
        SkipUndeclaredOptions = 0   // Default parsing policy - ignore undefined option keys when parsing
    ,   ForbidUndeclaredOptions = 1 // Throw exception if unknown option key met when parsing
    };

    inline constexpr ParsingPolicy operator|(ParsingPolicy lhs, ParsingPolicy rhs)
    // For possible future use in case if ParsingPolicy has extended
    {
        return static_cast<ParsingPolicy>(static_cast<std::underlying_type_t<ParsingPolicy>>(lhs) | static_cast<std::underlying_type_t<ParsingPolicy>>(rhs));
    }

    inline constexpr ParsingPolicy& operator|=(ParsingPolicy& lhs, ParsingPolicy rhs)
    // For possible future use in case if ParsingPolicy has extended
    {
        return lhs = lhs | rhs;
    }

    class Parser
    // The main class for argument parsing.
    // It's initialized with initialization list containing option definitions (objects of Option class).
    // Then argument parsing is performed by calling the method 'int parse(int, const char*[])'.
    {
    public:

        using Options = std::vector<Option>;
        using OptionSearchTable = std::unordered_map<std::string_view, Option*>;

        Parser() = delete;
        Parser(const Parser&) = delete;
        Parser(Parser&&) = delete;

        Parser& operator=(Parser&&) = delete;
        Parser& operator=(const Parser&) = delete;

        Parser(std::initializer_list<Option>, ParsingPolicy = ParsingPolicy::SkipUndeclaredOptions);

        // Option accessors by its key (in its main or short form)
        const Option& operator[](std::string_view) const;
        Option& operator[](std::string_view);

        // Verify an option definition presense (by option key)
        bool has_option(std::string_view) const;

        // Parse arguments passed in command line
        int parse(int, const char*[]);

        std::ostream& output(std::ostream&) const;
        std::istream& input(std::istream&);

    private:

        // Internal exception-free option accessors
        const Option* get_option_(std::string_view) const;
        Option* get_option_(std::string_view);

        // Accept next option key provided with SubrangeOfArgV_ object and get the pointer to option by it
        std::tuple<std::string_view, Option*> accept_next_option_(Option::SubrangeOfArgV_&);

        Options             options_;               // Options container
        OptionSearchTable   option_search_table_;   // An index for searching an option by its key
        ParsingPolicy       parsing_policy_;        // See ParsingPolicy enum class definition
    };

    std::ostream& operator<<(std::ostream&, const Parser&);
    std::istream& operator>>(std::istream&, Parser&);
}

#endif // SIMPLE_ARG_PARSER_HPP
