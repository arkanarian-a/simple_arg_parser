CONFIG -= qt

TEMPLATE = lib
DEFINES += SIMPLE_ARG_PARSER_LIBRARY

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_CXXFLAGS += -std=gnu++20 #-fsanitize=address

VERSION = 1.0.0

SOURCES += \
    simple_arg_parser.cpp \
    simple_arg_parser_option.cpp

HEADERS += \
    hpp/simple_arg_parser.hpp \
    hpp/simple_arg_parser_auxiliaries.hpp \
    hpp/simple_arg_parser_compiler_fine_tunes.hpp \
    hpp/simple_arg_parser_exceptions.hpp \
    hpp/simple_arg_parser_iostream_handlers.hpp \
    hpp/simple_arg_parser_option.hpp \
    hpp/simple_arg_parser_scalar_value.hpp \
    hpp/simple_arg_parser_spec_value_traits.hpp \
    hpp/simple_arg_parser_switch_state.hpp \
    hpp/simple_arg_parser_vectored_value.hpp

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    LICENSE.txt \
    readme.md
