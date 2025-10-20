#ifndef SIMPLE_ARG_PARSER_SCALAR_VALUE_HPP
#define SIMPLE_ARG_PARSER_SCALAR_VALUE_HPP

#include <variant>
#include <type_traits>

namespace SimpleArgParser::Internals_
{
    template <typename T>
    class ScalarValue
    // Wrapper over the scalar value. Hides the difference between the reference and pointer to value ot type T
    {
    public:

        ScalarValue() = default;
        ScalarValue(const ScalarValue&) = default;
        ScalarValue(ScalarValue&&) = default;

        ScalarValue(T&& value)
        :   value_(std::move(value))
        {}

        ScalarValue(T* value)
        :   value_(value)
        {}

        ScalarValue& operator=(const ScalarValue&) = default;
        ScalarValue& operator=(ScalarValue&&) = default;

        T& get_value()              { return get_value_(); };
        const T& get_value() const  { return get_value_(); };

    private:

        T& get_value_()
        {
            return
                std::visit
                (
                    [] (auto& value) -> T&
                    {
                        using ValueType = std::decay_t<decltype(value)>;

                        if constexpr (std::is_same_v<ValueType, T>)
                            return value;
                        else
                            return *value;
                    }
                ,   value_
                )
            ;
        }

        std::variant<T, T*> value_;
    };
}

#endif // SIMPLE_ARG_PARSER_SCALAR_VALUE_HPP
