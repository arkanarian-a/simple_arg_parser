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
