TEMPLATE = app
CONFIG += console c++20
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=gnu++20 #-fsanitize=address

SOURCES += \
        main.cpp

unix:!macx: LIBS += -L$$PWD/../../build/Desktop-Debug/ -lsimple_arg_parser

INCLUDEPATH += $$PWD/../../hpp
DEPENDPATH += $$PWD/../../hpp
