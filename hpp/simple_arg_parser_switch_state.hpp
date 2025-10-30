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
