TEMPLATE = app
CONFIG += console c++23
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=gnu++23

SOURCES += \
        main.cpp

unix:!macx: LIBS += -L$$PWD/../../../build/Desktop-Debug/ -lsimple_arg_parser

INCLUDEPATH += $$PWD/../../../hpp
DEPENDPATH += $$PWD/../../../hpp

unix:!macx: LIBS += -L$$PWD/../../../build/Desktop-Debug/ -lsimple_arg_parser

INCLUDEPATH += $$PWD/''
DEPENDPATH += $$PWD/''
