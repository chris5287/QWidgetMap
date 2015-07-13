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

# Add support for c++11.
CONFIG += c++11

# Enforce all warnings as errors.
unix:QMAKE_CXXFLAGS += -Werror
win32|win64:QMAKE_CXXFLAGS += /WX

# Hide debug output in release mode.
CONFIG(release, debug|release) : DEFINES += QT_NO_DEBUG_OUTPUT

# Required defines.
DEFINES +=                          \
    # Windows: To force math constants to be defined in <cmath>
    _USE_MATH_DEFINES               \

# Build MOC directory.
MOC_DIR = moc

# Build object directory.
OBJECTS_DIR = objects

# Build resources directory.
RCC_DIR = resources

# Build UI directory.
UI_DIR = ui

# Target install directory.
DESTDIR = ../lib

# Function to generate the target type.
defineReplace(targetType) {
    # Capture whether this is a release/debug build.
    CONFIG(debug, debug|release) {
        return(debug)
    }
    CONFIG(release, debug|release) {
        return(release)
    }
}

# Function to generate the required prefix.
defineReplace(prefix) {
    # Has a prefix been specified?
    !isEmpty(prefix) {
        # Return the prefix path with the target type appended.
        return($${prefix}/$$targetType())
    }
    # Return the default prefix/target type path.
    return(../../$$targetType())
}

# Function to generate the required prefix for plugins.
defineReplace(prefixPlugin) {
    # Has a prefix been specified?
    !isEmpty(prefix) {
        # Return the prefix path with the target type appended.
        return($${prefix}/$$targetType())
    }
    # Return the default prefix/target type path.
    return(../../../$$targetType())
}
