QT       += core gui sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17#11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    account.cpp \
    baseclass.cpp \
    ingredients.cpp \
    login.cpp \
    main.cpp \
    mainwindow.cpp \
    recipes.cpp \
    removerecipes.cpp \
    removeresources.cpp \
    resources.cpp \
    rfid.cpp \
    search.cpp \
    settings.cpp \
    shoppinglist.cpp

HEADERS += \
    account.h \
    baseclass.h \
    ingredients.h \
    login.h \
    mainwindow.h \
    recipes.h \
    removerecipes.h \
    removeresources.h \
    resources.h \
    rfid.h \
    search.h \
    settings.h \
    shoppinglist.h

FORMS += \
    baseui.ui \
    ingredients.ui \
    login.ui \
    mainwindow.ui \
    recipes.ui \
    removerecipes.ui \
    removeresources.ui \
    resources.ui \
    search.ui \
    settings.ui \
    shoppinglist.ui

# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target

LIBS += -L$$PWD/sdk/INC/ -lVHLib

INCLUDEPATH += $$PWD/sdk
#DEPENDPATH += $$PWD/sdk

DISTFILES += \
    main.qss

RESOURCES += \
    resources.qrc
