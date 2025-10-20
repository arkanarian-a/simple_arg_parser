#ifndef SIMPLE_ARG_PARSER_SWITCH_STATE_H
#define SIMPLE_ARG_PARSER_SWITCH_STATE_H

#include "simple_arg_parser_iostream_handlers.hpp"


namespace SimpleArgParser
{
    enum class SwitchState: bool
    // Special enum class representing the state of switch-like option value
    // (such options define their value (state) by the presense of their keys in command line).
    {
        Omitted = false     // switch is off (no option key)
    ,   Specified = true    // switch is on (option key is present)
    };

    template <>
    class Internals_::OptionIOImpl<SwitchState, false>: public IOptionIO
    // Helper class aggregating stream inputter and outputter for an option value
    {
    public:

        ~OptionIOImpl() override = default;

    private:

        void output_option_(std::ostream& os) const override;
    };
}

#endif // SIMPLE_ARG_PARSER_SWITCH_STATE_H
