##
# Copyright (C) 2015 Chris Stylianou
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
##

# Include configurations.
include(Example.pri)

# Target name.
TARGET = Example

# Build an application.
TEMPLATE = app

# Add Qt modules.
QT +=                           \
    widgets                     \

# Add header files.
HEADERS +=                      \
    Example.h                   \

# Add source files.
SOURCES +=                      \
    main.cpp                    \
    Example.cpp                 \

# Add QWidgetMap library.
LIBS += -L../lib -l$$qtLibraryTarget(qwidgetmap)

# Should we also build with GDAL support?
with-gdal {
    # Add QWM_GDAL define (for code path detection).
    DEFINES += QWM_GDAL

    # Add GDAL library path and library.
    unix:LIBS += -L$$(QWM_GDAL_LIB) -lgdal
    win32|win64:LIBS += -L$$(QWM_GDAL_LIB) -lgdal_i
}

# Install details.
# Install target to $$prefix()/bin.
target.path = $$prefix()/bin
# Install libraries to $$prefix()/bin.
libraries.path = $$prefix()/bin
libraries.extra = $${QMAKE_COPY} $$system_path($$prefix()/lib/*) $$system_path($$prefix()/bin/.)
# Install target and libraries.
INSTALLS += target libraries
