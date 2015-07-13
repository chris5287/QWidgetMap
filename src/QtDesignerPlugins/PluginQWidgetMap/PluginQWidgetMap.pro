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

# Include common configurations.
include(../QtDesignerPlugins.pri)

# Build a library plugin.
CONFIG += plugin
TEMPLATE = lib

# Specify the name of the target file.
TARGET = $$qtLibraryTarget($$TARGET)

# Add the defines.
DEFINES +=                                                          \
    # Mark that this is a Qt Designer Plugin.
    QT_DESIGNER_PLUGIN                                              \
    # Windows: Export functions, data, objects to DLL.
    QDESIGNER_EXPORT_WIDGETS                                        \

# Add header files for each plugins.
HEADERS +=                                                          \
    PluginQWidgetMap.h                                              \

# Add source files for each plugins.
SOURCES +=                                                          \
    PluginQWidgetMap.cpp                                            \

# Add Qt modules.
QT +=                                                               \
    designer                                                        \
    network                                                         \
    widgets                                                         \

# Add QWidgetMap library.
LIBS += -L../../lib -l$$qtLibraryTarget(qwidgetmap)

# Install details.
# Install target to $$prefixPlugin()/plugin.
target.path = $$prefixPlugin()/plugin
# Install libraries to $$prefixPlugin()/plugin.
libraries.path = $$prefixPlugin()/plugin
libraries.extra = $${QMAKE_COPY} $$system_path($$prefixPlugin()/lib/*) $$system_path($$prefixPlugin()/plugin/.)
# Install target and libraries.
INSTALLS += target libraries
