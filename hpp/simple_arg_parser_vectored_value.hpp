#ifndef SIMPLE_ARG_PARSER_VECTORED_VALUE_HPP
#define SIMPLE_ARG_PARSER_VECTORED_VALUE_HPP

#include <vector>
#include <variant>
#include <type_traits>
#include "simple_arg_parser_exceptions.hpp"


namespace SimpleArgParser
{
// ------------
// Declarations
// ------------
    struct Quantifier
    // Represents a quantifier of value count for vectored option value
    {
        std::size_t min_values{0}; // min values must be provided for a vectored option
        std::size_t max_values{std::numeric_limits<std::size_t>::max()}; // max values possible to be provided
    };

    namespace Internals_
    {
        template <typename T>
        struct IsVector: std::false_type
        {};

        template <typename T>
        struct IsVector<std::vector<T>>: std::true_type
        {};

        template <typename T>
        class VectoredValue
        // Wrapper over vectored value. Hides the difference between reference and pointer to vector of values of type T.
        // Enriches std::vector<T> with Quantifier defining the minimal item count required to be contained and maximal
        // item count possible to be contained.
        {
        public:

            using value_type = T;

            VectoredValue() = default;
            VectoredValue(const VectoredValue&) = default;
            VectoredValue(std::vector<T>&&, Quantifier&& = {});
            VectoredValue(std::initializer_list<T>, Quantifier&& = {});
            VectoredValue(std::vector<T>*, Quantifier&& = {});

            VectoredValue& operator=(const VectoredValue&) = default;
            VectoredValue& operator=(VectoredValue&&) = default;

            operator std::vector<T>& ()             { return items_(); };
            operator const std::vector<T>& () const { return items_(); };

            std::vector<T>& items()             { return items_(); }
            const std::vector<T>& items() const { return items_(); }

            std::size_t min_items() const { return quantifier_.min_values; }
            std::size_t max_items() const { return quantifier_.max_values; }

        private:

            std::vector<T>& items_();

            std::variant<std::vector<T>, std::vector<T>*>   value_;
            Quantifier                                      quantifier_;
        };


// -----------
// Definitions
// -----------
        template <typename T>
        VectoredValue<T>::VectoredValue(std::vector<T>&& value, Quantifier&& quantifier)
        :   value_(std::move(value))
        ,   quantifier_(std::move(quantifier))
        {
            // Reserve enough space for items if argument quantifier is not default:
            items_().reserve
            (
                quantifier.max_values < std::numeric_limits<std::size_t>::max()
            ?   quantifier.max_values
            :   quantifier.min_values
            );
        }

        template <typename T>
        VectoredValue<T>::VectoredValue(std::initializer_list<T> init_values, Quantifier&& quantifier)
        :   value_(init_values)
        ,   quantifier_(std::move(quantifier))
        {
            auto item_count{items_().size()};

            // Check if the size of constructed vector of values of type T complies argument quantifier boundaries:
            if (item_count > quantifier.max_values)
                throw OptionAccessException::NumberOfItemsSpecifiedExceedsMaximum(item_count, quantifier.max_values, std::source_location::current());
            else if (item_count < quantifier.min_values)
                throw OptionAccessException::SpecifiedNumberOfItemsIsLessThanMinimum(item_count, quantifier.min_values, std::source_location::current());
        }

        template <typename T>
        VectoredValue<T>::VectoredValue(std::vector<T>* value_ptr, Quantifier&& quantifier)
        :   value_(value_ptr)
        ,   quantifier_(std::move(quantifier))
        {
            auto item_count{items_().size()};

            // Reserve enough space for items if argument quantifier is not default:
            if (item_count > quantifier.max_values)
                throw OptionAccessException::NumberOfItemsSpecifiedExceedsMaximum(item_count, quantifier.max_values, std::source_location::current());
            else if (item_count < quantifier.min_values)
                throw OptionAccessException::SpecifiedNumberOfItemsIsLessThanMinimum(item_count, quantifier.min_values, std::source_location::current());
        }

        template <typename T>
        std::vector<T>& VectoredValue<T>::items_()
        {
            return
                std::visit
                (
                    [] (auto& value) -> std::vector<T>&
                    {
                        using ValueType = std::decay_t<decltype(value)>;

                        if constexpr (std::is_same_v<ValueType, std::vector<T>>)
                            return value;
                        else
                            return *value;
                    }
                ,   value_
                )
            ;
        }

        template <typename T>
        struct IsVector<VectoredValue<T>>: std::true_type
        {};
    }
}

#endif // SIMPLE_ARG_PARSER_VECTORED_VALUE_HPP
