/**
 * @copyright 2015 Chris Stylianou
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

// Qt includes.
#include <QtGui/QBrush>
#include <QtGui/QPen>

// STL includes.
#include <memory>
#include <string>
#include <vector>

// Local includes.
#include "../../qwidgetmap_global.h"
#include "../../Viewport.h"
#include "../Drawable.h"

// Forward-declare gdal-specific resources.
class GDALDataset;
class OGRFeature;

/// QWidgetMap namespace.
namespace qwm
{

    /// Drawable namespace.
    namespace draw
    {

        /// Other/Miscellaneous namespace.
        namespace other
        {

            /**
             * Implementation of the drawable ESRI Shapefile item.
             */
            class QWIDGETMAP_EXPORT ESRIShapefile : public Drawable
            {
                Q_OBJECT

            public:

                /**
                 * This is used to construct a ESRIShapefile.
                 * @param file_path The file path of the ESRI Shapefile.
                 * @param layer_names The name of the layers to draw from the ESRI Shapefile.
                 * @param parent QObject parent ownership.
                 */
                explicit ESRIShapefile(const std::string& file_path, const std::vector<std::string>& layer_names = std::vector<std::string>{}, QObject* parent = nullptr);

                /**
                 * This is used to construct a ESRIShapefile.
                 * @param file_path The file path of the ESRI Shapefile.
                 * @param layer_name The name of the layer to draw from the ESRI Shapefile.
                 * @param parent QObject parent ownership.
                 */
                ESRIShapefile(const std::string& file_path, const std::string& layer_name, QObject* parent = nullptr);

                /// Disable copy constructor.
                ESRIShapefile(const ESRIShapefile&) = delete;

                /// Disable copy assignment.
                ESRIShapefile& operator=(const ESRIShapefile&) = delete;

                /// Destructor.
                ~ESRIShapefile();

            public:

                /**
                 * Fetches the pen to draw the polygon with (outline).
                 * @return the QPen to use for drawing.
                 */
                const QPen& penPolygon() const;

                /**
                 * Sets the pen to draw the polygon with (outline).
                 * @param pen The QPen to use for drawing.
                 */
                void setPenPolygon(const std::shared_ptr<QPen>& pen);

                /**
                 * Sets the pen to draw the polygon with (outline).
                 * @param pen The QPen to use for drawing.
                 */
                void setPenPolygon(const QPen& pen);

                /**
                 * Fetches the brush to draw the polygon with (fill).
                 * @return the QBrush to use for drawing.
                 */
                const QBrush& brushPolygon() const;

                /**
                 * Sets the brush to draw the polygon with (fill).
                 * @param brush The QBrush to use for drawing.
                 */
                void setBrushPolygon(const std::shared_ptr<QBrush>& brush);

                /**
                 * Sets the brush to draw the polygon with (fill).
                 * @param brush The QBrush to use for drawing.
                 */
                void setBrushPolygon(const QBrush& brush);

                /**
                 * Fetches the pen to draw the linestring with (outline).
                 * @return the QPen to use for drawing.
                 */
                const QPen& penLineString() const;

                /**
                 * Sets the pen to draw the linestring with (outline).
                 * @param pen The QPen to use for drawing.
                 */
                void setPenLineString(const std::shared_ptr<QPen>& pen);

                /**
                 * Sets the pen to draw the linestring with (outline).
                 * @param pen The QPen to use for drawing.
                 */
                void setPenLineString(const QPen& pen);

                /**
                 * Draws the item to the provided painter.
                 * @param painter The painter to draw on.
                 * @param drawing_rect_world_coord The drawing rect in world coordinates.
                 * @param viewport The current viewport to use.
                 */
                void draw(QPainter& painter, const util::RectWorldCoord& drawing_rect_world_coord, const Viewport& viewport) const final;

            protected:

                /**
                 * Draws ESRI Shapefile feature to a pixmap using the provided painter.
                 * @param ogr_feature The feature to draw.
                 * @param painter The painter to draw on.
                 * @param viewport The current viewport to use.
                 */
                virtual void drawFeature(OGRFeature* ogr_feature, QPainter& painter, const Viewport& viewport) const;

            private:

                /// The OGR data set of the ESRI Shapefile.
                GDALDataset* m_ogr_data_set { nullptr };

                /// The OGR layer names.
                std::vector<std::string> m_ogr_layer_names;

                /// The pen to use when drawing a polygon.
                mutable std::shared_ptr<QPen> m_pen_polygon;

                /// The brush to use when drawing a polygon.
                mutable std::shared_ptr<QBrush> m_brush_polygon;

                /// The pen to use when drawing a linestring.
                mutable std::shared_ptr<QPen> m_pen_linestring;

            };

        }

    }

}
