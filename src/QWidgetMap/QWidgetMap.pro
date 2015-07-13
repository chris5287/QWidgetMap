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
include(../QWidgetMap.pri)

# Build a library.
TEMPLATE = lib

# Target name.
TARGET = $$qtLibraryTarget(qwidgetmap)

# Required defines.
DEFINES += QWIDGETMAP_LIBRARY

# Add Qt modules.
QT +=                                               \
    network                                         \
    widgets                                         \

# Add header files.
HEADERS +=                                          \
    qwidgetmap_global.h                             \
    EventManager.h                                  \
    Layer.h                                         \
    LayerManager.h                                  \
    QWidgetMap.h                                    \
    RenderManager.h                                 \
    Viewport.h                                      \
    ViewportManager.h                               \
    draw/Drawable.h                                 \
    draw/geometry/Geometry.h                        \
    draw/geometry/GeometryEllipse.h                 \
    draw/geometry/GeometryFixed.h                   \
    draw/geometry/GeometryLineString.h              \
    draw/geometry/GeometryPoint.h                   \
    draw/geometry/GeometryPointShape.h              \
    draw/geometry/GeometryPointArrow.h              \
    draw/geometry/GeometryPointCircle.h             \
    draw/geometry/GeometryPointImage.h              \
    draw/geometry/GeometryPointText.h               \
    draw/geometry/GeometryPolygon.h                 \
    draw/map/Map.h                                  \
    draw/map/MapGoogle.h                            \
    draw/map/MapOSM.h                               \
    draw/map/MapTile.h                              \
    projection/Projection.h                         \
    projection/ProjectionEquirectangular.h          \
    projection/ProjectionSphericalMercator.h        \
    util/Algorithms.h                               \
    util/ImageManager.h                             \
    util/InertiaEventManager.h                      \
    util/NetworkManager.h                           \
    util/Point.h                                    \
    util/QuadtreeContainer.h                        \
    util/QProgressIndicator.h                       \
    util/Rect.h                                     \

# Add source files.
SOURCES +=                                          \
    EventManager.cpp                                \
    Layer.cpp                                       \
    LayerManager.cpp                                \
    QWidgetMap.cpp                                  \
    RenderManager.cpp                               \
    Viewport.cpp                                    \
    ViewportManager.cpp                             \
    draw/Drawable.cpp                               \
    draw/geometry/Geometry.cpp                      \
    draw/geometry/GeometryEllipse.cpp               \
    draw/geometry/GeometryFixed.cpp                 \
    draw/geometry/GeometryLineString.cpp            \
    draw/geometry/GeometryPoint.cpp                 \
    draw/geometry/GeometryPointShape.cpp            \
    draw/geometry/GeometryPointArrow.cpp            \
    draw/geometry/GeometryPointCircle.cpp           \
    draw/geometry/GeometryPointImage.cpp            \
    draw/geometry/GeometryPointText.cpp             \
    draw/geometry/GeometryPolygon.cpp               \
    draw/map/Map.cpp                                \
    draw/map/MapGoogle.cpp                          \
    draw/map/MapOSM.cpp                             \
    draw/map/MapTile.cpp                            \
    projection/Projection.cpp                       \
    projection/ProjectionEquirectangular.cpp        \
    projection/ProjectionSphericalMercator.cpp      \
    util/Algorithms.cpp                             \
    util/ImageManager.cpp                           \
    util/InertiaEventManager.cpp                    \
    util/NetworkManager.cpp                         \
    util/QProgressIndicator.cpp                     \

# Add form files.
FORMS +=                                            \
    QWidgetMap.ui                                   \

# Should we also build with GDAL support?
with-gdal {
    message(GDAL support will be included in the build...)

    # Add QWM_GDAL define (for code path detection).
    DEFINES += QWM_GDAL

    # Add additional header files.
    HEADERS +=                                      \
        draw/other/ESRIShapefile.h                  \

    # Add additional source files.
    SOURCES +=                                      \
        draw/other/ESRIShapefile.cpp                \

    # Add GDAL include path.
    INCLUDEPATH += $$(QWM_GDAL_INC)

    # Add GDAL library path and library.
    unix:LIBS += -L$$(QWM_GDAL_LIB) -lgdal
    win32|win64:LIBS += -L$$(QWM_GDAL_LIB) -lgdal_i
}

# Install details.
# Install target to $$prefix()/lib.
target.path = $$prefix()/lib
# Install headers to $$prefix()/include/QWidgetMap
headers.path = $$prefix()/include/QWidgetMap
headers.files = *.h
headers_draw.path = $$prefix()/include/QWidgetMap/draw
headers_draw.files = draw/*.h
headers_draw_geometry.path = $$prefix()/include/QWidgetMap/draw/geometry
headers_draw_geometry.files = draw/geometry/*.h
headers_draw_map.path = $$prefix()/include/QWidgetMap/draw/map
headers_draw_map.files = draw/map/*.h
headers_draw_other.path = $$prefix()/include/QWidgetMap/draw/other
headers_draw_other.files = draw/other/*.h
headers_projection.path = $$prefix()/include/QWidgetMap/projection
headers_projection.files = projection/*.h
headers_util.path = $$prefix()/include/QWidgetMap/util
headers_util.files = util/*.h
# Install headers and target.
INSTALLS += target headers headers_draw headers_draw_geometry headers_draw_map headers_draw_other headers_projection headers_util
