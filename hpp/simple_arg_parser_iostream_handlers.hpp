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

#ifndef SIMPLE_ARG_PARSER_IOSTREAM_HANDLERS_HPP
#define SIMPLE_ARG_PARSER_IOSTREAM_HANDLERS_HPP

#include <iostream>
#include <functional>
#include <any>
#include <optional>
#include "simple_arg_parser_vectored_value.hpp"

namespace SimpleArgParser
{
// ------------
// Declarations
// ------------
    struct TypeIndependentValueTraits
    // Base class for ValueTraits<T>. Contains properties that are independent of T.
    // (Actually, only the count of tokens representing the value in a stream depends on specific representation,
    // not the features of the value type (T). Although these features may determine the form of such representation.
    // Briefly: the same object (and its type) may be represented in different ways.)
    {
        TypeIndependentValueTraits(std::size_t default_representation_token_count = 1)
        :   representation_token_count(default_representation_token_count)
        {}

        std::size_t representation_token_count;  // Number of tokens (words) in a stream to represent the option value
    };

    template <typename T>
    struct ValueTraits: public TypeIndependentValueTraits
    // Structure to define different type-specific traits for an option value.
    // It could be specialized for the type if needed.
    {
        ValueTraits(): TypeIndependentValueTraits()
        {}

        std::optional<std::string> output(std::ostream& os, const T& value) const
        // If specialized, this function MUST return std::nullopt on success or an error description (of std::string) otherwise.
        {
            os << value;

            return std::nullopt;
        }

        std::optional<std::string> input(std::istream& is, T& value) const
        // If specialized, this function MUST return std::nullopt on success or an error description (of std::string) otherwise.
        {
            is >> value;

            return std::nullopt;
        }
    };

    template <typename T>
    std::optional<std::string> default_value_outputter(std::ostream& os, const T& value, const ValueTraits<T>& value_traits = {})
    // This function is used by default to output the value of type T to std::ostream, taking into account value's of type T
    // traits,if them needed. This (default) implementation relies on the method, supplied by traits, but it's possible
    // to implement the output without the traits completly or with custom traits specilization.
    // Both the outputter (default or custom one) and the object of ValueTraits<T> are saved in the Option object when it's
    // initialized. Then the Option object colls the outputter, passing the object of ValueTraits<T> as its third argument.
    // So, the behaviour of option's value outputter is highly customizible, according to specific project's needs.
    // MUST return std::nullopt on success or an error description (of std::string) otherwise.
    {
        return value_traits.output(os, value);
    }

    template <typename T>
    std::optional<std::string> default_value_inputter(std::istream& is, T& value, ValueTraits<T>& value_traits = {})
    // This function is used by default to input the value of type T from std::istream, taking into account type's T traits.
    // It's design and behaviour are symmetric to default_value_outputter. The only important point have to be mensioned:
    // value inputter set when the Option object initialized will be used during parsing the option's value further.
    // MUST return std::nullopt on success or an error description (of std::string) otherwise.
    {
        return value_traits.input(is, value);
    }

    template <typename T>
    using ValueOutputter = std::function<std::optional<std::string>(std::ostream&, const T&, const ValueTraits<T>&)>;
    // This type defines the option value outputter. Any user defined outputter MUST be of this type.

    template <typename T>
    using ValueInputter = std::function<std::optional<std::string>(std::istream&, T&, ValueTraits<T>&)>;
    // This type defines the option value inputter (and parser!). Any user defined inputter MUST be of this type.

    class Option;

    namespace Internals_
    {
        template <typename T>
        T& get_value(Option*);
        // Helper function to inderect the access to option's value from IOptionIO implementation.

        struct IOptionIO
        // Interface incapsulating option (and its value) input and output details.
        // It's used by Option class methods to input (or parse) and output Option's object.
        // CAUTION: constructed object of this type (as part of constructing its descendants)
        //          STAY INCONSISTENT UNTIL EXPLICIT CALL OF link_to method, which links it
        //          to Option object it serves for!
        {
            virtual ~IOptionIO() = default;

            template <typename T>
            std::optional<std::string> input_value(std::istream&, T&);
            // Input value of type T.
            // Returns std::nullopt on success or an error description (of std::string) otherwise.

            template <typename T>
            ValueTraits<T> get_value_traits() const;
            // Get the value traits object.

            void output_option(std::ostream& os) { return output_option_(os); };
            // Output option key and value.

            void input_option_value(std::istream& is) { return input_option_value_(is);  };
            // Input option value (according to its type).

            void link_to(Option* option_ptr) { option_ptr_ = option_ptr; };
            // Link this input/output option handler to the option specified with a pointer
            // This method is needed for Option copy constructor (see the comment there).

        private:

            virtual std::any get_value_inputter_()  const { return nullptr; };
            // Implementation of value inputter getter method.
            // By default it returns nullptr (which meens 'no inputter defined').
            // Must be overriden in derived class to provide correct inputter address.

            virtual std::any get_value_traits_()    const { return nullptr; };
            // Implementation of value traits object getter method
            // By default it returns nullptr (which meens 'no traits object defined').
            // Must be overriden in derived class to provide correct traits object.

            virtual void output_option_(std::ostream&) const = 0;
            // Implementation of method outputting the option (its key and value).
            // Must be overriden in derived class accordingly.

            virtual void input_option_value_(std::istream&) {};
            // Implementation of method inputting option's value.
            // By default it does nothing.
            // Must be overriden in derived class to perform input.

        protected:

            Option* option_ptr_;
            // This member is not initialized when constructing the object!
            // Initializing is done explicitly with link_to method.
        };

        const std::string_view& get_key(Option*);

        template <typename T, bool IS_VECTORED_VALUE>
        class OptionIOImpl: public IOptionIO
        // Implementation of IOptionIO option input/output handler
        {
        public:

            OptionIOImpl(ValueOutputter<T>&& = {}, ValueInputter<T>&& = {}, ValueTraits<T>&& = {});
            ~OptionIOImpl() override = default;

        private:

            std::any get_value_inputter_() const override;
            std::any get_value_traits_() const override;

            void input_value_(std::istream&, T&);
            void output_value_(std::ostream&, const T&) const;

            void output_option_(std::ostream&) const override;
            void input_option_value_(std::istream&) override;

            ValueTraits<T>      value_traits_;
            ValueOutputter<T>   value_outputter_;
            ValueInputter<T>    value_inputter_;
        };


// -----------
// Definitions
// -----------
        template <typename T>
        std::optional<std::string> IOptionIO::input_value(std::istream& is, T& value)
        {
            try
            {
                auto value_traits{std::any_cast<ValueTraits<T>>(get_value_traits_())};
                const auto value_inputter{std::any_cast<ValueInputter<T>>(get_value_inputter_())};

                if (auto failure_message{value_inputter(is, value, value_traits)}; failure_message)
                    throw OptionIOException::ValueInputterFailure(*failure_message, std::source_location::current());
            }
            catch (const std::bad_any_cast&)
            {
                throw InternalError::OptionIOTypeMismatch("Failed to access value inputter or traits", std::source_location::current());
            }

            return std::nullopt;
        }

        template <typename T>
        ValueTraits<T> IOptionIO::get_value_traits() const
        {
            try
            {
                return std::any_cast<ValueTraits<T>>(get_value_traits_());
            }
            catch (const std::bad_any_cast&)
            {
                throw InternalError::OptionIOTypeMismatch("Failed to access value traits", std::source_location::current());
            }
        }

        template <typename T, bool IS_VECTORED_VALUE>
        OptionIOImpl<T, IS_VECTORED_VALUE>::OptionIOImpl
        (
            ValueOutputter<T>&& value_outputter
        ,   ValueInputter<T>&& value_inputter
        ,   ValueTraits<T>&& value_traits
        )
        :   value_traits_(value_traits)
        ,   value_outputter_(value_outputter)
        ,   value_inputter_(value_inputter)
        {}

        template <typename T, bool IS_VECTORED_VALUE>
        std::any OptionIOImpl<T, IS_VECTORED_VALUE>::get_value_inputter_() const
        {
            return value_inputter_;
        }

        template <typename T, bool IS_VECTORED_VALUE>
        std::any OptionIOImpl<T, IS_VECTORED_VALUE>::get_value_traits_() const
        {
            return value_traits_;
        }

        template <typename T, bool IS_VECTORED_VALUE>
        void OptionIOImpl<T, IS_VECTORED_VALUE>::input_value_(std::istream& is, T& value)
        {
            if (auto failure_message{value_inputter_(is, value, value_traits_)}; failure_message)
                throw OptionIOException::ValueInputterFailure(*failure_message, std::source_location::current());
        }

        template <typename T, bool IS_VECTORED_VALUE>
        void OptionIOImpl<T, IS_VECTORED_VALUE>::output_value_(std::ostream& os, const T& value) const
        {
            if (auto failure_message{value_outputter_(os, value, value_traits_)}; failure_message)
                throw OptionIOException::ValueOututterFailure(*failure_message, std::source_location::current());
        }

        template <typename T, bool IS_VECTORED_VALUE>
        void OptionIOImpl<T, IS_VECTORED_VALUE>::output_option_(std::ostream& os) const
        {
            os << get_key(option_ptr_);

            if constexpr (IS_VECTORED_VALUE)
            {
                for (auto items = Internals_::get_value<std::vector<T>>(option_ptr_); const auto& item : items)
                    output_value_(os << " ", item);
            }
            else
            {
                output_value_(os << " ", Internals_::get_value<T>(option_ptr_));
            }
        }

        template <typename T, bool IS_VECTORED_VALUE>
        void OptionIOImpl<T, IS_VECTORED_VALUE>::input_option_value_(std::istream& is)
        {
            if constexpr (IS_VECTORED_VALUE)
            {
                auto& value{Internals_::get_value<Internals_::VectoredValue<T>>(option_ptr_)};
                auto& items{value.items()};
                auto representation_token_count{(get_value_traits<T>()).representation_token_count};

                items.clear();

                for
                (
                    std::size_t items_got{0}, max_items_to_get{value.max_items() * representation_token_count}
                ;   items_got <= max_items_to_get && is
                ;   items_got+=representation_token_count
                )
                {
                    items.resize(items_got / representation_token_count + 1);
                    input_value_(is, items.at(items_got));
                }
            }
            else
            {
                input_value_(is, Internals_::get_value<T>(option_ptr_));
            }
        }
    }
}

#endif // SIMPLE_ARG_PARSER_IOSTREAM_HANDLERS_HPP
