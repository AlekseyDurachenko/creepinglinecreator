# Copyright 2014, Durachenko Aleksey V. <durachenko.aleksey@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# project options
TARGET          = creepinglinecreator
TEMPLATE        = app

# build options
CONFIG         += console debug_and_release
QT             += core gui

# Release build options
release:DESTDIR     = bin
release:OBJECTS_DIR = build/release/obj
release:MOC_DIR     = build/release/moc
release:RCC_DIR     = build/release/rcc
release:UI_DIR      = build/release/ui
# Debug build options
debug:DESTDIR       = bin
debug:OBJECTS_DIR   = build/debug/obj
debug:MOC_DIR       = build/debug/moc
debug:RCC_DIR       = build/debug/rcc
debug:UI_DIR        = build/debug/ui

# uncomment this line if you want to start the test
# or use: qmake "QT += testlib"
#QT      += testlib

# uncomment this line if you want use application as portable
# (all files will placed in the application directory)
#DEFINES += APP_PORTABLE

# uncomment this line if you want deploy the application
# (this line enables the many additional dialogs)
#DEFINES += APP_DEPLOY

# sources
INCLUDEPATH    +=                                           \
    $$PWD/src                                               \

HEADERS        +=                                           \
    src/global.h                                            \
    src/consts.h                                            \
    src/cmainwindow.h                                       \
    src/caboutdialog.h                                      \

SOURCES        +=                                           \
    src/cmainwindow.cpp                                     \
    src/caboutdialog.cpp                                    \

FORMS          +=                                           \
    src/cmainwindow.ui                                      \
    src/caboutdialog.ui                                     \

RESOURCES      +=                                           \
    qrc/icons.qrc                                           \
    qrc/about.qrc                                           \

OTHER_FILES    +=                                           \
    get_version.sh                                          \
    ABOUT                                                   \
    AUTHORS                                                 \
    CHANGELOG                                               \
    COPYING                                                 \
    README                                                  \
    README.md                                               \
    LIBRARIES                                               \

# if you want to test the project, do: qmake "QT += testlib"
contains(QT, testlib) {
    SOURCES   +=                                            \

    HEADERS   +=                                            \

} else {
    SOURCES   +=                                            \
        $$PWD/src/main.cpp                                  \

}

# debug
build_pass:CONFIG(debug, debug|release) {
    win32 {
    }

    unix {
        DEFINES += "G_VERSION=\"\\\"$$system($$PWD/get_version.sh)\\\"\""
    }
}

# release
build_pass:CONFIG(release, debug|release) {
    win32 {
    }

    unix {
        DEFINES += "G_VERSION=\"\\\"$$system($$PWD/get_version.sh)\\\"\""
    }
}
