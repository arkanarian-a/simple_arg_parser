// Copyright 2025 Arkanarian

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

#include "hpp/simple_arg_parser.hpp"

namespace SimpleArgParser
{
    Parser::Parser(std::initializer_list<Option> options_il, ParsingPolicy parsing_policy)
    :   options_(options_il)
    ,   option_search_table_(2 * options_.size())
    ,   parsing_policy_(parsing_policy)
    {
        for (auto options_iter{options_.begin()}; options_iter != options_.end(); ++options_iter)
        {
            options_iter->link_to_(this);

            option_search_table_[options_iter->attributes_.key] = &*options_iter;

            if (options_iter->attributes_.alias_key.has_value())
            {
                option_search_table_[options_iter->attributes_.alias_key.value()] = &*options_iter;
            }
        }
    }

    const Option& Parser::operator[](std::string_view option_key) const
    {
        return *get_option_(option_key);
    }

    Option& Parser::operator[](std::string_view option_key)
    {
        return *get_option_(option_key);
    }

    bool Parser::has_option(std::string_view option_key) const
    {
        return option_search_table_.contains(option_key);
    }

    int Parser::parse(int argc, const char* argv[])
    {
        try
        {
            if (argc < 2) return 0;

            Option::SubrangeOfArgV_ subrange_of_argv{argv + 1, argv + argc};

            std::size_t arg_parsed{0};

            while (!subrange_of_argv.empty())
            {
                auto [option_key, option_ptr] = accept_next_option_(subrange_of_argv);

                if (!option_ptr) continue;

                arg_parsed+=option_ptr->parse_option_argument_(subrange_of_argv);
            }

            return arg_parsed; // <-- return the number of args consumed from subrange_of_argv, including option_key
        }
        catch (const OptionAccessException::UndeclaredOptionOrWrongOptionKey& oae)
        {
            throw ParserException::ParsingPolicyViolation(oae.undefined_option_key, std::source_location::current());
        }
    }

    std::ostream& Parser::output(std::ostream& os) const
    {
        for (std::size_t option_count{options_.size()}; const auto& option : options_)
        {
            if (option.has_value())
                os << option << (--option_count ? " " : "");
        }

        return os;
    }

    std::istream& Parser::input(std::istream& is)
    {
        while (is)
        {
            std::string option_key;

            is >> option_key;

            auto* option_ptr{get_option_(option_key)};

            if (!option_ptr) continue;

            is >> *option_ptr;
        }

        return is;
    }


    Option* Parser::get_option_(std::string_view option_key)
    {
        try
        {
            return option_search_table_.at(option_key);
        }
        catch (const std::out_of_range& e)
        {
            if (parsing_policy_ == ParsingPolicy::ForbidUndeclaredOptions)
                throw OptionAccessException::UndeclaredOptionOrWrongOptionKey{option_key, std::source_location::current()};

            return nullptr;
        }
    }

    const Option* Parser::get_option_(std::string_view option_key) const
    {
        return const_cast<Parser*>(this)->get_option_(option_key);
    }

    std::tuple<std::string_view, Option*> Parser::accept_next_option_(Option::SubrangeOfArgV_& subrange_of_argv)
    {
        if (subrange_of_argv.empty()) return { "", nullptr };

        std::string_view option_key{*subrange_of_argv.begin()};

        auto* option_ptr{get_option_(option_key)};

        subrange_of_argv.advance(1);

        return {option_key, option_ptr};
    }

    std::ostream& operator<<(std::ostream& os, const Parser& parser)
    {
        return parser.output(os);
    }

    std::istream& operator>>(std::istream& is, Parser& parser)
    {
        return parser.input(is);
    }
}
