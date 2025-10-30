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

#ifndef SIMPLE_ARG_PARSER_OPTION_HPP
#define SIMPLE_ARG_PARSER_OPTION_HPP

#include <sstream>
#include <iterator>
#include <initializer_list>
#include <memory>

#include <assert.h>

#include "simple_arg_parser_switch_state.hpp"
#include "simple_arg_parser_scalar_value.hpp"
#include "simple_arg_parser_vectored_value.hpp"


namespace SimpleArgParser
{
// ------------
// Declarations
// ------------
    struct OptionAttributes
    {
        std::string_view                key{};
        std::optional<std::string_view> alias_key{};
        std::optional<std::string_view> description{};
    };

    class Parser;

    class Option
    {
    public:

        template <typename T>
        friend T& Internals_::get_value(Option*);

        using enum SwitchState;
        using Quantifier = SimpleArgParser::Quantifier;

        static constinit std::size_t UNLIMITED_MAX_ITEMS;

        Option() = delete;
        Option(Option&&) = default;
        Option(const Option&);

        template <typename T>
        Option
        // This constructor is to construct an option containing a vector of values of type T even if it's empty one
        (
            OptionAttributes&&
        ,   std::vector<T>&& = {}
        ,   Quantifier&& = { 0, UNLIMITED_MAX_ITEMS }
        ,   ValueTraits<T>&& = {}
        ,   ValueInputter<T>&& = nullptr
        ,   ValueOutputter<T>&& = nullptr
        );

        template <typename T>
        Option
        // This constructor is to construct a vectored option from initializer list
        (
            OptionAttributes&&
        ,   std::initializer_list<T>
        ,   Quantifier&& = { 0, UNLIMITED_MAX_ITEMS }
        ,   ValueTraits<T>&& = {}
        ,   ValueInputter<T>&& = nullptr
        ,   ValueOutputter<T>&& = nullptr
        );

        template <typename T>
        Option
        // This constructor is for a scalar (containing only one value instead of a sequence of values) valued option
        (
            OptionAttributes&&
        ,   T&&
        ,   ValueTraits<T>&& = {}
        ,   ValueInputter<T>&& = nullptr
        ,   ValueOutputter<T>&& = nullptr
        );

        template <typename T>
        Option
        // This constructor is for a pointer to vectored option value
        (
            OptionAttributes&&
        ,   std::vector<T>*
        ,   Quantifier&& = { 0, UNLIMITED_MAX_ITEMS }
        ,   ValueTraits<T>&& = {}
        ,   ValueInputter<T>&& = nullptr
        ,   ValueOutputter<T>&& = nullptr
        );

        template <typename T>
        Option
        // This constructor is for a pointer to scalar option value
        (
            OptionAttributes&&
        ,   T*
        ,   ValueTraits<T>&& = {}
        ,   ValueInputter<T>&& = nullptr
        ,   ValueOutputter<T>&& = nullptr
        );

        // Special case is for switch option:
        Option(OptionAttributes&&, SwitchState&&);
        Option(OptionAttributes&&, SwitchState*);

        Option& operator=(Option&&) = delete;
        Option& operator=(const Option&) = delete;

        const std::string_view& get_key() const;

        bool has_value() const;

        template <typename T>
        const T& get_value() const;

        template <typename T>
        T& copy_value(T& destination) const; // returns the reference to destination

        std::ostream& output(std::ostream&) const;
        std::istream& input(std::istream&);

    private:

        friend class Parser;

        using SubrangeOfArgV_ = std::ranges::subrange<const char**, const char**>;
        // Subrange for iterating the sequence of arguments passed.

        void link_to_(Parser*);
        // Make a link to the Parser which is this option defined for.
        bool option_key_is_defined_(std::string_view) const;
        // Check whether the option with specifid key is defined in the Parser (which is linked to this option).

        template <typename T>
        T& get_value_();
        // Implementation of option's value of its type T getter

        int parse_option_argument_(SubrangeOfArgV_&);
        // Parse next argument value from a subrange of arguments.

        int set_switch_option_on_(SubrangeOfArgV_&);
        // If the option value type is SwitchOptionStatus and its key is specified in argument list.
        // then it's parsed value Option::Specified should be set.

        template <typename T>
        int parse_argument_(SubrangeOfArgV_& subrange_of_argv);
        // Type-dependent option value parser implementation.
        // Implements one of two different algorithms, depending on wether the option has vectored or scalar value.

        using ArgParser_ = int(Option::*)(SubrangeOfArgV_&);
        // A pointer to parsing method for an option.

        OptionAttributes                        attributes_;
        std::any                                value_;
        ArgParser_                              arg_parser_;
        std::shared_ptr<Internals_::IOptionIO>  io_handler_;
        Parser*                                 parser_ptr_{nullptr};
    };


// -----------
// Definitions
// -----------
    template <typename T>
    Option::Option
    (
        OptionAttributes&& attributes
    ,   std::vector<T>&& value
    ,   Quantifier&& arg_quantifier
    ,   ValueTraits<T>&& value_traits
    ,   ValueInputter<T>&& value_inputter
    ,   ValueOutputter<T>&& value_outputter
    )
    :   attributes_(attributes)
    ,   value_(Internals_::VectoredValue<T>(std::move(value), std::move(arg_quantifier)))
    ,   arg_parser_(&Option::parse_argument_<std::vector<T>>)
    ,   io_handler_
        (
            std::make_shared<Internals_::OptionIOImpl<T, true>>
            (
                std::move(value_outputter ? value_outputter : default_value_outputter<T>)
            ,   std::move(value_inputter ? value_inputter : default_value_inputter<T>)
            ,   std::move(value_traits)
            )
        )
    {}

    template <typename T>
    Option::Option
    (
        OptionAttributes&& attributes
    ,   std::initializer_list<T> init_values
    ,   Quantifier&& arg_quantifier
    ,   ValueTraits<T>&& value_traits
    ,   ValueInputter<T>&& value_inputter
    ,   ValueOutputter<T>&& value_outputter
    )
    :   attributes_(attributes)
    ,   value_(Internals_::VectoredValue<T>(init_values, std::move(arg_quantifier)))
    ,   arg_parser_(&Option::parse_argument_<std::vector<T>>)
    ,   io_handler_
        (
            std::make_shared<Internals_::OptionIOImpl<T, true>>
            (
                std::move(value_outputter ? value_outputter : default_value_outputter<T>)
            ,   std::move(value_inputter ? value_inputter : default_value_inputter<T>)
            ,   std::move(value_traits)
            )
        )
    {}

    template <typename T>
    Option::Option
    (
        OptionAttributes&& attributes
    ,   T&& init_value
    ,   ValueTraits<T>&& value_traits
    ,   ValueInputter<T>&& value_inputter
    ,   ValueOutputter<T>&& value_outputter
    )
    :   attributes_(attributes)
    ,   value_(Internals_::ScalarValue<T>(std::move(init_value)))
    ,   arg_parser_(&Option::parse_argument_<T>)
    ,   io_handler_
        (
            std::make_shared<Internals_::OptionIOImpl<T, false>>
            (
                (value_outputter ? value_outputter : default_value_outputter<T>)
            ,   (value_inputter ? value_inputter : default_value_inputter<T>)
            ,   std::move(value_traits)
            )
        )
    {}

    template <typename T>
    Option::Option
    (
        OptionAttributes&& attributes
    ,   std::vector<T>* value_ptr
    ,   Quantifier&& arg_quantifier
    ,   ValueTraits<T>&& value_traits
    ,   ValueInputter<T>&& value_inputter
    ,   ValueOutputter<T>&& value_outputter
    )
    :   attributes_(attributes)
    ,   value_(Internals_::VectoredValue<T>(value_ptr, std::move(arg_quantifier)))
    ,   arg_parser_(&Option::parse_argument_<std::vector<T>>)
    ,   io_handler_
        (
            std::make_shared<Internals_::OptionIOImpl<T, true>>
            (
                std::move(value_outputter ? value_outputter : default_value_outputter<T>)
            ,   std::move(value_inputter ? value_inputter : default_value_inputter<T>)
            ,   std::move(value_traits)
            )
        )
    {}

    template <typename T>
    Option::Option
    (
        OptionAttributes&& attributes
    ,   T* value_ptr
    ,   ValueTraits<T>&& value_traits
    ,   ValueInputter<T>&& value_inputter
    ,   ValueOutputter<T>&& value_outputter
    )
    :   attributes_(attributes)
    ,   value_(Internals_::ScalarValue<T>(value_ptr))
    ,   arg_parser_(&Option::parse_argument_<T>)
    ,   io_handler_
        (
            std::make_shared<Internals_::OptionIOImpl<T, false>>
            (
                std::move(value_outputter ? value_outputter : default_value_outputter<T>)
            ,   std::move(value_inputter ? value_inputter : default_value_inputter<T>)
            ,   std::move(value_traits)
            )
        )
    {}

    template <typename T>
    const T& Option::get_value() const
    {
        return const_cast<Option*>(this)->get_value_<T>();
    }

    template <typename T>
    T& Option::copy_value(T& destination) const
    {
        return destination = get_value<T>();
    }

    template <typename T>
    T& Option::get_value_()
    {
        try
        {
            if constexpr (Internals_::IsVector<T>())
                return std::any_cast<Internals_::VectoredValue<typename T::value_type>&>(value_);
            else
                return std::any_cast<Internals_::ScalarValue<T>&>(value_).get_value();
        }
        catch (const std::bad_any_cast&)
        {
            throw OptionAccessException::AccessingValueTypeMismatch{std::source_location::current()};
        }
    }

    template <typename T>
    int Option::parse_argument_(SubrangeOfArgV_& subrange_of_argv)
    // Type-dependent option value parser implementation.
    // Implements one of two different algorithms, depending on wether the option has vectored or scalar value.
    {
        if constexpr (Internals_::IsVector<T>())
        {
            using ItemType = typename T::value_type;

            auto& value = get_value_<Internals_::VectoredValue<ItemType>>();
            auto& items = value.items();
            auto representation_token_count{(io_handler_->get_value_traits<ItemType>()).representation_token_count};
            std::size_t args_consumed{0}, max_args_to_consume{value.max_items() * representation_token_count};

            items.clear();

            // Iterate the subrange until any next option key met or the args parsed count reaches the max values of the quantifier:
            for
            (
                auto arg{subrange_of_argv.begin()}, arg_items_num{representation_token_count}
            ;   arg != subrange_of_argv.end() && !option_key_is_defined_(*arg) && args_consumed < max_args_to_consume
            ;   arg+=arg_items_num, args_consumed+=arg_items_num
            )
            {
                std::stringstream ss;
                arg_items_num = std::min(representation_token_count, static_cast<std::size_t>(subrange_of_argv.end() - arg));

                std::copy_n(arg, arg_items_num, std::ostream_iterator<std::string>(ss, " "));

                items.resize(args_consumed / representation_token_count + 1);
                io_handler_->input_value(ss, items.at(args_consumed / representation_token_count));
            }

            if (args_consumed < value.min_items())
                throw OptionAccessException::InsufficientNumberOfValueItems(get_key(), args_consumed, value.min_items(), std::source_location::current());

            subrange_of_argv.advance(args_consumed);

            return args_consumed + 1; // <-- return the number of args consumed from subrange_of_argv, including option_key
        }
        else
        {
            if (subrange_of_argv.empty())
                throw OptionParsingException::ScalarOptionValueLost{get_key(), std::source_location::current()};

            std::stringstream ss;
            auto arg_items_num{std::min((io_handler_->get_value_traits<T>()).representation_token_count, subrange_of_argv.size())};

            std::copy_n(subrange_of_argv.begin(), arg_items_num, std::ostream_iterator<std::string>(ss, " "));

            io_handler_->input_value(ss, get_value_<T>());

            subrange_of_argv.advance(arg_items_num);

            return arg_items_num + 1;
        }
    }


    std::ostream& operator<<(std::ostream&, const Option&);
    std::istream& operator>>(std::istream&, Option&);


    template <typename T>
    T& Internals_::get_value(Option* option_ptr)
    {
        return option_ptr->get_value_<T>();
    }
}

#endif // SIMPLE_ARG_PARSER_OPTION_HPP
