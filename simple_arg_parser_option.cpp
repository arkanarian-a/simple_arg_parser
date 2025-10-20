#include <utility>
#include <limits>
#include "hpp/simple_arg_parser.hpp"

namespace SimpleArgParser
{
    const std::string_view& Internals_::get_key(Option* option_ptr)
    {
        return option_ptr->get_key();
    }


    void Internals_::OptionIOImpl<SwitchState, false>::output_option_(std::ostream& os) const
    {
        if (Internals_::get_value<SwitchState>(option_ptr_) == SwitchState::Specified)
            os << get_key(option_ptr_);
    }


    constinit std::size_t Option::UNLIMITED_MAX_ITEMS{std::numeric_limits<std::size_t>::max()};

    Option::Option(const Option& option)
    // Because of constructing vector<Option> from std::initializer_list<T> in Parser constructor and perceiving objects
    // from brace-enclosed initializer list as <const T> by the std::initializer_list<T> this copy constructor is needed.
    :   attributes_(option.attributes_)
    ,   value_(option.value_)
    ,   arg_parser_(option.arg_parser_)
    ,   io_handler_(option.io_handler_)
    {
        // The source Option object keeps in the io_handler_ the pointer to input/output handler which stay inconsistent
        // after its construction until explicit linking it to Option object it serves for.
        // That's why we MUST link the Option object copy to io_handler_ (set Option object copy's address into io_handler_).
        io_handler_->link_to(this);
    }

    Option::Option(OptionAttributes&& attributes, SwitchState&& init_value)
    :   attributes_(std::move(attributes))
    ,   value_(Internals_::ScalarValue(std::move(init_value)))
    ,   arg_parser_(&Option::set_switch_option_on_)
    ,   io_handler_(std::make_shared<Internals_::OptionIOImpl<SwitchState, false>>())
    {}

    Option::Option
    (
        OptionAttributes&& attributes
    ,   SwitchState* value_ptr
    )
    :   attributes_(attributes)
    ,   value_(Internals_::ScalarValue(value_ptr))
    ,   arg_parser_(&Option::set_switch_option_on_)
    ,   io_handler_(std::make_shared<Internals_::OptionIOImpl<SwitchState, false>>())
    {}

    const std::string_view& Option::get_key() const
    {
        return attributes_.key;
    }

    bool Option::has_value() const
    {
        return value_.has_value();
    }

    std::ostream& Option::output(std::ostream& os) const
    {
        io_handler_->output_option(os);

        return os;
    }

    std::istream& Option::input(std::istream& is)
    {
        io_handler_->input_option_value(is);

        return is;
    }

    void Option::link_to_(Parser* parser_ptr)
    {
        parser_ptr_ = parser_ptr;
    }

    bool Option::option_key_is_defined_(std::string_view option_key) const
    {
        return parser_ptr_ && parser_ptr_->has_option(option_key);
    }

    int Option::parse_option_argument_(SubrangeOfArgV_& subrange_of_argv)
    {
        return (this->*arg_parser_)(subrange_of_argv);
    }

    int Option::set_switch_option_on_(SubrangeOfArgV_&)
    {
        get_value_<SwitchState>() = Specified;

        return 1;
    };

    std::ostream& operator<<(std::ostream& os, const Option& option)
    {
        return option.output(os);
    }

    std::istream& operator>>(std::istream& is, Option& option)
    {
        return option.input(is);
    }

    std::ostream& operator<<(std::ostream& os, SwitchState state)
    {
        return os << (state == SwitchState::Specified ? "SwitchState::Specified": "SwitchState::Omitted");
    }
}
