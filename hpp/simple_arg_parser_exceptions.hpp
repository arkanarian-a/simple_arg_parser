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

#ifndef SIMPLE_ARG_PARSER_EXCEPTIONS_HPP
#define SIMPLE_ARG_PARSER_EXCEPTIONS_HPP

#include <source_location>
#include <string>
#include <exception>
#include <format>
#include <ostream>


namespace SimpleArgParser
{
    // Here are exception definitions used in the lib. The exception purposes are self-documented

    class OptionException: public std::exception
    {
    public:

        OptionException(std::string_view message, const std::source_location sl)
        :   message_(std::format("[{}: {}: {}] {}", sl.file_name(), sl.line(), sl.function_name(), message))
        {}

        const char* what() const noexcept
        {
            return message_.c_str();
        }

        std::ostream& output(std::ostream& os, const std::source_location sl) const
        {
            return os << "[" << sl.file_name() << ": " << sl.line() << "]\n" << message_ << std::endl;
        }

    private:

        std::string message_;
    };

    namespace OptionIOException
    {
        struct ValueInputterFailure: public OptionException
        {
            ValueInputterFailure(std::string_view inputter_message, const std::source_location sl)
            : OptionException(std::format("Value inputter execution failed with message: \"{}\"!", inputter_message), sl)
            {}
        };

        struct ValueOututterFailure: public OptionException
        {
            ValueOututterFailure(std::string_view inputter_message, const std::source_location sl)
            : OptionException(std::format("Value outputter execution failed with message: \"{}\"!", inputter_message), sl)
            {}
        };
    };

    namespace OptionParsingException
    {
        struct ScalarOptionValueLost: public OptionException
        {
            ScalarOptionValueLost(std::string_view option_key, const std::source_location sl)
            : OptionException(std::format("Scalar option '{}' must follow a value when specified in command line!", option_key), sl)
            {}
        };
    }

    namespace OptionAccessException
    {
        struct UndeclaredOptionOrWrongOptionKey: public OptionException
        {
            UndeclaredOptionOrWrongOptionKey(std::string_view option_key, const std::source_location sl)
            :   OptionException(std::format("Attempt to access an option by undefined key '{}'!", option_key), sl)
            ,   undefined_option_key(option_key)
            {}

            const std::string_view undefined_option_key;
        };

        struct AccessingValueTypeMismatch: public OptionException
        {
            AccessingValueTypeMismatch(const std::source_location sl)
            : OptionException("Attempt to get a value of wrong type!", sl)
            {}
        };

        struct InsufficientNumberOfValueItems: public OptionException
        {
            InsufficientNumberOfValueItems(std::string_view option_key, std::size_t args_got, std::size_t args_min, const std::source_location sl)
            :   OptionException
                (
                    std::format
                    (
                        "Insufficient number of arguments specified for vectored option '{}' value! ({} got when minimum {} expected)"
                    ,   option_key
                    ,   args_got
                    ,   args_min
                    )
                ,   sl
                )
            {}
        };

        struct NumberOfItemsSpecifiedExceedsMaximum: public OptionException
        {
            NumberOfItemsSpecifiedExceedsMaximum(std::size_t args_got, std::size_t args_max, const std::source_location sl)
            :   OptionException
                (
                    std::format
                    (
                        "Number of arguments {} specified for a vectored option value exeedes maximum {}!"
                    ,   args_got
                    ,   args_max
                    )
                ,   sl
                )
            {}
        };

        struct SpecifiedNumberOfItemsIsLessThanMinimum: public OptionException
        {
            SpecifiedNumberOfItemsIsLessThanMinimum(std::size_t args_got, std::size_t args_min, const std::source_location sl)
            :   OptionException
                (
                    std::format
                    (
                        "Number of arguments {} specified for a vectored option value is less than minimum {}!"
                    ,   args_got
                    ,   args_min
                    )
                ,   sl
                )
            {}
        };
    }

    namespace ParserException
    {
        struct ParsingPolicyViolation: public OptionException
        {
            ParsingPolicyViolation(std::string_view undeclared_option_key, const std::source_location sl)
            :   OptionException
                (
                    std::format("Undeclared option key '{}' met when ParsingPolicy::ForbidUndeclaredOptions set!", undeclared_option_key)
                ,   sl
                )
            {}
        };
    }

    namespace InternalError
    {
        struct OptionIOTypeMismatch: public OptionException
        {
            OptionIOTypeMismatch(std::string_view cause, const std::source_location sl)
            : OptionException(std::format("Option IO type internal error by cause of: '{}'!", cause), sl)
            {}
        };

        struct OptionObjectIntegrityViolation: public OptionException
        {
            OptionObjectIntegrityViolation(std::string_view cause, const std::source_location sl)
            : OptionException(std::format("Option object integrity violated by cause of: '{}'!", cause), sl)
            {}
        };
    }
}

#endif // SIMPLE_ARG_PARSER_EXCEPTIONS_HPP
