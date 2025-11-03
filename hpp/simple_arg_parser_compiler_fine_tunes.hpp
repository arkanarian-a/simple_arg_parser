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

#ifndef SIMPLE_ARG_PARSER_COMPILER_FINE_TUNES_HPP
#define SIMPLE_ARG_PARSER_COMPILER_FINE_TUNES_HPP

#include <string>
#include <format>

namespace SimpleArgParser
{
#if __cplusplus > 202302L

    template<typename... Args>
    std::string runtime_format(std::string_view format_str, Args&&... args)
    {
        return std::format(std::runtime_format(format_str), args...);
    }

#else

    template<typename... Args>
    std::string runtime_format(std::string_view format_str, Args&&... args)
    {
        return std::vformat(format_str, std::make_format_args(args...));
    }

#endif
}

#endif // SIMPLE_ARG_PARSER_COMPILER_FINE_TUNES_HPP
