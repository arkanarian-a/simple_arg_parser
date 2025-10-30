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

#include <iostream>
#include <chrono>
#include "simple_arg_parser.hpp"
#include "simple_arg_parser_spec_value_traits.hpp" // IWYU pragma: keep
#include "simple_arg_parser_auxiliaries.hpp"

// --------------------------------------------------------------
// This sample illustrates basic usage of SimpleArgParser library
// --------------------------------------------------------------

namespace SAP = SimpleArgParser;

using namespace std::literals::string_view_literals;
using namespace std::literals::string_literals;
using namespace std::literals::chrono_literals;

using sl = std::source_location;

using SAP::lno; // Using std::ostream manipulator outputting current source line (from std::source_location::current())
using SAP::qd;  // Using std::ostream manipulator outputting a value in quotes (possibly specified by user)


int main(int argc, const char* argv[])
// Try to run this sample with command line argumens like
// -i 1000 -s '"And this string is passed from the command line now!"' -b true -sw
// Than try to change it and see resulting messages.
{
    try
    {
        // First let declare options while innitializing the SAP::Parser:
        SAP::Parser
            parser
            {
            //  Here goes two SAP::Parser constructor's parameters ...
                {
                    { { "--switch"sv, "-sw"sv }, SAP::Option::Omitted },
                //      ^             ^          ^
                //      |             |          |
                //      |             |          +-- initial value for the option
                //      |             +-- option key alias (optional)
                //      +-- option key (which makes it known to the parser)
                //  So, this declares a switch-style option that doesn't take any arguments besides its key.
                //  When the key is specified, switch option is set "on", othewise it set "off".
                //  This sample declares a switch option that is set "off" by default.
                    { { "--bool"sv, "-b"sv }, false },
                    { { "--int"sv, "-i"sv }, -42 },
                    { { "--double"sv, "-d"sv }, 3.14159259 },
                    { { "--string"sv, "-s"sv }, "Just a sample string... (that's default)"s },
                    { { "--timepoint"sv, "-tp"sv }, std::chrono::system_clock::time_point{} },
                    { { "--vector-of-ints"sv, "-voi"sv }, { -1, 0, 1 }, { 3, 5 } },
                 //                                       ^             ^
                 //                                       |             |
                 //                                       |             +-- here is the quantifier definition
                 //                                       |                 this one requires minimum 3 items
                 //                                       |                 and consumes no more than 5 items
                 //                                       |                 when parsing option value
                 //                                       +-- here goes a vector of ints initializer list
                    { { "--vector-of-doubles"sv, "-vod"sv }, { -3.14159259, 0., 3.14159259 }, { 1 } },
                 //                                                                           ^
                 //                                                                           |
                 //                                                                           +-- here is the quantifier again,
                 //                                                                               but with ommited upper bound
                 //                                                                               assuming "not less than 1
                 //                                                                               and no limit besides it"
                    { { "--vector-of-strings"sv, "-vos"sv }, { "Zero"s, "One"s, "Two"s, "Three"s, "Four"s } },
                    {
                        { "--vector-of-timepoints"sv, "-votp"sv },
                        { std::chrono::system_clock::now(), std::chrono::system_clock::now() },
                        { 2, 2 } // <-- onother special case of quantifier meanning "must be exactly 2 items"
                    }
                },
                SAP::ParsingPolicy::ForbidUndeclaredOptions // <-- the second one is optional parsing policy.
                                                            //     This sample says "Do not specify any undeclared
                                                            //     options in the command line or catch an exception!".
                                                            //     If this parameter is omitted, default policy
                                                            //     SAP::ParsingPolicy::SkipUndeclaredOptions is applied which
                                                            //     means "Skip any undeclared option key and keep on".
            }
        ;

        std::cout << lno() << "// Here we parse arguments from command line:\n";
        std::cout << lno() << "Command line argument parsed: " << parser.parse(argc, argv) << std::endl;
        std::cout << lno() << "// ... and show the resulting options which are not specified in command line, so defaulted:\n";
        std::cout << lno() << "Resulting options are: " << parser << std::endl;

        std::cout << "-----" << std::endl;

        std::cout << lno() << "// So, we've parsed command line options (ommited ones are defaulted to values, specified in parser initialization)\n";
        std::cout << lno() << "// and now we can get their values, accessing them by their keys\n";
        std::cout << lno() << "// (Note that we always can use short option key ailas if it's defined):\n";
        std::cout << lno() << "parser[\"-sw\"] => " << qd(parser["-sw"sv]) << "\n";
        std::cout << lno() << "parser[\"-b\"] => " << qd(parser["-b"sv]) << "\n";
        std::cout << lno() << "parser[\"-i\"] => " << qd(parser["-i"sv]) << "\n";
        std::cout << lno() << "parser[\"-d\"] => " << qd(parser["-d"sv]) << "\n";
        std::cout << lno() << "parser[\"-s\"] => " << qd(parser["-s"sv]) << "\n";
        std::cout << lno() << "parser[\"-tp\"] => " << qd(parser["-tp"sv]) << "\n";
        std::cout << lno() << "parser[\"-voi\"] => " << qd(parser["-voi"sv]) << "\n";
        std::cout << lno() << "parser[\"-vod\"] => " << qd(parser["-vod"sv]) << "\n";
        std::cout << lno() << "parser[\"-vos\"] => " << qd(parser["-vos"sv]) << "\n";
        std::cout << lno() << "parser[\"-votp\"] => " << qd(parser["-votp"sv]) << "\n";

        std::cout << "-----" << std::endl;

        std::cout << lno() << "// Accessing values keeped by parser may be not useful (especially after exceeding its lifetime),\n";
        std::cout << lno() << "// that's why using the method SAP::Option::copy_value may make sense:\n";

        int i{};

        std::cout << lno() << "parser[\"--int\"].copy_value(i) == " << parser["--int"sv].copy_value(i) << "\n";

        double d{};

        std::cout << lno() << "parser[\"--double\"].copy_value(d) == " << parser["--double"sv].copy_value(d) << "\n";

        std::string s{};

        std::cout << lno() << "parser[\"--string\"].copy_value(s) == " << qd(parser["--string"sv].copy_value(s), "'") << "\n";

        std::chrono::system_clock::time_point tp{};

        std::cout << lno() << "parser[\"--timepoint\"].copy_value(tp) == " << qd(parser["--timepoint"sv].copy_value(tp), "'") << "\n";

        std::vector<int> voi{};

        std::cout << lno() << "parser[\"--vector-of-ints\"].copy_value(voi) == " << qd(parser["--vector-of-ints"sv].copy_value(voi)) << "\n";

        std::vector<double> vod{};

        std::cout << lno() << "parser[\"--vector-of-doubles\"].copy_value(vod) == " << qd(parser["--vector-of-doubles"sv].copy_value(vod)) << "\n";

        std::vector<std::string> vos{};

        std::cout << lno() << "parser[\"--vector-of-strings\"].copy_value(vos) == " << qd(parser["--vector-of-strings"sv].copy_value(vos), "\"") << "\n";

        std::vector<std::chrono::system_clock::time_point> votp{};

        std::cout << lno() << "parser[\"--vector-of-timepoints\"].copy_value(votp) == " << qd(parser["--vector-of-timepoints"sv].copy_value(votp), "'") << "\n";

        std::cout << "-----" << std::endl;
    }
    catch (const SAP::OptionException& oe)
    {
        // ... and here we come if you've specified any option with a key, that hasn't been declared when the parser constructed
        // [Try to run the sample with something like --and-i-am-not-declared to see the effect]
        oe.output(std::cout, std::source_location::current());
    }

    try
    {
        std::cout << lno() << "// Here we parse arguments from command line putting their values into variables pointed by their addresses:\n";

        SAP::SwitchState sw{};
        auto b{false};
        auto i{42};
        auto d{3.14};
        auto s{"Just another default sample string..."s};
        auto tp{std::chrono::system_clock::now()};
        std::vector voi{ -1, 0, 1 };
        std::vector vod{ -3.14159259, 0., 3.14159259 };
        std::vector vos{ "Zero"s, "One"s, "Two"s, "Three"s, "Four"s };
        std::vector votp{ std::chrono::system_clock::now(), std::chrono::system_clock::now() };

        SAP::Parser
            parser
            {
                { { "--switch"sv, "-sw"sv }, &sw },
                { { "--bool"sv, "-b"sv }, &b },
                { { "--int"sv, "-i"sv }, &i },
                { { "--double"sv, "-d"sv }, &d },
                { { "--string"sv, "-s"sv }, &s },
                { { "--timepoint"sv, "-tp"sv }, &tp },
                { { "--vector-of-ints"sv, "-voi"sv }, &voi, { 1, 3 } },
                { { "--vector-of-doubles"sv, "-vod"sv }, &vod, { 1 } },
                { { "--vector-of-strings"sv, "-vos"sv }, &vos },
                { { "--vector-of-timepoints"sv, "-votp"sv }, &votp, { 2, 2 } }
            }
        ;

        std::cout << lno() << "Command line argument parsed: " << parser.parse(argc, argv) << std::endl;
        std::cout << lno() << "Resulting options are: " << parser << std::endl;

        std::cout << "-----" << std::endl;

        std::cout << lno() << "// We can still access option values by option keys (although they stay in variables, not in options):\n";
        std::cout << lno() << "parser[\"-sw\"] => " << qd(parser["-sw"sv]) << "\n";
        std::cout << lno() << "parser[\"-b\"] => " << qd(parser["-b"sv]) << "\n";
        std::cout << lno() << "parser[\"-i\"] => " << qd(parser["-i"sv]) << "\n";
        std::cout << lno() << "parser[\"-d\"] => " << qd(parser["-d"sv]) << "\n";
        std::cout << lno() << "parser[\"-s\"] => " << qd(parser["-s"sv]) << "\n";
        std::cout << lno() << "parser[\"-tp\"] => " << qd(parser["-tp"sv]) << "\n";
        std::cout << lno() << "parser[\"-voi\"] => " << qd(parser["-voi"sv]) << "\n";
        std::cout << lno() << "parser[\"-vod\"] => " << qd(parser["-vod"sv]) << "\n";
        std::cout << lno() << "parser[\"-vos\"] => " << qd(parser["-vos"sv]) << "\n";
        std::cout << lno() << "parser[\"-vtp\"] => " << qd(parser["-votp"sv]) << "\n";

        std::cout << "-----" << std::endl;

        std::cout << lno() << "// Otherwise, we can access option values just accessing the variables:\n";
        std::cout << lno() << "sw == " << (sw == SAP::Option::Specified ? "Specified" : "Omitted") << "\n";
        std::cout << lno() << "b == " << b << "\n";
        std::cout << lno() << "i == " << i << "\n";
        std::cout << lno() << "d == " << d << "\n";
        std::cout << lno() << "s == " << qd(s) << "\n";
        std::cout << lno() << "tp == " << qd(tp, "'") << "\n";
        std::cout << lno() << "voi == " << qd(voi) << "\n";
        std::cout << lno() << "vod == " << qd(vod) << "\n";
        std::cout << lno() << "vos == " << qd(vos, "\"") << "\n";
        std::cout << lno() << "votp == " << qd(votp, "'") << "\n";

        std::cout << "-----" << std::endl;
    }
    catch (const SAP::OptionException& oe)
    {
        oe.output(std::cout, std::source_location::current());
    }

    try
    {
        std::cout << lno() << "// Also, it's possible to combine options initialized by value with options keeping value's address:\n";

        auto b{false};
        auto d{3.14};
        auto tp{std::chrono::system_clock::now()};
        std::vector vod{ -3.14159259, 0., 3.14159259 };
        std::vector votp{ std::chrono::system_clock::now(), std::chrono::system_clock::now() };

        SAP::Parser
            parser
            {
                { { "--switch"sv, "-sw"sv }, SAP::Option::Omitted },
                { { "--bool"sv, "-b"sv }, &b },
                { { "--int"sv, "-i"sv }, 42 },
                { { "--double"sv, "-d"sv }, &d },
                { { "--string"sv, "-s"sv }, "Just another default sample string..."s },
                { { "--timepoint"sv, "-tp"sv }, &tp },
                { { "--vector-of-ints"sv, "-voi"sv }, { -1, 0, 1 }, { 1, 3 } },
                { { "--vector-of-doubles"sv, "-vod"sv }, &vod, { 1, SAP::Option::UNLIMITED_MAX_ITEMS } },
                { { "--vector-of-strings"sv, "-vos"sv }, { "Zero"s, "One"s, "Two"s, "Three"s, "Four"s } },
                { { "--vector-of-timepoints"sv, "-votp"sv }, &votp, { 2, 2 } }
            }
        ;

        std::cout << lno() << "Command line argument parsed: " << parser.parse(argc, argv) << std::endl;
        std::cout << lno() << "Resulting options are: " << parser << std::endl;

        std::cout << "-----" << std::endl;

        std::cout << lno() << "parser[\"-sw\"] => " << qd(parser["-sw"sv]) << "\n";
        std::cout << lno() << "b == " << b << "\n";
        std::cout << lno() << "parser[\"-i\"] => " << qd(parser["-i"sv]) << "\n";
        std::cout << lno() << "d == " << d << "\n";
        std::cout << lno() << "parser[\"-s\"] => " << qd(parser["-s"sv]) << "\n";
        std::cout << lno() << "tp == " << qd(tp, "'") << "\n";
        std::cout << lno() << "parser[\"-voi\"] => " << qd(parser["-voi"sv]) << "\n";
        std::cout << lno() << "vod == " << qd(vod) << "\n";
        std::cout << lno() << "parser[\"-vos\"] => " << qd(parser["-vos"sv]) << "\n";
        std::cout << lno() << "votp == " << qd(votp, "'") << "\n";

        std::cout << "-----" << std::endl;
    }
    catch (const SAP::OptionException& oe)
    {
        oe.output(std::cout, std::source_location::current());
    }

    return 0;
}
