/**
 * @copyright 2015 Chris Stylianou
 * @copyright 2007 - 2008 Kai Winter
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

// Local includes.
#include "../../qwidgetmap_global.h"
#include "../../Viewport.h"
#include "../../util/Point.h"
#include "../../util/Rect.h"
#include "../Drawable.h"

/// QWidgetMap namespace.
namespace qwm
{

    /// Drawable namespace.
    namespace draw
    {

        /// Geometry namespace.
        namespace geometry
        {

            /// Geometry types.
            enum class GeometryType
            {
                /// Geoemtry that represent an ellipse.
                GeometryEllipse,

                /// Geometry that represents a line string.
                GeometryLineString,

                /// Geometry that represents a point.
                GeometryPoint,

                /// Geometry that represents a polygon.
                GeometryPolygon
            };

            /// Alignment types.
            enum class AlignmentType
            {
                /// Align on middle point.
                Middle,

                /// Align on middle left point.
                MiddleLeft,

                /// Align on middle right point.
                MiddleRight,

                /// Align on top left point.
                TopLeft,

                /// Align on top right point.
                TopRight,

                /// Align on top middle point.
                TopMiddle,

                /// Align on bottom left point.
                BottomLeft,

                /// Align on bottom right point.
                BottomRight,

                /// Align on bottom middle point.
                BottomMiddle
            };

            /**
             * Interface to be implemented to make a drawable geometry item.
             */
            class QWIDGETMAP_EXPORT Geometry : public Drawable
            {
                Q_OBJECT

            protected:

                /**
                 * This constructs a Geometry.
                 * @param geometry_type The geometry type.
                 * @param parent QObject parent ownership.
                 */
                explicit Geometry(const GeometryType& geometry_type, QObject* parent = nullptr);

            public:

                /// Disable copy constructor.
                Geometry(const Geometry&) = delete;

                /// Disable copy assignment.
                Geometry& operator=(const Geometry&) = delete;

                /// Destructor.
                virtual ~Geometry() = default;

            public:

                /**
                 * Fetches the geometry type.
                 * @return the geometry type.
                 */
                const GeometryType& geometryType() const;

                /**
                 * Fetches the pen to draw the geometry with (outline).
                 * @return the QPen to use for drawing.
                 */
                const QPen& pen() const;

                /**
                 * Sets the pen to draw the geometry with (outline).
                 * @param pen The QPen to use for drawing.
                 */
                virtual void setPen(const std::shared_ptr<QPen>& pen);

                /**
                 * Sets the pen to draw the geometry with (outline).
                 * @param pen The QPen to use for drawing.
                 */
                virtual void setPen(const QPen& pen);

                /**
                 * Fetches the brush to draw the geometry with (fill).
                 * @return the QBrush to use for drawing.
                 */
                const QBrush& brush() const;

                /**
                 * Sets the brush to draw the geometry with (fill).
                 * @param brush The QBrush to use for drawing.
                 */
                virtual void setBrush(const std::shared_ptr<QBrush>& brush);

                /**
                 * Sets the brush to draw the geometry with (fill).
                 * @param brush The QBrush to use for drawing.
                 */
                virtual void setBrush(const QBrush& brush);

                /**
                 * Fetches the font to draw the geometry's metadata with.
                 * @return the QFont to use for drawing.
                 */
                const QFont& font() const;

                /**
                 * Sets the font to draw the geometry's metadata with.
                 * @param font The QFont to use for drawing.
                 */
                virtual void setFont(const std::shared_ptr<QFont>& font);

                /**
                 * Sets the font to draw the geometry's metadata with.
                 * @param font The QFont to use for drawing.
                 */
                virtual void setFont(const QFont& font);

                /**
                 * Set the meta-data value to display with the geometry.
                 * @param key The meta-data's key for the value to display.
                 * @param zoom_minimum The minimum zoom to display the meta-data value at.
                 * @param alignment_type The alignment type to use when displaying the meta-data value.
                 * @param alignment_offset_px The offset that the meta-data value is displayed from in pixels.
                 */
                void setMetadataDisplayed(const std::string& key, const int& zoom_minimum = 10, const AlignmentType& alignment_type = AlignmentType::TopRight, const double& alignment_offset_px = 5.0);

                /**
                 * Draws the meta-data to display if set.
                 * @param painter The painter to draw on.
                 * @param viewport The current viewport to use.
                 */
                void drawMetadataDisplayed(QPainter& painter, const Viewport& viewport);

            protected:

                /**
                 * Calculates the top-left world point in pixels after the alignment type has been applied.
                 * @param point_px The world point in pixels to align.
                 * @param alignment_type The alignment type of the world point.
                 * @param geometry_size_px The geometry object (widget/pixmap) at this zoom level.
                 * @return the top-left world point in pixels.
                 */
                util::PointWorldPx calculateTopLeftPoint(const util::PointWorldPx& point_px, const AlignmentType& alignment_type, const QSizeF& geometry_size_px) const;

            public:

                /**
                 * Fetches the bounding box (world coordinates).
                 * @param viewport The current viewport to use.
                 * @return the bounding box.
                 */
                virtual util::RectWorldCoord boundingBox(const Viewport& viewport) const = 0;

                /**
                 * Checks if the geometry touches (intersects) with another geometry.
                 * @param geometry The geometry to check against.
                 * @param viewport The current viewport to use.
                 * @return whether the geometries touch (intersects).
                 */
                virtual bool touches(const Geometry& geometry, const Viewport& viewport) const = 0;

            private:

                /// The geometry type.
                const GeometryType m_geometry_type;

                /// The pen to use when drawing a geometry.
                mutable std::shared_ptr<QPen> m_pen;

                /// The brush to use when drawing a geometry.
                mutable std::shared_ptr<QBrush> m_brush;

                /// The font to use when drawing a geometry's metadata.
                mutable std::shared_ptr<QFont> m_font;

                /// The meta-data's key for the value to display.
                std::string m_metadata_displayed_key;

                /// The minimum zoom to display the meta-data value at.
                int m_metadata_displayed_zoom_minimum { 10 };

                /// The alignment type to use when displaying the meta-data value.
                AlignmentType m_metadata_displayed_alignment_type { AlignmentType::TopRight };

                /// The offset that the meta-data value is displayed from in pixels.
                double m_metadata_displayed_alignment_offset_px { 5.0 };

            };

        }

    }

}
