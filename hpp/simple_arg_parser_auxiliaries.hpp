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

#ifndef SIMPLE_ARG_PARSER_AUXILIARIES_HPP
#define SIMPLE_ARG_PARSER_AUXILIARIES_HPP

#include <iostream>
#include <vector>
#include <algorithm>
#include <source_location>

// Include this file into your code if you want to use following std:ostream manipulators:
// lno - to output to std::ostream current source line number (from std::source_location::current())
// qd  - to output to std::ostream a value in quotes (default or specified)

namespace SimpleArgParser
{
    struct Quotes
    // Helper struct keeping a pair of quotes (or same single quote for left and right side)
    {
        Quotes(std::string_view left_quote = "\"", std::string_view right_quote = "")
        :   left_quote(left_quote)
        ,   right_quote(right_quote.empty() ? left_quote : right_quote)
        {}

        const std::string_view  left_quote;
        const std::string_view  right_quote;
    };

    struct CurrentSourceLineNo
    // Value keeper for lno manipulator. Holds the source_location (containing line number) and quotes enclosing the line number.
    {
        CurrentSourceLineNo(const std::source_location source_location, Quotes&& quotes = { "[", "] " })
        :   source_location(std::move(source_location))
        ,   quotes(std::move(quotes))
        {}

        const std::source_location  source_location;
        const Quotes                quotes;
    };

    CurrentSourceLineNo lno
    (
        std::string_view left_quote = "[",
        std::string_view right_quote = "] ",
        const std::source_location& source_location = std::source_location::current()
    )
    // std::ostream manipulator outputting the current source line number.
    // By default it encloses the value with left quote "[" and right quote "] ".
    {
        return CurrentSourceLineNo(source_location, Quotes(left_quote, right_quote));
    }

    std::ostream& operator<<(std::ostream& os, const CurrentSourceLineNo& current_source_line_no)
    // std::ostream outout operator for lno's value keeper object (of type CurrentSourceLineNo)
    {
        return
            os
            <<  current_source_line_no.quotes.left_quote
            <<  current_source_line_no.source_location.line()
            <<  current_source_line_no.quotes.right_quote
        ;
    }

    template <typename T>
    struct Quoted
    // Value keeper for qd std::ostream manipulator. Keeps the const reference to value and quotes to enclose it.
    {
        Quoted(const T& value, const Quotes& quotes = {})
        :   value(value)
        ,   quotes(quotes)
        {}

        const T&        value;
        const Quotes    quotes;
    };

    template <typename T>
    struct Quoted<std::vector<T>>
    // Partial specialization of template Quoted<T> for std::vector<T>. Keeps the const reference to vector,
    // quots for vector items, delimiter for vector items and quotes for the vector.
    {
        Quoted
        (
            const std::vector<T>&   vector,
            const Quotes&           item_quotes = { "" },
            std::string_view        item_delimiter = ", ",
            const Quotes&           vector_quotes = { "{ ", " }" }
        )
        :   vector(vector)
        ,   item_quotes(item_quotes)
        ,   item_delimiter(item_delimiter)
        ,   vector_quotes(vector_quotes)
        {}

        const std::vector<T>&   vector;
        const Quotes            item_quotes;
        const std::string_view  item_delimiter;
        const Quotes            vector_quotes;
    };

    template <typename T>
    Quoted<T> qd(const T& value, std::string_view left_quote = "\"", std::string_view right_quote = "")
    // std::ostream manipulator outputting a value of type T enclosed with left and right quotes (single quote " by default)
    {
        return Quoted<T>(value, Quotes(left_quote, right_quote));
    }

    template <typename T>
    Quoted<std::vector<T>>
    qd
    (
        const std::vector<T>&   vector,
        std::string_view        item_left_quote = "",
        std::string_view        item_right_quote = "",
        std::string_view        item_delimiter = ", ",
        std::string_view        vector_left_quote = "",
        std::string_view        vector_right_quote = ""
    )
    // std::ostream manipulator outputting a vector of values of type T with each item enclosed with left and right quotes
    // (witout quotes by default), delimited with specified delimiter (", " by default) and left and right quotes surrounding
    // the whole item list (by default - "{ " and " }").
    // To set single quote surrounding for each item specify item_left_quote only.
    // The same way for whole vector value (set vector_left_quote only). Set any value for vector_right_quote leaving the
    // vector_left_quote empty (equal to "") to omit vector quotes on output.
    {
        if (!vector_left_quote.empty())
            return
                Quoted<std::vector<T>>
                (
                    vector,
                    Quotes(item_left_quote, item_right_quote),
                    item_delimiter,
                    Quotes(vector_left_quote, vector_right_quote)
                )
            ;

        if (vector_right_quote.empty())
            return Quoted<std::vector<T>>(vector, Quotes(item_left_quote, item_right_quote), item_delimiter, {"{ ", " }"});
        else
            return Quoted<std::vector<T>>(vector, Quotes(item_left_quote, item_right_quote), item_delimiter, {"", ""});
    }

    template <typename T>
    std::ostream& operator<<(std::ostream& os, const Quoted<T>& out_value)
    // std::ostream output operator for Quoted<T> value keeper.
    {
        return os << out_value.quotes.left_quote << out_value.value << out_value.quotes.right_quote;
    }

    template <typename T>
    std::ostream& operator<<(std::ostream& os, const Quoted<std::vector<T>>& out_vector)
    // std::ostream output operator for Quoted<std::vector<T>> value keeper.
    {
        os << out_vector.vector_quotes.left_quote;

        std::for_each
        (
            out_vector.vector.cbegin(),
            out_vector.vector.cend() - 1,
            [&] (const auto& item)
            {
                os
                <<  qd(item, out_vector.item_quotes.left_quote, out_vector.item_quotes.right_quote)
                <<  out_vector.item_delimiter
                ;
            }
        );

        return
        os
        <<  qd(*(out_vector.vector.cend() - 1), out_vector.item_quotes.left_quote, out_vector.item_quotes.right_quote)
        <<  out_vector.vector_quotes.right_quote
        ;
    }
}

#endif // SIMPLE_ARG_PARSER_AUXILIARIES_HPP
