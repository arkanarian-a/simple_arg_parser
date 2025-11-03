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

#include <iostream>
#include <flat_map>
#include <regex>
#include "simple_arg_parser.hpp"
#include "simple_arg_parser_compiler_fine_tunes.hpp"    // <-- This's included to avoid the lack of std::runtime_format()
                                                        //     function in C++ standard of version less than 26


namespace SAP = SimpleArgParser;

using namespace std::literals::string_view_literals;
using namespace std::literals::string_literals;


// ------------------------------------------------------------------
// This sample illustrates customization hints for user defined types
// ------------------------------------------------------------------

struct IPv4Address
// E.g.: we've got a type to store an IPv4 address
{
    using Octet = std::byte;

    std::array<Octet, 4> octet{}; // array of 4 bytes to store the IP address
};


// The simpliest customization of input/output option values of type IPv4Address is to define classical stream operators.
// This way imposes some restrictions on the value representation in the stream:
// 1. It must consist of one token (not containing any whitespaces word) only;
// 2. There are only one representation for all objects of the type.
// For IPv4 address it fits perfect: \d{1, 3}.\d{1, 3}.\d{1, 3}.\d{1, 3}

std::ostream& operator<<(std::ostream& os, const IPv4Address& ip)
{
    return
    os
    <<  std::to_integer<short>(ip.octet[0])
    <<  "."
    <<  std::to_integer<short>(ip.octet[1])
    <<  "."
    <<  std::to_integer<short>(ip.octet[2])
    <<  "."
    <<  std::to_integer<short>(ip.octet[3])
    ;
}

std::istream& operator>>(std::istream& is, IPv4Address& ip)
{
    unsigned short octet[4];

    (((is >> octet[0]).ignore(1, '.') >> octet[1]).ignore(1, '.') >> octet[2]).ignore(1, '.') >> octet[3];

    ip.octet[0] = static_cast<IPv4Address::Octet>(octet[0]);
    ip.octet[1] = static_cast<IPv4Address::Octet>(octet[1]);
    ip.octet[2] = static_cast<IPv4Address::Octet>(octet[2]);
    ip.octet[3] = static_cast<IPv4Address::Octet>(octet[3]);

    return is;
}
// ------------------------------------------------------------------


struct CartesianCoords
// Another sample - an object containing 3 cartesian coordinates
{
    double x{};
    double y{};
    double z{};
};

// Cartesian coordinates could be represented in different ways including their order (x y z or z y x for example),
// delimiters, name prefixes and so on. Such variety entails unpredictable number of tokens in the stream for each
// representation. So, we need more flexible (that's why more complex) customizing way.

// First of all we need to define own SAP::ValueTraits<CartesianCoords> template specialization filled with
// CartesianCoords-specific traits:
template <>
struct SAP::ValueTraits<CartesianCoords>: public TypeIndependentValueTraits
// Structure to define differente type-specific traits for an option value.
// It could be specialized for the type if needed.
{
    enum class CoordOrder : std::size_t
    // This is to define the coordinates order in representation
    {
        XYZ = 0
    ,   XZY = 1
    ,   YXZ = 2
    ,   YZX = 3
    ,   ZXY = 4
    ,   ZYX = 5
    };

static constexpr std::size_t   CARDINALITY{3}; // CartesianCoords type has 3 coordinates (x y z)

static const std::flat_map<CoordOrder, std::string_view> coord_orders; // A map for CoordOrder to string translation

    ValueTraits(CoordOrder default_coord_order = CoordOrder::XYZ, std::size_t representation_token_count = 3)
    // Constructor takes the default order of coordinates and token count for the representation
    :   TypeIndependentValueTraits(representation_token_count) // 3 tokens to represent the value by default
    ,   coord_order(default_coord_order)
    {}

    std::ostream& fancy_output
    (
        std::ostream& os
    ,   const CartesianCoords& coords
    ,   std::string_view coord_value_delimiter = ""sv
    ,   std::string_view coord_name_delimiter = ""sv
    )
    // Fancy output with specified value (<cvd/>) and name (<cnd/>) delimiters for representation like
    // x<cnd/>x_value<cvd/> y<cnd/>y_value<cvd/> z<cnd/>z_value<cvd/> (in currently set coordinate order)
    const
    {
        for (std::size_t passed{1}; auto coord_name : coord_orders.at(coord_order))
        {
            if (!coord_name_delimiter.empty())
                os << coord_name << coord_name_delimiter;

            switch (coord_name)
            {
                case 'x': os << coords.x; break;
                case 'y': os << coords.y; break;
                case 'z': os << coords.z; break;
            }

            if (passed++ < CARDINALITY)
                std::cout << coord_value_delimiter;
        }

        return os;
    }

    std::optional<std::string> output(std::ostream& os, const CartesianCoords& coords) const
    // This is the mandatory output method specialization wich is a key customization point for the SAP::ValueTraits.
    // This method is called by SAP::Option for outputting its value.
    // It must return std::nullopt on success or an error description string on failure. In case of error return
    // the description will be inserted into SAP::OptionIOException::ValueOututterFailure exception error message.
    {
        fancy_output(os, coords, " "sv, "="sv);

        return std::nullopt;
    }

    std::string coord_order_label() const
    // Coordinates label of ({};{};{}) format for another ('labeled') representation
    {
        const auto& coord_names = coord_orders.at(coord_order);

        return SAP::runtime_format("({};{};{})"sv, coord_names[0], coord_names[1], coord_names[2]);
    }

    CoordOrder  coord_order; // currently set order of coordinates
};

// The map for CoordOrder to string translation definition:
const std::flat_map<SAP::ValueTraits<CartesianCoords>::CoordOrder, std::string_view>
    SAP::ValueTraits<CartesianCoords>::coord_orders
    {
        { CoordOrder::XYZ, "xyz"sv }
    ,   { CoordOrder::XZY, "xzy"sv }
    ,   { CoordOrder::YXZ, "yxz"sv }
    ,   { CoordOrder::YZX, "yzx"sv }
    ,   { CoordOrder::ZXY, "zxy"sv }
    ,   { CoordOrder::ZYX, "zyx"sv }
    }
;

// It's possible to define even specific (representation) outputter for any specific option.
// In such case it must be passed to Option constructor explicitly as an argument.
std::optional<std::string> labeled_outputter(std::ostream& os, const CartesianCoords& coords, const SAP::ValueTraits<CartesianCoords>& traits)
{
    traits.fancy_output(os << traits.coord_order_label() << "=(", coords, ";"sv) << ')';

    return std::nullopt;
}

// The SAP::default_value_inputter template function specialization allows to define default representation inputter for any
// Option containing a value of the type (CartesianCoords here). Note the use of traits.representation_token_count in this
// specialization. It's needed here to fetch correct number of tokens from the stream. If the representation consist of
// only one token, traits may be not needed at all.
// Also, note the error description return on failure case.
template <>
std::optional<std::string> SAP::default_value_inputter(std::istream& is, CartesianCoords& coords, SAP::ValueTraits<CartesianCoords>& traits)
{
    for (std::size_t coord_count{traits.representation_token_count}; coord_count; --coord_count)
    {
        std::string coord_name;

        std::getline(is >> std::ws, coord_name, '=');

        switch (coord_name[0])
        {
            case 'x': case 'X': is >> coords.x; break;
            case 'y': case 'Y': is >> coords.y; break;
            case 'z': case 'Z': is >> coords.z; break;
            default: return std::format("Incorrect cartesian coordinates input format! \"{}\" coordinate name met when x, y or z are allowed only", coord_name);
        }
    }

    return std::nullopt;
}

// Here we define 'labeled' representation inputter which must be passed to Option constructor explicilty.
// As the representation format is a bit tricky to parse, we use std::regex, std::stod to convert parsed value to double
// and catch std::exception to process std::stod's convertion errors.
std::optional<std::string> labeled_inputter(std::istream& is, CartesianCoords& coords, SAP::ValueTraits<CartesianCoords>& traits)
{
    assert(traits.representation_token_count == 1 && "labeled_inputter function requires traits.representation_token_count == 1!");

    std::string labeled_representation;

    is >> labeled_representation;

    const std::regex
        labeled_value_pattern
        {
            R"(\(([xyz]{1});([xyz]{1});([xyz]{1})\)=\(([+-]?\d*\.\d+);([+-]?\d*\.\d+);([+-]?\d*\.\d+)\))"
        ,   std::regex::icase
        }
    ;

    std::smatch matches;

    if (!std::regex_match(labeled_representation, matches, labeled_value_pattern))
        return std::format("Value input failed: '{}' does not comply 'labeled' representation!", labeled_representation);

    try
    {
        for (std::size_t i = 1; i < 4; ++i)
        {
            switch (matches[i].str()[0])
            {
            case 'x': case 'X': coords.x = std::stod(matches[i + 3].str()); break;
            case 'y': case 'Y': coords.y = std::stod(matches[i + 3].str()); break;
            case 'z': case 'Z': coords.z = std::stod(matches[i + 3].str()); break;
            default: return std::format("Incorrect cartesian coordinates input format! '{}' coordinate name met when x, y or z are allowed only", matches[i].str());
            }
        }
    }
    catch (const std::exception& e)
    {
        return std::format("Coordinate value conversion to double failed: {}", e.what());
    }

    return std::nullopt;
}

int main(int argc, const char* argv[])
// Try to run this sample with arguments like this:
// -ipv4a 192.168.0.1 -ccs z=0.12 y=0.5 x=1.3 -3dps "(x;y;z)=(.1;.01;.5)" "(x;y;z)=(.01;.1;.3)" "(x;y;z)=(0;.0;.0)" --ip-v4-address-list 192.168.1.1 192.168.1.2 192.168.1.100
// Than try to change the command line and see the resulting error messages
{
    try
    {
        IPv4Address                 ip;
        std::vector<IPv4Address>    ip_list;

        CartesianCoords                 coords;
        std::vector<CartesianCoords>    _3d_points;

        SAP::Parser
            parser
            {
                { { "--ip-v4-address"sv, "-ipv4a"sv }, &ip } // <- Define option on ip variable
            ,   { { "--ip-v4-address-list"sv }, &ip_list }   // <- Define option on ip_list variable (Note: without alias key)
            ,   {  // Define option on coords variable:
                    { "--cartesian-coords"sv, "-ccs"sv } // <- Define option's key and its alias key
                ,   &coords // <- Pass the address of variable coords to use it as the option's value
                ,   { SAP::ValueTraits<CartesianCoords>::CoordOrder::ZYX } // <- Supply the option with appropriate value traits
                }
            ,   {  // Define option on _3d_points variable:
                    { "--3d-points"sv, "-3dps"sv } // <- Define option's key and alias key
                ,   &_3d_points // <- Pass the address of variable _3d_points (option's value)
                ,   { 0, 2 }    // <- Set the quantifier of min 0 and max 2 items for _3d_points vector
                                //    Note that if you set more than 0 items as minimum you MUST init
                                //    _3d_points variable with at least that number of items, otherwise the
                                //    SAP::OptionAccessException::SpecifiedNumberOfItemsIsLessThanMinimum
                                //    will be thrown when constructing Option object, and if you init the
                                //    _3d_points variable with more than max items you'll cause throwing
                                //    SAP::OptionAccessException::NumberOfItemsSpecifiedExceedsMaximum exception.
                                //    Also, the max items causes ignoring any extra items in --3d-points argument.
                ,   { SAP::ValueTraits<CartesianCoords>::CoordOrder::XYZ, 1 } // <- Value traits object for the option
                ,   { ::labeled_inputter }  // <- Custom value inputter for the option
                ,   { ::labeled_outputter } // <- Custom value outputter for the option
                }
            }
        ;

        std::cout << "Command line argument parsed: " << parser.parse(argc, argv) << std::endl;
        std::cout << "Resulting options are: " << parser << std::endl;

        std::cout << "\nVariables are initialized with following values:";
        std::cout << "\nip: " << ip;
        std::cout << "\nip_list: ";
        std::copy(ip_list.cbegin(), ip_list.cend(), std::ostream_iterator<IPv4Address>(std::cout, " "));
        std::cout << "\ncoords: ";
        SAP::ValueTraits<CartesianCoords>{}.output(std::cout, coords);
        std::cout << "\n_3d_points: ";
        std::for_each
        (
            _3d_points.cbegin()
        ,   _3d_points.cend()
        ,   [] (const CartesianCoords& point_coords)
            {
                labeled_outputter(std::cout, point_coords, { SAP::ValueTraits<CartesianCoords>::CoordOrder::ZYX, 1 });
                std::cout << " ";
            }
        );
        std::cout << std::endl;
    }
    catch (const SAP::OptionException& oe)
    {
        oe.output(std::cout, std::source_location::current());
    }

    return 0;
}
