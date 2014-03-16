TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -std=c++0x
LIBS += -lncurses

SOURCES += main.cpp \
    view.cpp \
    game.cpp \
    world.cpp \
    tile.cpp \
    player.cpp \
    item.cpp \
    combat.cpp \
    monster.cpp \
    door.cpp \
    trap.cpp \
    armament.cpp \
    weapon.cpp \
    potion.cpp \
    key.cpp \
    shield.cpp

HEADERS += \
    view.h \
    game.h \
    state.h \
    world.h \
    tile.h \
    terrain.h \
    player.h \
    item.h \
    itemtype.h \
    combat.h \
    monster.h \
    door.h \
    trap.h \
    armament.h \
    weapon.h \
    potion.h \
    key.h \
    shield.h \
    direction.h

