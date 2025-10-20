#include <iostream>
#include "simple_arg_parser.hpp"
#include "simple_arg_parser_auxiliaries.hpp"


namespace SAP = SimpleArgParser;


struct NamedInt
{
    std::string name{"IntValue"};
    int         value{42};
};

std::ostream& operator<<(std::ostream& os, const NamedInt& ni)
{
    return os << ni.name << ' ' << ni.value;
}

std::istream& operator>>(std::istream& is, NamedInt& ni)
{
    return is >> ni.name >> ni.value;
}

template <>
SAP::ValueTraits<NamedInt>::ValueTraits(): TypeIndependentValueTraits(2)
{}


int main()
{
    const char* argv[] =
    {
        "sapt_vector_valued_option_parsing",
        "--vector-of-named-ints",
        "Zero",
        "0",
        "One",
        "1",
        "Two",
        "2",
        "--vector-of-ints",
        "0",
        "1",
        "2"
    };

    int argc{sizeof(argv) / sizeof(const char*)};

    std::vector<int>        voi;
    std::vector<NamedInt>   voni;

    try
    {
        SAP::Parser parser
        {
            { { "--vector-of-ints"sv }, &voi },
            { { "--vector-of-named-ints" }, &voni }
        };

        std::cout << "Command line argument parsed: " << parser.parse(argc, argv) << std::endl;
        std::cout << "Resulting options are: " << parser << std::endl;
    }
    catch (SAP::OptionException& oe)
    {
        oe.output(std::cout, std::source_location::current());
    }

    std::cout << "voi == " << SAP::qd(voi) << '\n';
    std::cout << "voni == " << SAP::qd(voni) << std::endl;

    return 0;
}
